#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "eval.hpp"

struct Var {
    std::string name;
    int* addr;
};

static std::vector<std::vector<Var>> scopes = { {} };

static void push_scope() {
    scopes.push_back({});
}

static void pop_scope() {
    // Free heap memory for variables in this scope
    for (auto& v : scopes.back()) {
        delete v.addr;
    }
    scopes.pop_back();
}

void cleanup_scopes() {
    while (!scopes.empty())
        pop_scope();
}

static int get_var(const std::string& name) {
    // Search from innermost scope outwards
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        for (auto& v : *it) {
            if (v.name == name)
                return *(v.addr);
        }
    }
    std::cerr << "Undefined variable: " << name << "\n";
    exit(1);
}

static void set_var(const std::string& name, int value) {
    // Search from innermost scope outwards
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        for (auto& v : *it) {
            if (v.name == name) {
                *(v.addr) = value;
                return;
            }
        }
    }
    // If not found, define in current (innermost) scope
    Var var{name, new int(value)};
    scopes.back().push_back(var);
}

static int assign(AST* lhs, int val) {
    if (lhs->type == ASTType::Ident) {
        set_var(lhs->name, val);
        return val;
    } else {
        std::cerr << "Left-hand side of assignment must be an identifier\n";
        exit(1);
    }
}

int eval(AST* n) {
    switch (n->type) {
        case ASTType::Number: return n->value;
        case ASTType::Char:   return n->value;
        case ASTType::Ident:  return get_var(n->name);
        case ASTType::If: {
            int cond_val = eval(n->ifstmt.cond);
            if (cond_val)
                return eval(n->ifstmt.then_branch);
            else if (n->ifstmt.else_branch)
                return eval(n->ifstmt.else_branch);
            return 0; // if no else branch
            }
        case ASTType::While: {
            int result = 0;
            while (eval(n->whilestmt.cond)) {
                result = eval(n->whilestmt.body);
            }
            return result;
        }
        case ASTType::BinOp: {
            int l = n->binop.left ? eval(n->binop.left) : 0;
            int r = eval(n->binop.right);

            if (n->binop.op == "+") return l + r;
            else if (n->binop.op == "++") return r++;
            else if (n->binop.op == "-") return l - r;
            else if (n->binop.op == "--") return r--;
            else if (n->binop.op == "*") return l * r;
            else if (n->binop.op == "**") return pow(l,r);
            else if (n->binop.op == "/") return r ? l / r : (std::cerr << "division by zero\n", exit(1), 0);
            
            else if (n->binop.op == "&")  return l & r;
            else if (n->binop.op == "|")  return l | r;
            else if (n->binop.op == "^") return l ^ r;
            
            else if (n->binop.op == "&&") return (l != 0) && (r != 0);
            else if (n->binop.op == "||") return (l != 0) || (r != 0);

            else if (n->binop.op == "~")  return ~r;        // unary, left is nullptr
            else if (n->binop.op == "neg") return -r;      // unary minus
            
            else if (n->binop.op == "!") return !(r != 0);

            else if (n->binop.op == "<")  return l < r;
            else if (n->binop.op == ">")  return l > r;
            else if (n->binop.op == "<=") return l <= r;
            else if (n->binop.op == ">=") return l >= r;
            else if (n->binop.op == "==") return l == r;
            else if (n->binop.op == "!=") return l != r;
            // Compound assignments
            else if (n->binop.op == "&=") {
                int val = l & r;
               return assign(n->binop.left, val); // helper that writes into vars
            }
            else if (n->binop.op == "|=") {
                int val = l | r;
                return assign(n->binop.left, val);
            }
            else if (n->binop.op == "^=") {
                int val = l ^ r;
                return assign(n->binop.left, val);
            }
            else if (n->binop.op == "+="){
                int val = l + r;
                return assign(n->binop.left, val);
            }
            
            else if (n->binop.op == "-="){
                int val = l - r;
                return assign(n->binop.left, val);
            }
            
            else if (n->binop.op == "/="){
                int val = l / r;
                return assign(n->binop.left, val);
            }

            else if (n->binop.op == "*="){
                int val = l * r;
                return assign(n->binop.left, val);
            }

            printf("Unknown binop\n"); exit(1);
            }

        case ASTType::Assign: {
            int val = eval(n->assign.expr);
            set_var(n->assign.name, val);
            return val;
        }

        case ASTType::Block: {
            push_scope();
            int result = 0;
            for (AST* stmt : n->block.stmts) {
                result = eval(stmt);
            }
            pop_scope();
            return result;
}
    }
    return 0;
}


