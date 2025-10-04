// parser.hpp
#pragma once
#include "expr.hpp"
#include "stmt.hpp"
#include "decl.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include <memory>

class Parser {
public:
    explicit Parser(Lexer& l) : lexer(l) { advance(); }

    std::unique_ptr<Node> parse();

private:
    Lexer& lexer;
    Token current;

    void advance() { current = lexer.get_next_token(); }
    void expect(TokenType type);

    std::unique_ptr<Expr> parse_expr();
    std::unique_ptr<Stmt> parse_stmt();
    std::unique_ptr<Decl> parse_decl();
};
AST* parse(Lexer& lex); // old entry point
