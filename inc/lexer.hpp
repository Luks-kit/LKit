#pragma once

#include <string>
#include <array>
#include <cctype>
#include <iostream>
#include <optional>
#include "value.hpp"

enum class TokenType {
    End,
    Literal, KwType,
    Ident,

    // arithmetic
    Plus, Minus, Star, Slash,
    PlusEq, MinusEq, StarEq, SlashEq,
    Increment, Decrement, Pow,
    // bitwise
    Amp, AmpEq, Pipe, PipeEq, Caret, CaretEq, Tilde, 
    
    Not,Eq, Lt, Le, Gt, Ge, EqEq, NotEq,
    
    BoolAnd, BoolOr,
    // keywords
    KwInt, KwShort, KwLong, KwChar, KwBool, KwFloat, KwDouble, KwString,
    KwCheck, KwThen, KwRecheck,KwOn, KwReturn, KwConst,
    KwTrue, KwFalse, KwLet, KwSubr,

    Assign, Semi, LParen, RParen,
    LBrace, RBrace
};

constexpr std::array<const char*, 60> token_type_names = {
    "End",
    "Literal", "KwType",
    "Ident",
    
    // arithmetic
    "Plus", "Minus", "Star", "Slash",
    "PlusEq", "MinusEq", "StarEq", "SlashEq",
    "Increment", "Decrement", "Pow",
    
    // bitwise
    "Amp", "AmpEq", "Pipe", "PipeEq", "Caret", "CaretEq", "Tilde",

    // logical/comparison
    "Not", "Eq", "Lt", "Le", "Gt", "Ge", "EqEq", "NotEq",
    "BoolAnd", "BoolOr",

    // keywords
    "KwInt","KwShort", "KwLong", "KwChar", "KwBool", "KwFloat","KwDouble", "KwString",
    "KwCheck", "KwThen", "KwRecheck", "KwOn", "KwReturn", "KwConst",
    "KwTrue", "KwFalse", "KwLet", "KwSubr",

    // punctuation
    "Assign", "Semi", "LParen", "RParen", "LBrace", "RBrace"
};

inline const char* token_type_to_string(TokenType type) {
    auto index = static_cast<size_t>(type);
    if (index >= token_type_names.size()) {
        return "Unknown";
    }
    return token_type_names[index];
}

struct Token {
    TokenType type;
    Value value;
    std::string lexeme;
};

struct Lexer {
    std::string src;
    size_t pos = 0;
    Token current;

    Lexer(const std::string& input) : src(input), pos(0) {
        current = get_next_token();
    }
    Lexer () {}
    Token get_next_token();

    void reset_lexer(const std::string& input) { src.clear(); src = input; std::cout << src; pos = 0; current = get_next_token(); }
        
    Token op_check();
    Token ident_check(const std::string& ident);
    Token literal_check();
    
    void log_token(const Token& tok) {
        std::cerr << "Token: " << tok.lexeme
                  << " type=" << token_type_to_string(tok.type) << std::endl;
    }

    void advance() {
        log_token(current);
        current = get_next_token();
    }

    void skip_whitespace() {
        while (pos < src.size() && isspace(src[pos])) pos++;
    }
    bool is_ident_start(char c) {
        return isalpha(c) || c == '_';
    }
    bool is_ident_char(char c) {
        return isalnum(c) || c == '_';
    }
    bool match_str(const std::string& s) {
    // skip if we're too close to end
    if (pos + s.size() > src.size()) return false;

    // compare substring
    if (src.compare(pos, s.size(), s) != 0) return false;

    // make sure it's not part of a longer identifier
    char next = (pos + s.size() < src.size()) ? src[pos + s.size()] : '\0';
    if (isalnum(next) || next == '_') return false;

    // match succeeded, advance and return true
    pos += s.size();
    return true;
}
};


