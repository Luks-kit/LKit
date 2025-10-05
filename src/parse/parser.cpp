#include "parser.hpp"
#include "lexer.hpp"
#include "error.hpp"
#include <iostream>
#include <string>

// Forward declarations
static AST* parse_expr(Lexer& lex);
static AST* parse_check(Lexer& lex);
static AST* parse_comparison(Lexer& lex);
static AST* parse_stmt(Lexer& lex);


static void expect(Lexer& lex, TokenType type) {
    if (lex.current.type != type) {
        throw ParseError(std::string("Expected token ") + token_type_to_string(type)
                         + " but got " + token_type_to_string(lex.current.type));
    }
    lex.advance();
}

static void light_expect(Lexer& lex, TokenType type) {
    if (lex.current.type != type) {
        throw ParseError(std::string("Expected token ") + token_type_to_string(type)
                         + " but got " + token_type_to_string(lex.current.type));
    }
}

AST* parse_recheck(Lexer& lex) {
    lex.advance(); // consume 'while'
    expect(lex, TokenType::LParen);
    AST* cond = parse_comparison(lex); // condition uses full precedence
    
    expect(lex, TokenType::RParen);
    AST* body = parse_stmt(lex); // single statement or a block

    return AST::make_recheck(cond, body);
}

// Factor: numbers, chars, identifiers, parentheses
static AST* parse_factor(Lexer& lex) {
    if (lex.current.type == TokenType::Literal) {
        AST* n = AST::make_literal(lex.current.value);
        lex.advance();
        return n;
    }
    if (lex.current.type == TokenType::Ident) {
        AST* n = AST::make_ident(lex.current.lexeme);
        lex.advance();
        return n;
    }
    if (lex.current.type == TokenType::LParen) {
        lex.advance(); // consume '('
        AST* n = parse_expr(lex); // full precedence
        if (lex.current.type != TokenType::RParen) {
                throw ParseError("Expected ) in parse_factor");
            }
        lex.advance(); // consume ')'
        return n;
    }
    throw ParseError(std::string("Unexpected token in factor: ") + lex.current.lexeme);
}

// Unary operators: ~ (bitwise NOT), - (negation)
AST* parse_unary(Lexer& lex) {
    if (lex.current.type == TokenType::Tilde) {
        lex.advance();
        AST* expr = parse_unary(lex);
        return AST::make_binop("~", nullptr, expr);
    }
    if (lex.current.type == TokenType::Minus) {
        lex.advance();
        AST* expr = parse_unary(lex);
        return AST::make_binop("neg", nullptr, expr);
    }

    return parse_factor(lex);
}

// Term: * /
AST* parse_term(Lexer& lex) {
    AST* n = parse_unary(lex);
    while (lex.current.type == TokenType::Star || lex.current.type == TokenType::Slash) {
        std::string op = (lex.current.type == TokenType::Star) ? "*" : "/";
        lex.advance();
        AST* right = parse_unary(lex);
        n = AST::make_binop(op, n, right);
    }
    return n;
}

// Additive: + -
AST* parse_additive(Lexer& lex) {
    AST* n = parse_term(lex);
    while ( lex.current.type == TokenType::Plus || lex.current.type == TokenType::Minus) {
        std::string op = (lex.current.type == TokenType::Plus) ? "+" : "-";
        lex.advance();
        AST* right = parse_term(lex);
        n = AST::make_binop(op, n, right);
    }
    return n;
}

// Bitwise AND: &
AST* parse_bitwise_and(Lexer& lex) {
    AST* n = parse_additive(lex);
    while (lex.current.type == TokenType::Amp) {
        lex.advance();
        AST* right = parse_additive(lex);
        n = AST::make_binop("&", n, right);
    }
    return n;
}

// Bitwise OR: |
AST* parse_bitwise_or(Lexer& lex) {
    AST* n = parse_bitwise_and(lex);
    while (lex.current.type == TokenType::Pipe) {
        lex.advance();
        AST* right = parse_bitwise_and(lex);
        n = AST::make_binop("|", n, right);
    }
    return n;
}

