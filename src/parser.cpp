#include "parser.hpp"
#include "lexer.hpp"
#include <iostream>
#include <cstdlib>
#include <string>

extern Token current;
void advance();

// Forward declarations
static AST* parse_expr();
static AST* parse_check();
static AST* parse_comparison();
static AST* parse_stmt();


static void expect(TokenType type) {
    if (current.type != type) {
        std::cerr << "Expected token " << (int)type 
                  << " but got " << (int)current.type << "\n";
        exit(1);
    }
    advance();
}

AST* parse_recheck() {
    advance(); // consume 'while'

    if (current.type != TokenType::LParen) { std::cerr << "Expected (\n"; exit(1);}
    advance();

    AST* cond = parse_comparison(); // condition uses full precedence

    if (current.type != TokenType::RParen) { std::cerr << "Expected )\n"; exit(1);}
    advance();

    AST* body = parse_stmt(); // single statement or a block

    return AST::make_recheck(cond, body);
}

// Factor: numbers, chars, identifiers, parentheses
static AST* parse_factor() {
    if (current.type == TokenType::Number) {
        AST* n = AST::make_number(current.value);
        advance();
        return n;
    }
    if (current.type == TokenType::Char) {
        AST* n = AST::make_char(current.value);
        advance();
        return n;
    }
    if (current.type == TokenType::Ident) {
        AST* n = AST::make_ident(current.lexeme);
        advance();
        return n;
    }
    if (current.type == TokenType::LParen) {
        advance(); // consume '('
        AST* n = parse_expr(); // full precedence
        if (current.type != TokenType::RParen) {
            std::cerr << "Expected ) in parse_factor\n"; exit(1);
        }
        advance(); // consume ')'
        return n;
    }
    std::cerr << "Unexpected token in factor:" << current.lexeme << "\n";
    exit(1);
}

// Unary operators: ~ (bitwise NOT), - (negation)
AST* parse_unary() {
    if (current.type == TokenType::Tilde) {
        advance();
        AST* expr = parse_unary();
        return AST::make_binop("~", nullptr, expr);
    }
    if (current.type == TokenType::Minus) {
        advance();
        AST* expr = parse_unary();
        return AST::make_binop("neg", nullptr, expr);
    }
    

    return parse_factor();
}

// Term: * /
AST* parse_term() {
    AST* n = parse_unary();
    while (current.type == TokenType::Star || current.type == TokenType::Slash) {
        std::string op = (current.type == TokenType::Star) ? "*" : "/";
        advance();
        AST* right = parse_unary();
        n = AST::make_binop(op, n, right);
    }
    return n;
}

// Additive: + -
AST* parse_additive() {
    AST* n = parse_term();
    while ( current.type == TokenType::Plus || current.type == TokenType::Minus) {
        std::string op = (current.type == TokenType::Plus) ? "+" : "-";
        advance();
        AST* right = parse_term();
        n = AST::make_binop(op, n, right);
    }
    return n;
}

// Bitwise AND: &
AST* parse_bitwise_and() {
    AST* n = parse_additive();
    while (current.type == TokenType::Amp) {
        advance();
        AST* right = parse_additive();
        n = AST::make_binop("&", n, right);
    }
    return n;
}

// Bitwise OR: |
AST* parse_bitwise_or() {
    AST* n = parse_bitwise_and();
    while (current.type == TokenType::Pipe) {
        advance();
        AST* right = parse_bitwise_and();
        n = AST::make_binop("|", n, right);
    }
    return n;
}

// Comparison: < > <= >= == !=
static AST* parse_comparison() {
    AST* n = parse_bitwise_or();
    while (current.type == TokenType::Lt || current.type == TokenType::Gt ||
           current.type == TokenType::Le || current.type == TokenType::Ge ||
           current.type == TokenType::EqEq || current.type == TokenType::NotEq ||
           current.type == TokenType::BoolAnd || current.type == TokenType::BoolOr)
    {
        std::string op;
        switch (current.type) {
            case TokenType::Lt:    op = "<"; break;
            case TokenType::Gt:    op = ">"; break;
            case TokenType::Le:    op = "<="; break;
            case TokenType::Ge:    op = ">="; break;
            case TokenType::EqEq:  op = "=="; break;
            case TokenType::NotEq: op = "!="; break;
            case TokenType::BoolAnd:op = "&&"; break;
            case TokenType::BoolOr: op = "||"; break; 
            default: std::cerr << "Unknown comparison\n"; exit(1);
        }
        advance();
        AST* right = parse_bitwise_or();
        n = AST::make_binop(op, n, right);
    }
    return n;
}

// Full expression
AST* parse_expr() {
    return parse_comparison();
}

static AST* parse_block() {
    expect(TokenType::LBrace);
    std::vector<AST*> stmts;
    while (current.type != TokenType::RBrace && current.type != TokenType::End) {
        stmts.push_back(parse_stmt());
    }
    expect(TokenType::RBrace);
    return AST::make_block(stmts);
}

// Statement
static AST* parse_stmt() {
    if (current.type == TokenType::KwCheck) return parse_check();
    if (current.type == TokenType::LBrace) { return parse_block();}
    if (current.type == TokenType::KwRecheck) {return parse_recheck();}
    if (current.type == TokenType::Ident) {
        std::string name = current.lexeme;
        advance();
        if (current.type == TokenType::Eq) {
            advance();
            AST* rhs = parse_expr(); // full precedence
            if (current.type != TokenType::Semi) { std::cerr << "Expected ; in parse_stmt\n"; exit(1); }
            advance(); 
            return AST::make_assign(name, rhs);
        }
        if(current.type == TokenType::PlusEq || current.type == TokenType::MinusEq ||
        current.type == TokenType::StarEq || current.type == TokenType::SlashEq) {

        char op = (current.type == TokenType::PlusEq) ? '+' :
                  (current.type == TokenType::MinusEq) ? '-' :
                  (current.type == TokenType::StarEq) ? '*' : '/';
        advance(); 
        AST* rhs = parse_expr();
        expect(TokenType::Semi);
        return AST::make_assign_op(name, rhs, op);
        }  

        if (current.type == TokenType::Increment || current.type == TokenType::Decrement) {
        char op = (current.type == TokenType::Increment) ? '+' : '-';
        advance();
        expect(TokenType::Semi);
        return AST::make_incdec(name, op);
    }

    }
    std::cerr << "Invalid statement\n";
    exit(1);
}

// If statement
AST* parse_check() {
    advance(); // consume 'if'
    if (current.type != TokenType::LParen) { std::cerr << "Expected ( in parse_check\n"; exit(1);}
    advance(); // consume '('
    AST* cond = parse_expr();
    if (current.type != TokenType::RParen) { std::cerr << "Expected ) in parse_check\n"; exit(1);}
    advance(); // consume ')'
    AST* then_branch = parse_stmt();
    AST* else_branch = nullptr;
    if (current.type == TokenType::KwThen) {
        advance();
        else_branch = parse_stmt();
    }
    return AST::make_check(cond, then_branch, else_branch);
}

// Entry point
AST* parse() {
    return parse_stmt();
}

