#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>

enum class TokenType {
    End,
    Number, Char, Bool, Float, String,
    Ident,

    // arithmetic
    Plus, Minus, Star, Slash,
    // bitwise
    Amp, AmpEq, Pipe, PipeEq, Caret, CaretEq, Tilde, 
    
    Not,Eq, Lt, Le, Gt, Ge, EqEq, NotEq,
    
    BoolAnd, BoolOr,
    // keywords
    KwInt, KwChar, KwBool, KwFloat, KwString,
    KwIf, KwElse, KwWhile, KwReturn, KwTrue, KwFalse,

    Assign, Semi, LParen, RParen,
    LBrace, RBrace
};

struct Token {
    TokenType type;
    int value = 0;
    std::string lexeme;
};

// shared globals
extern Token current;
extern std::string src;
extern int pos;

// lexer functions
Token get_next_token();
void advance();

#endif

