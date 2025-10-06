#ifdef _TOKENIZE_DEBUG

#include <iostream>
#include "lexer.hpp"

int main() {
    std::string src((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
    Lexer lx(src);
    Token t = lx.get_next_token();
    while (t.type != TokenType::End) {
        std::cout << token_type_to_string(t.type) << " ";
        if (!t.lexeme.empty()) std::cout << "lex='" << t.lexeme << "' ";
        std::cout << "@" << t.row << ":" << t.col << std::endl;
        t = lx.get_next_token();
    }
    std::cout << "End @" << lx.row << ":" << lx.col << std::endl;
    return 0;
}
#endif