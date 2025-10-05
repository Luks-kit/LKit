#include "parser.hpp"
#include "error.hpp"
#include <iostream>
#include <string>
#include "lexer.hpp"

void Parser::expect(TokenType type) {
    if (current.type != type) {
        throw ParseError(std::string("Expected token ") + token_type_to_string(type)
                         + " but got " + token_type_to_string(current.type));
    }
}