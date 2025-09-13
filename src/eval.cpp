#include <iostream>
#include <vector>
#include <string>
#include "eval.hpp"

struct Var {
    std::string name;
    int* addr;
};

static std::vector<Var> vars;

static int get_var(const std::string& name) {
    for (auto& v : vars) {
        if (v.name == name)
            return *(v.addr);
    }
    std::cerr << "Undefined variable: " << name << "\n";
    exit(1);
}

static void set_var(const std::string& name, int value) {
    for (auto& v : vars) {
        if (v.name == name) {
            *(v.addr) = value;
            return;
        }
    }
    Var var{name, new int(value)};
    vars.push_back(var);
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

            case ASTType::BinOp: {
            int l = n->binop.left ? eval(n->binop.left) : 0;
            int r = eval(n->binop.right);

            if (n->binop.op == "+") return l + r;
            else if (n->binop.op == "-") return l - r;
            else if (n->binop.op == "*") return l * r;
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
            if (n->binop.op == "^=") {
                int val = l ^ r;
                return assign(n->binop.left, val);
            }
            
            printf("Unknown binop\n"); exit(1);
            }

        case ASTType::Assign: {
            int val = eval(n->assign.expr);
            set_var(n->assign.name, val);
            return val;
        }
    }
    return 0;
}

void free_vars() {
    for (auto& v : vars) {
        delete v.addr;
    }
    vars.clear();
}

