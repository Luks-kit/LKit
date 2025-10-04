#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "ast.hpp"

AST* parse(Lexer& lex);  // entry point

#endif

