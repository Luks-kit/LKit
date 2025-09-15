#include <iostream>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include "eval.hpp"

void reploop(){
std::string line, source;
int brace_balance = 0;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line) || line.empty()) break;

        source += line + "\n";
        for (char c : line) {
            if (c == '{') brace_balance++;
            else if (c == '}') brace_balance--;
        }

        if (brace_balance > 0) continue; // keep reading until closed
        if (brace_balance < 0) {
            std::cerr << "Unmatched closing brace!\n";
            source.clear();
            brace_balance = 0;
            continue;
        }

        // Only parse when braces are balanced
        reset_lexer(source);
        AST* tree = parse();
        int result = eval(tree);
        std::cout << "Result = " << result << "\n";
        source.clear();
}       cleanup_scopes();
    
}

int main() {
    
    reploop();


    return 0;
}

