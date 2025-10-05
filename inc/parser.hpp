// parser.hpp
#pragma once
#include "expr.hpp"
#include "stmt.hpp"
#include "decl.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include <memory>
#include <vector>


struct Parser {
    explicit Parser(Lexer& l) : lexer(l) { lexer.advance(); }
    Parser() = default;

    std::unique_ptr<Node> parse(); // new entry point
    Lexer& lexer;
    Token current;

    void advance() { current = lexer.get_next_token(); lexer.advance(); }
    void expect(TokenType type);

    std::unique_ptr<Expr> parse_expr();
        std::unique_ptr<Expr> parse_literal();
        std::unique_ptr<Expr> parse_factor();
        std::unique_ptr<Expr> parse_term();
        std::unique_ptr<Expr> parse_additive();
        std::unique_ptr<Expr> parse_bitwise_and();
        std::unique_ptr<Expr> parse_bitwise_or();
        std::unique_ptr<Expr> parse_comparison();
        std::unique_ptr<Expr> parse_unary();
        std::unique_ptr<Expr> parse_call();
    std::unique_ptr<Stmt> parse_stmt();
        std::unique_ptr<Stmt> parse_block();
        std::unique_ptr<Stmt> parse_recheck();
        std::unique_ptr<Stmt> parse_check();
        std::unique_ptr<Stmt> parse_if();
        std::unique_ptr<Stmt> parse_while();
        std::unique_ptr<Stmt> parse_for();
        std::unique_ptr<Stmt> parse_assign();
        std::unique_ptr<Stmt> parse_expr_stmt();
    std::unique_ptr<Decl> parse_decl();
        std::unique_ptr<Decl> parse_var_decl();
        std::unique_ptr<Decl> parse_subr_decl();
        std::unique_ptr<Decl> parse_struct_decl();
        std::unique_ptr<Decl> parse_enum_decl();
        std::unique_ptr<Decl> parse_union_decl();
        std::unique_ptr<Decl> parse_tool_decl();
        std::unique_ptr<Decl> parse_kit_decl();
};


AST* parse(Lexer& lex); // old entry point to be removed
