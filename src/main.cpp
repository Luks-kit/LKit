#include <iostream>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include "eval.hpp"

int main() {
    std::cout << "Mini language REPL (enter empty line to quit)\n";

    while (true) {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line) || line.empty()) break;

        src = line;
        pos = 0;
        advance();              // prime lexer

        AST* tree = parse();    // parse one statement
        int result = eval(tree);

        std::cout << "Result = " << result << "\n";
    }

    free_vars();
    return 0;
}

