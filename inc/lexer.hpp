#pragma once

#include <string>
#include <array>
#include <cctype>
#include <iostream>
#include <optional>
#include "value.hpp"
#include "error.hpp"
#include <unordered_map>

enum class TokenType {
    End,
    Literal, KwType,
    Ident,

    // arithmetic
    Plus, Minus, Star, Slash,
    PlusEq, MinusEq, StarEq, SlashEq,
    Increment, Decrement, Pow, Modulo, ModEq,
    // bitwise
    Amp, AmpEq, Pipe, PipeEq, Caret, CaretEq, Tilde, 
    ShiftLeft, ShiftRight, ShiftLeftEq, ShiftRightEq,
    // logical
    Not,Eq, Lt, Le, Gt, Ge, EqEq, NotEq,
    BoolAnd, BoolOr, 
    // keywords
    KwIf, KwWhile, KwFor,
    KwCheck, KwThen, KwRecheck,KwOn, KwOnly, KwReturn, KwConst,
    KwTrue, KwFalse, KwLet, KwSubr, KwCase, KwElse, KwTypeof,
    KwStruct, KwEnum, KwUnion, KwTool, KwKit, KwImport,
    // punctuation
    Colon, Assign, Semi, Comma,
    LParen, RParen, LBrace, RBrace,
    //handle shenanigains
    Perm, Sizeof, Arrow, Dot, Question, QuestionDot, DoubleColon,
    
};

constexpr std::array<const char*, 76> token_type_names = {
    "End",
    "Literal", "KwType",
    "Ident",

    // arithmetic
    "Plus", "Minus", "Star", "Slash",
    "PlusEq", "MinusEq", "StarEq", "SlashEq",
    "Increment", "Decrement", "Pow", "Modulo", "ModEq",
    // bitwise
    "Amp", "AmpEq", "Pipe", "PipeEq", "Caret", "CaretEq", "Tilde", 
    "ShiftLeft", "ShiftRight", "ShiftLeftEq", "ShiftRightEq",
    // logical
    "Not", "Eq", "Lt", "Le", "Gt", "Ge", "EqEq", "NotEq",
    "BoolAnd", "BoolOr", 
    // keywords
    "KwIf", "KwWhile", "KwFor",
    "KwCheck", "KwThen", "KwRecheck", "KwOn", "KwOnly", "KwReturn", "KwConst",
    "KwTrue", "KwFalse", "KwLet", "KwSubr", "KwCase", "KwElse", "KwTypeof",
    "KwStruct", "KwEnum", "KwUnion", "KwTool", "KwKit", "KwImport",
    // punctuation
    "Colon", "Assign", "Semi", "Comma",
    "LParen", "RParen", "LBrace", "RBrace",
    // handle shenanigains
    "Perm", "Sizeof", "Arrow", "Dot", "Question", "QuestionDot", "DoubleColon"
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
    size_t row = 1;
    size_t col = 1;
};

struct Lexer {
    std::string src;
    size_t pos = 0;
    size_t row = 1;
    size_t col = 1;
    Token current;

    Lexer(const std::string& input) : src(input), pos(0) {
        current = get_next_token();
    }
    Lexer () {}
    Token get_next_token();

    void reset_lexer(const std::string& input) {  src = input; pos = 0; current = get_next_token(); }
        
    Token op_check();
    Token ident_check(const std::string& ident, size_t start_row = 0, size_t start_col = 0);
    Token literal_check();
    Token access_check();
    void log_token(const Token& tok) {
#ifdef TOKEN_LOG
        std::cerr << "Token: ";
        if (tok.type != TokenType::Literal) std::cerr << tok.lexeme;
        else std::cerr << "<literal>";
        std::cerr << " type=" << token_type_to_string(tok.type)
                  << " @" << tok.row << ":" << tok.col << std::endl;
#endif
    }

    void advance() {
#ifdef TOKEN_LOG
    log_token(current);
#endif
        current = get_next_token();
    }

    void skip_whitespace() {
        while (pos < src.size() && isspace((unsigned char)src[pos])) {
            if (src[pos] == '\n') { row++; col = 1; pos++; }
            else { pos++; col++; }
        }
    }
    bool is_ident_start(char c) {
        return isalpha(c) || c == '_';
    }
    bool is_ident_char(char c) {
        return isalnum(c) || c == '_';
    }
    bool match_str(const std::string& s, bool requireIdBoundary = true) {
        // skip if we're too close to end
        if (pos + s.size() > src.size()) return false;

        // compare substring
        if (src.compare(pos, s.size(), s) != 0) return false;

        // if caller requires identifier boundary, ensure it's not part of a longer identifier
        if (requireIdBoundary) {
            char next = (pos + s.size() < src.size()) ? src[pos + s.size()] : '\0';
            if (isalnum(next) || next == '_') return false;
        }

        // match succeeded, advance and return true
        // advance pos and update row/col for matched string
        for (size_t i = 0; i < s.size(); ++i) {
            if (src[pos] == '\n') { row++; col = 1; pos++; }
            else { pos++; col++; }
        }
        return true;
}
     static const std::unordered_map<std::string, TokenType> keywords;


};


