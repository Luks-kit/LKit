#include "parser.hpp"
#include "error.hpp"
#include <iostream>
#include <string>
#include "lexer.hpp"

void Parser::expect(TokenType type) {
    if (current.type != type) {
    throw ParseError(std::string("Parser::expect: Expected token ") + token_type_to_string(type)
             + " but got " + token_type_to_string(current.type)
             + " at " + std::to_string(current.row) + ":" + std::to_string(current.col));
    }
}

std::unique_ptr<Node> Parser::parse() {
    if (is_decl_kind(current.type)) return parse_decl();
    else if (current.type == TokenType::End) return nullptr;
    else return parse_stmt();
}
