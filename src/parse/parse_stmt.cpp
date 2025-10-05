#include <iostream>
#include <cstdlib>
#include <string>
#include "parser.hpp"
#include "lexer.hpp"
#include "stmt.hpp"
#include "expr.hpp"

std::unique_ptr<Stmt> Parser::parse_check() {
    advance(); // consume 'check'
    expect(TokenType::LParen);
    auto expr = parse_expr(); // full precedence
    expect(TokenType::RParen);

    CheckArms arms;
    if (current.type == TokenType::KwOnly) {
        advance(); // consume 'only'
        arms.else_arm = nullptr;
        arms.arms.emplace_back(nullptr, parse_stmt());
        return std::make_unique<CheckStmt>(std::move(expr), std::move(arms));
    } else if (current.type == TokenType::KwOn) {
        advance(); // consume 'on'
        while (current.type != TokenType::KwThen && current.type != TokenType::End) {
            expect(TokenType::KwCase);
            auto case_expr = parse_expr();
            expect(TokenType::Colon);
            auto case_stmt = parse_stmt();
            arms.arms.emplace_back(std::move(case_expr), std::move(case_stmt));
        }
        if (current.type == TokenType::KwThen) {
            advance(); // consume 'else'
            arms.else_arm = parse_stmt();
        }
        return std::make_unique<CheckStmt>(std::move(expr), std::move(arms));
    } else {
        throw std::runtime_error("Expected 'only' or 'on' after check(expr)");
    }
}

std::unique_ptr<Stmt> Parser::parse_recheck() {
    advance(); // consume 'recheck'
    expect(TokenType::LParen);
    auto expr = parse_expr(); // full precedence
    expect(TokenType::RParen);

    CheckArms arms;
    while (current.type != TokenType::End) {
        expect(TokenType::KwCase);
        auto case_expr = parse_expr();
        expect(TokenType::Colon);
        auto case_stmt = parse_stmt();
        arms.arms.emplace_back(std::move(case_expr), std::move(case_stmt));
    }
    return std::make_unique<RecheckStmt>(std::move(expr), std::move(arms));
}

std::unique_ptr<Stmt> Parser::parse_block() {
    advance(); // consume '{'
    auto block = std::make_unique<BlockStmt>();
    while (current.type != TokenType::RBrace && current.type != TokenType::End) {
        block->stmts.push_back(parse_stmt());
    }
    expect(TokenType::RBrace);
    return block;
}

std::unique_ptr<Stmt> Parser::parse_assign() {
    std::string name = current.lexeme;
    advance();
    if (current.type == TokenType::Eq) {
        advance();
        auto rhs = parse_expr(); // full precedence
        expect(TokenType::Semi);
        advance(); 
        return std::make_unique<ExprStmt>(std::make_unique<BinaryExpr>("=", std::make_unique<IdentExpr>(name), std::move(rhs)));
    }
    if(current.type == TokenType::PlusEq || current.type == TokenType::MinusEq ||
       current.type == TokenType::StarEq || current.type == TokenType::SlashEq) {

        char op = (current.type == TokenType::PlusEq) ? '+' :
                  (current.type == TokenType::MinusEq) ? '-' :
                  (current.type == TokenType::StarEq) ? '*' : '/';
        advance(); 
        auto rhs = parse_expr();
        expect(TokenType::Semi);
        return std::make_unique<ExprStmt>(std::make_unique<BinaryExpr>(std::string(1, op) + "=", std::make_unique<IdentExpr>(name), std::move(rhs)));
    }  

    if (current.type == TokenType::Increment || current.type == TokenType::Decrement) {
        char op = (current.type == TokenType::Increment) ? '+' : '-';
        advance();
        expect(TokenType::Semi);
        return std::make_unique<ExprStmt>(std::make_unique<BinaryExpr>(std::string(1, op) + "=", std::make_unique<IdentExpr>(name), std::make_unique<BinaryExpr>("+", std::make_unique<IdentExpr>(name), std::make_unique<LiteralExpr>(Value(1)))));
    }    
    throw std::runtime_error("Expected assignment operator");
}

std::unique_ptr<Stmt> Parser::parse_if() {
    advance(); // consume 'if'
    expect(TokenType::LParen);
    auto cond = parse_expr(); // full precedence
    expect(TokenType::RParen);
    auto then_branch = parse_stmt(); // single statement or a block
    std::unique_ptr<Stmt> else_branch = nullptr;
    if (current.type == TokenType::KwElse) {
        advance(); // consume 'else'
        else_branch = parse_stmt();
    }
    return std::make_unique<IfStmt>(std::move(cond), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<Stmt> Parser::parse_while() {
    advance(); // consume 'while'
    expect(TokenType::LParen);
    auto cond = parse_expr(); // full precedence
    expect(TokenType::RParen);
    auto body = parse_stmt(); // single statement or a block
    return std::make_unique<WhileStmt>(std::move(cond), std::move(body));
}

std::unique_ptr<Stmt> Parser::parse_for() {
    advance(); // consume 'for'
    expect(TokenType::LParen);
    std::string var_name;
    if (current.type == TokenType::Ident) {
        var_name = current.lexeme;
        advance();
    } else {
        throw std::runtime_error("Expected identifier in for-each");
    }
    expect(TokenType::Colon);
    auto iterable = parse_expr(); // full precedence
    expect(TokenType::RParen);
    auto body = parse_stmt(); // single statement or a block
    return std::make_unique<ForEachStmt>(var_name, std::move(iterable), std::move(body));
}



std::unique_ptr<Stmt> Parser::parse_stmt() {
    if (current.type == TokenType::KwCheck) return parse_check();
    if (current.type == TokenType::LBrace) return parse_block();
    if (current.type == TokenType::KwRecheck) return parse_recheck();
    // if (current.type == TokenType::KwLet) return parse_decl(); // not yet implemented
    if (current.type == TokenType::Ident) return parse_assign();
    throw std::runtime_error("Invalid statement");
}


std::unique_ptr<Expr> Parser::parse_call() {
    auto callee = parse_factor();
    while (current.type == TokenType::LParen) {
        advance(); // consume '('
        std::vector<std::unique_ptr<Expr>> args;
        if (current.type != TokenType::RParen) {
            while (true) {
                args.push_back(parse_expr());
                if (current.type == TokenType::Comma) {
                    advance(); // consume ','
                } else {
                    break;
                }
            }
        }
        expect(TokenType::RParen);
        callee = std::make_unique<CallExpr>(std::move(callee), std::move(args));
    }
    return callee;
}