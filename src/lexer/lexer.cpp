#include "lexer.hpp"
#include <cctype>
#include <iostream>
#include <optional>
#include <unordered_map>


const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"int", TokenType::KwType},        {"short", TokenType::KwType},
    {"long", TokenType::KwType},       {"char", TokenType::KwType},
    {"bool", TokenType::KwType},       {"float", TokenType::KwType},
    {"double", TokenType::KwType},     {"string", TokenType::KwType},
    {"const", TokenType::KwConst},     {"check", TokenType::KwCheck},
    {"then", TokenType::KwThen},       {"on", TokenType::KwOn},
    {"recheck", TokenType::KwRecheck}, {"return", TokenType::KwReturn},
    {"if", TokenType::KwIf},           {"else", TokenType::KwElse},
    {"while", TokenType::KwWhile},     {"for", TokenType::KwFor},
    {"only", TokenType::KwOnly},       {"case", TokenType::KwCase},
    {"let", TokenType::KwLet},         {"subr", TokenType::KwSubr},
    {"struct", TokenType::KwStruct},   {"enum", TokenType::KwEnum},
    {"union", TokenType::KwUnion},     {"tool", TokenType::KwTool},
    {"kit", TokenType::KwKit},         {"import", TokenType::KwImport},
    {"typeof", TokenType::KwTypeof},   {"sizeof", TokenType::Sizeof},
    {"true", TokenType::KwTrue},       {"false", TokenType::KwFalse}
};


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

    // access chars and operators
    Token access_return = access_check();
    if(access_return.type != TokenType::End) { return access_return; }
    // single char tokens...
    Token op_return = op_check();

    if(op_return.type != TokenType::End) { return op_return; }

    std::cerr << "Unknown character: " << c << "\n";
    return {TokenType::End, 0, ""};
}

Token Lexer::ident_check(const std::string& ident){

     // keyword checks...

        auto it = keywords.find(ident);
        if (it != keywords.end()) {
            if (it->first == "true") return {it->second, true, ident};
            if (it->first == "false") return {it->second, false, ident};
            return {it->second, 0, ident};
        }

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



    // Multi-character operators (longest match first)
    struct OpMap {
        const char* str;
        TokenType type;
    };
    static const OpMap ops[] = {
        {"++", TokenType::Increment},   {"+=", TokenType::PlusEq},   {"+", TokenType::Plus},
        {"--", TokenType::Decrement},   {"-=", TokenType::MinusEq},  {"**", TokenType::Pow}, {"-", TokenType::Minus},
        {"*=", TokenType::StarEq},      {"*", TokenType::Star},
        {"/=", TokenType::SlashEq},     {"/", TokenType::Slash},
        {"&&", TokenType::BoolAnd},     {"&=", TokenType::AmpEq},    {"&", TokenType::Amp},
        {"||", TokenType::BoolOr},      {"|=", TokenType::PipeEq},   {"|", TokenType::Pipe},
        {"^=", TokenType::CaretEq},     {"^", TokenType::Caret},
        {"~", TokenType::Tilde},
        {"<=", TokenType::Le},          {"<", TokenType::Lt},
        {">=", TokenType::Ge},          {">", TokenType::Gt},
        {"==", TokenType::EqEq},        {"=", TokenType::Eq},
        {"!=", TokenType::NotEq},       {"!", TokenType::Not},
        {";", TokenType::Semi},
        {"(", TokenType::LParen},       {")", TokenType::RParen},
        {"{", TokenType::LBrace},       {"}", TokenType::RBrace}
    };

    for (const auto& op : ops) {
        if (match_str(op.str, false)) {
            return Token{op.type, 0, op.str};
        }
    }

    return {TokenType::End, 0, ""};

}

Token Lexer::access_check(){
    if (match_str("->", false)) {
        return Token{TokenType::Arrow, 0, "->"};
    }
    if (match_str(".", false)) {
        return Token{TokenType::Dot, 0, "."};
    }
    if (match_str("?.", false)) {
        return Token{TokenType::QuestionDot, 0, "?."};
    }
    if (match_str("?", false)) {
        return Token{TokenType::Question, 0, "?"};
    }
    if (match_str("::", false)) {
        return Token{TokenType::DoubleColon, 0, "::"};
    }
    if(match_str("$", false)) {
        return Token{TokenType::Perm, 0, "$"};
    }
    if(match_str(":", false)) {
        return Token{TokenType::Colon, 0, ":"};
    }
    return {TokenType::End, 0, ""};
}