// Comparison: < > <= >= == !=
static AST* parse_comparison(Lexer& lex) {
    AST* n = parse_bitwise_or(lex);
    while ( lex.current.type == TokenType::Lt || lex.current.type == TokenType::Gt ||
           lex.current.type == TokenType::Le || lex.current.type == TokenType::Ge ||
           lex.current.type == TokenType::EqEq || lex.current.type == TokenType::NotEq ||
           lex.current.type == TokenType::BoolAnd || lex.current.type == TokenType::BoolOr)
    {
        std::string op;
        switch (lex.current.type) {
            case TokenType::Lt:    op = "<"; break;
            case TokenType::Gt:    op = ">"; break;
            case TokenType::Le:    op = "<="; break;
            case TokenType::Ge:    op = ">="; break;
            case TokenType::EqEq:  op = "=="; break;
            case TokenType::NotEq: op = "!="; break;
            case TokenType::BoolAnd:op = "&&"; break;
            case TokenType::BoolOr: op = "||"; break; 
            default: throw ParseError("Unknown comparison");
        }
        lex.advance();
        AST* right = parse_bitwise_or(lex);
        n = AST::make_binop(op, n, right);
    }
    return n;
}

// Full expression
AST* parse_expr(Lexer& lex) {
    return parse_comparison(lex);
}

static AST* parse_block(Lexer& lex) {
    expect(lex, TokenType::LBrace);
    std::vector<AST*> stmts;
    while (lex.current.type != TokenType::RBrace && lex.current.type != TokenType::End) {
        stmts.push_back(parse_stmt(lex));
    }
   
    expect(lex, TokenType::RBrace); // consume '}'
    return AST::make_block(stmts);
}

static AST* parse_assign (Lexer& lex) {
        std::string name = lex.current.lexeme;
        lex.advance();
        if (lex.current.type == TokenType::Eq) {
            lex.advance();
            AST* rhs = parse_expr(lex); // full precedence
            expect(lex, TokenType::Semi);
            lex.advance(); 
            return AST::make_assign(name, rhs);
        }
        if(lex.current.type == TokenType::PlusEq || lex.current.type == TokenType::MinusEq ||
        lex.current.type == TokenType::StarEq || lex.current.type == TokenType::SlashEq) {

        char op = (lex.current.type == TokenType::PlusEq) ? '+' :
                  (lex.current.type == TokenType::MinusEq) ? '-' :
                  (lex.current.type == TokenType::StarEq) ? '*' : '/';
        lex.advance(); 
        AST* rhs = parse_expr(lex);
        expect(lex, TokenType::Semi);
        return AST::make_assign_op(name, rhs, op);
        }  

        if (lex.current.type == TokenType::Increment || lex.current.type == TokenType::Decrement) {
        char op = (lex.current.type == TokenType::Increment) ? '+' : '-';
        lex.advance();
        expect(lex, TokenType::Semi);
        return AST::make_incdec(name, op);
        }    
        
    // If we reach here, the identifier wasn't followed by a valid assignment operator
    throw ParseError(std::string("Invalid assignment statement for identifier '") + name + "'");
}

static AST* parse_decl(Lexer& lex){
    lex.advance();
    light_expect(lex, TokenType::Ident);
    std::string name = lex.current.lexeme;
    lex.advance();
        
    if (lex.current.type == TokenType::Eq) {
        lex.advance();
        AST* rhs = parse_expr(lex);
        expect(lex, TokenType::Semi);
        return AST::make_decl(name, rhs); 
    } else if (lex.current.type == TokenType::Semi) {
        lex.advance();
        return AST::make_decl(name, AST::make_literal(0));
    }
    // If we reach here, declaration syntax was invalid
    throw ParseError(std::string("Invalid declaration for '") + name + "'");
}

// Statement
static AST* parse_stmt(Lexer& lex) {
    if (lex.current.type == TokenType::KwCheck) return parse_check(lex);
    if (lex.current.type == TokenType::LBrace) { return parse_block(lex);}
    if (lex.current.type == TokenType::KwRecheck) {return parse_recheck(lex);}
    if (lex.current.type == TokenType::KwLet) { return parse_decl(lex); }
    if (lex.current.type == TokenType::Ident) { return parse_assign(lex);}
    
    throw ParseError("Invalid statement");
}

// If statement
AST* parse_check(Lexer& lex) {
    lex.advance(); // consume 'if'
    if (lex.current.type != TokenType::LParen) { throw ParseError("Expected ( in parse_check"); }
    lex.advance(); // consume '('
    AST* cond = parse_expr(lex);
    if (lex.current.type != TokenType::RParen) { throw ParseError("Expected ) in parse_check"); }
    lex.advance(); // consume ')'
    AST* then_branch = parse_stmt(lex);
    AST* else_branch = nullptr;
    if (lex.current.type == TokenType::KwThen) {
        lex.advance();
        else_branch = parse_stmt(lex);
    }
    return AST::make_check(cond, then_branch, else_branch);
}

// Entry point
AST* parse(Lexer& lex) {
    return parse_stmt(lex);
}




