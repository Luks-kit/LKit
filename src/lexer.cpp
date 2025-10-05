#include "lexer.hpp"
#include <cctype>
#include <iostream>
#include <optional>

Token Lexer::get_next_token() {
    skip_whitespace();
    if (pos >= src.size()) {
        return {TokenType::End, 0, ""};
    }

    char c = src[pos];

    Token literal = literal_check();
    if (literal.type == TokenType::Literal) return literal;

    // identifiers / keywords
    if (is_ident_start(c)) {
        std::string ident;
        while (pos < src.size() && is_ident_char(src[pos])) {
            ident.push_back(src[pos++]);
        }
        return ident_check(ident);
    }

    // single char tokens...
    Token op_return = op_check();

    if(op_return.type != TokenType::End) {return op_return; }

    std::cerr << "Unknown character: " << c << "\n";
    return {TokenType::End, 0, ""};
}

Token Lexer::ident_check(const std::string& ident){

     // keyword checks...
        if (ident == "int") return {TokenType::KwType, 0, ident};
        if (ident == "short") return {TokenType::KwType, 0, ident};
        if (ident == "long") return {TokenType::KwType, 0, ident};
        if (ident == "char") return {TokenType::KwType, 0, ident};
        if (ident == "bool") return {TokenType::KwType, 0, ident};
        if (ident == "float") return {TokenType::KwType, 0, ident};
        if (ident == "double") return {TokenType::KwType, 0, ident};
        if (ident == "string") return {TokenType::KwType, 0, ident};
        if (ident == "const") return {TokenType::KwConst, 0, ident};
        if (ident == "check") return {TokenType::KwCheck, 0, ident};
        if (ident == "then") return {TokenType::KwThen, 0, ident};
        if (ident == "on") return {TokenType::KwOn, 0, ident};
        if (ident == "recheck") return {TokenType::KwRecheck, 0, ident};
        if (ident == "return") return {TokenType::KwReturn, 0, ident};
        if (ident == "true") return {TokenType::KwTrue, true, ident};
        if (ident == "false") return {TokenType::KwFalse, false, ident};
        if (ident == "let") return {TokenType::KwLet, 0, ident};
        if (ident == "subr") return {TokenType::KwSubr, 0 , ident};
        return {TokenType::Ident, 0, ident};
}

Token Lexer::literal_check(){
    char c = src[pos];

    if (isdigit(c)) {
    // Number literal
    int start = pos;
    bool isFloat = false;

    while (pos < src.size() && isdigit(src[pos])) pos++;

    if (pos < src.size() && src[pos] == '.') {
        isFloat = true;
        pos++;
        while (pos < src.size() && isdigit(src[pos])) pos++;
    }

    std::string numStr = src.substr(start, pos - start);
    Value val;

    if (isFloat)
        val.set<float>(std::stof(numStr));
    else
        val.set<int>(std::stoi(numStr));

    return Token{TokenType::Literal, val, ""};
    }

    // Boolean literals
    if (match_str("true")) {
        Value val;
        val.set<bool>(true);
        return Token{TokenType::Literal, val, "true"};
    }
    if (match_str("false")) {
        Value val;
        val.set<bool>(false);
        return Token{TokenType::Literal, val, "false"};
    }

    // Character literal
    if (c == '\'') {
        pos++; // skip opening '
        if (pos >= src.size()) throw std::runtime_error("Unterminated char literal");

        char ch = src[pos++];
        if (ch == '\\') {
            if (pos >= src.size()) throw std::runtime_error("Invalid escape sequence");
            char esc = src[pos++];
            switch (esc) {
                case 'n': ch = '\n'; break;
                case 't': ch = '\t'; break;
                case 'r': ch = '\r'; break;
                case '\\': ch = '\\'; break;
                case '\'': ch = '\''; break;
                case '0': ch = '\0'; break;
                default: throw std::runtime_error("Unknown escape in char literal");
            }
        }

        if (pos >= src.size() || src[pos] != '\'')
            throw std::runtime_error("Unterminated char literal");
        pos++; // skip closing '

        Value val;
        val.set<char>(ch);
        return Token{TokenType::Literal, val, ""};
    }

    // String literal
    if (c == '"') {
        pos++; // skip opening "
        std::string s;
        while (pos < src.size() && src[pos] != '"') {
            if (src[pos] == '\\') {
                pos++;
                if (pos >= src.size()) throw std::runtime_error("Invalid escape in string");
                char esc = src[pos++];
                switch (esc) {
                    case 'n': s += '\n'; break;
                    case 't': s += '\t'; break;
                    case 'r': s += '\r'; break;
                    case '"': s += '"'; break;
                    case '\\': s += '\\'; break;
                    default: s += esc; break;
                }
            } else {
                s += src[pos++];
            }
        }
        if (pos >= src.size()) throw std::runtime_error("Unterminated string literal");
        pos++; // skip closing "

        Value val;
        val.set<std::string>(s);
        return Token{TokenType::Literal, val, ""};
    }

    return Token{TokenType::End, 0, ""};

}


Token Lexer::op_check() {

    char c = src[pos];

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
            pos++;
            if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::CaretEq, 0, "^="};
            }
            return Token{TokenType::Caret, 0, "^"};
        }
        case '~': return Token {TokenType::Tilde, 0, std::string(1,'~')};
        case '<': {
            pos++;
            if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::Le, 0, "<="};
            }
            return Token{TokenType::Lt, 0, "<"};
        }
        case '>': {
            pos++;
            if (pos < src.size() && src[pos] == '=') {
                pos++;
                return Token{TokenType::Ge, 0, ">="};
            }
            return Token{TokenType::Gt, 0, ">"};
        }
        case '=': {
            pos++;
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

        case ';': { pos++; return Token {TokenType::Semi, 0, std::string(1, ';')}; }
        case '(': { pos++; return Token {TokenType::LParen, 0, std::string(1, '(')}; }
        case ')': { pos++; return Token {TokenType::RParen, 0, std::string(1, ')')}; }
        case '{': { pos++; return Token {TokenType::LBrace, 0, std::string(1, '{')}; }
        case '}': { pos++; return Token {TokenType::RBrace, 0, std::string(1, '}')}; }
    }

    return {TokenType::End, 0, ""};

}