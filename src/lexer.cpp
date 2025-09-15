#include "lexer.hpp"
#include <cctype>
#include <iostream>

Token current;
std::string src;
int pos = 0;

static void skip_whitespace() {
    while (pos < (int)src.size() && isspace(src[pos])) pos++;
}

static bool is_ident_start(char c) {
    return isalpha(c) || c == '_';
}

static bool is_ident_char(char c) {
    return isalnum(c) || c == '_';
}

void reset_lexer(const std::string& input) {
    src = input;
    pos = 0;
    current = get_next_token();
}

Token get_next_token() {
    skip_whitespace();
    if (pos >= (int)src.size()) {
        return {TokenType::End, 0, ""};
    }

    char c = src[pos];

    // numbers
    if (isdigit(c)) {
        int val = 0;
        while (pos < (int)src.size() && isdigit(src[pos])) {
            val = val * 10 + (src[pos] - '0');
            pos++;
        }
        return {TokenType::Number, val, ""};
    }

    // identifiers / keywords
    if (is_ident_start(c)) {
        std::string ident;
        while (pos < (int)src.size() && is_ident_char(src[pos])) {
            ident.push_back(src[pos++]);
        }
        if (ident == "int") return {TokenType::KwInt, 0, ident};
        if (ident == "char") return {TokenType::KwChar, 0, ident};
        if (ident == "bool") return {TokenType::KwBool, 0, ident};
        if (ident == "float") return {TokenType::KwFloat, 0, ident};
        if (ident == "string") return {TokenType::KwString, 0, ident};
        if (ident == "if") return {TokenType::KwIf, 0, ident};
        if (ident == "else") return {TokenType::KwElse, 0, ident};
        if (ident == "while") return {TokenType::KwWhile, 0, ident};
        if (ident == "return") return {TokenType::KwReturn, 0, ident};
        if (ident == "true") return {TokenType::KwTrue, 1, ident};
        if (ident == "false") return {TokenType::KwFalse, 0, ident};
        return {TokenType::Ident, 0, ident};
    }

    // single char tokens
    pos++;
    switch (c) {
        case '+': {
            pos++;
            if (pos < src.size() && src[pos] == '+') {
                pos++;
                return Token{TokenType::Increment, 0, "++"};
            } else if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::PlusEq, 0, "+="};
            }
            return Token{TokenType::Plus, 0, "+"};
            }

        case '-':{
            pos++;
            if (pos < src.size() && src[pos] == '-') {
                pos++;
                return Token{TokenType::Decrement, 0, "--"};
            } else if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::MinusEq, 0, "-="};
            }
            return Token{TokenType::Minus, 0, "-"};
            }

        case '*': {
            pos++;
            if (pos < src.size() && src[pos] == '*') {
                pos++;
                return Token{TokenType::Pow, 0, "**"};
            } else if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::StarEq, 0, "*="};
            }
            return Token{TokenType::Star, 0, "*"};
            }


        case '/': {
            pos++;
            if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::SlashEq, 0, "/="};
            } 
            return Token{TokenType::Slash, 0, "/"};
        }

        case '&': {
            pos++;
            if (pos < src.size() && src[pos] == '&') {
                pos++;
                return Token{TokenType::BoolAnd, 0, "&&"};
            } else if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::AmpEq, 0, "&="};
            }
            return Token{TokenType::Amp, 0, "&"};
            }
        case '|': {
            pos++;
            if (pos < src.size() && src[pos] == '|') {
                pos++;
                return Token{TokenType::BoolOr, 0, "||"};
            } else if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::PipeEq, 0, "|="};}
            
            return Token{TokenType::Pipe, 0, "|"};
            }
        case '^': {
            if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::CaretEq, 0, "^="};
            }
            return Token{TokenType::Caret, 0, "^"};
        }
        case '~': return {TokenType::Tilde, 0, std::string(1,'~')};
        case '<': {
            if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::Le, 0, "<="};
            }
            return Token{TokenType::Lt, 0, "<"};
        }
        case '>': { 
            if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::Ge, 0, ">="};
            }
            return Token{TokenType::Gt, 0, ">"};
        }
        case '=': {
             if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::EqEq, 0, "=="};
            }
            return Token{TokenType::Eq, 0, "="};
        }
        case '!': {
            pos++;
            if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::NotEq, 0, "!="};
            }
            return Token{TokenType::Not, 0, "!"};
        }

        case ';': return {TokenType::Semi, 0, std::string(1, ';')};
        case '(': return {TokenType::LParen, 0, std::string(1, '(')};
        case ')': return {TokenType::RParen, 0, std::string(1, ')')};
        case '{': return {TokenType::LBrace, 0, std::string(1, '{')};
        case '}': return {TokenType::RBrace, 0, std::string(1, '}')};
}
    
    std::cerr << "Unknown character: " << c << "\n";
    return {TokenType::End, 0, ""};
}

void advance() {
    current = get_next_token();
}

