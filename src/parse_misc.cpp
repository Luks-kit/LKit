#include "parser.hpp"
#include <iostream>
#include <cstdlib>
#include <string>
#include "lexer.hpp"

void Parser::expect(TokenType type) {
    if (current.type != type) {
        std::cerr << "Expected token " << token_type_to_string(type) 
                  << " but got " << token_type_to_string(current.type) << "\n";
        exit(1);
    }
}