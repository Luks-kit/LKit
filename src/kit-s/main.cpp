#include <iostream>
#include <string>
#include "interpret.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "eval.hpp"

void reploop(){
    std::string line, source;
    int brace_balance = 0;
    Lexer lex;

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
    lex.reset_lexer(source);
    Parser parser(lex);
    std::unique_ptr<Node> tree = parser.parse();
    Evaluator evaluator;
    Value result;
    if (tree) result = evaluator.eval(tree.get());
    std::cout << "Result: " << result.toString() << "\n";
        source.clear();
    }       
    cleanup_scopes();
    
}

int main() {
    
    reploop();


    return 0;
}

