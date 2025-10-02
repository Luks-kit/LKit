#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <array>

enum class TokenType {
    End,
    Number, Char, Bool, Float, String, KwType,
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
    "Number", "Char", "Bool", "Float", "String","KwType",
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
    int value = 0;
    std::string lexeme;
};

struct Lexer {
    Token current;
    std::string src;
    int pos;
    

    std::string map_token(const Token& t);
    void reset_lexer(const std::string& input);
    Token next_token();
    void log_token(const Token& t);
    
};

// shared globals
extern Token current;
extern std::string src;
extern int pos;

// lexer functions
void reset_lexer(const std::string& input);
Token get_next_token();
void advance();
void log_token(const Token& t);
#endif

