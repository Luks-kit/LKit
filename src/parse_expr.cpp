#include <iostream>
#include <cstdlib>
#include <string>
#include "parser.hpp"
#include "lexer.hpp"
#include "stmt.hpp"
#include "expr.hpp"

std::unique_ptr<Expr> Parser::parse_literal() {
    if (current.type != TokenType::Literal)
        throw std::runtime_error("Expected literal");

    auto lit = std::make_unique<LiteralExpr>(current.value);
    advance();
    return lit;
}

std::unique_ptr<Expr> Parser::parse_factor() {
    if (current.type == TokenType::Literal) {
        return parse_literal();
    }
    if (current.type == TokenType::Ident) {
        auto ident = std::make_unique<IdentExpr>(current.lexeme);
        advance();
        return ident;
    }
    if (current.type == TokenType::LParen) {
        advance(); // consume '('
        auto expr = parse_expr();
        expect(TokenType::RParen);
        return expr;
    }
    throw std::runtime_error("Unexpected token in factor");
}    
        
std::unique_ptr<Expr> Parser::parse_unary() {
    if (current.type == TokenType::Tilde) {
        advance();
        auto expr = parse_unary();
        return std::make_unique<UnaryExpr>("~", std::move(expr));
    }
    if (current.type == TokenType::Minus) {
        advance();
        auto expr = parse_unary();
        return std::make_unique<UnaryExpr>("neg", std::move(expr));
    }

    return parse_factor();
}

std::unique_ptr<Expr> Parser::parse_term() {
    auto n = parse_unary();
    while (current.type == TokenType::Star || current.type == TokenType::Slash) {
        std::string op = (current.type == TokenType::Star) ? "*" : "/";
        advance();
        auto right = parse_unary();
        n = std::make_unique<BinaryExpr>(op, std::move(n), std::move(right));
    }
    return n;
}

std::unique_ptr<Expr> Parser::parse_additive() {
    auto n = parse_term();
    while ( current.type == TokenType::Plus || current.type == TokenType::Minus) {
        std::string op = (current.type == TokenType::Plus) ? "+" : "-";
        advance();
        auto right = parse_term();
        n = std::make_unique<BinaryExpr>(op, std::move(n), std::move(right));
    }
    return n;
}

std::unique_ptr<Expr> Parser::parse_bitwise_and() {
    auto n = parse_additive();
    while (current.type == TokenType::Amp) {
        advance();
        auto right = parse_additive();
        n = std::make_unique<BinaryExpr>("&", std::move(n), std::move(right));
    }
    return n;
}

std::unique_ptr<Expr> Parser::parse_bitwise_or() {
    auto n = parse_bitwise_and();
    while (current.type == TokenType::Pipe) {
        advance();
        auto right = parse_bitwise_and();
        n = std::make_unique<BinaryExpr>("|", std::move(n), std::move(right));
    }
    return n;
}   

std::unique_ptr<Expr> Parser::parse_comparison() {
    auto n = parse_bitwise_or();
    while (current.type == TokenType::Lt || current.type == TokenType::Le ||
           current.type == TokenType::Gt || current.type == TokenType::Ge ||
           current.type == TokenType::EqEq || current.type == TokenType::NotEq) {
        std::string op;
        switch (current.type) {
            case TokenType::Lt: op = "<"; break;
            case TokenType::Le: op = "<="; break;
            case TokenType::Gt: op = ">"; break;
            case TokenType::Ge: op = ">="; break;
            case TokenType::EqEq: op = "=="; break;
            case TokenType::NotEq: op = "!="; break;
            case TokenType::BoolAnd: op = "&&"; break;
            case TokenType::BoolOr: op = "||"; break;
            default: throw std::runtime_error("Invalid comparison operator");
        }  
        advance();
        auto right = parse_bitwise_or();
        n = std::make_unique<BinaryExpr>(op, std::move(n), std::move(right));
    }
    return n;
}

std::unique_ptr<Expr> Parser::parse_expr() {
    return parse_comparison();
}