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
        case ASTType::Check: {
            int cond_val = eval(n->checkstmt.cond);
            if (cond_val)
                return eval(n->checkstmt.ok_branch);
            else if (n->checkstmt.then_branch)
                return eval(n->checkstmt.then_branch);
            return 0; // if no else branch
            }
        case ASTType::Recheck: {
            int result = 0;
            while (eval(n->recheckstmt.cond)) {
                result = eval(n->recheckstmt.body);
            }
            return result;
        }
        case ASTType::BinOp: {
            int l = n->binop.lhs ? eval(n->binop.lhs) : 0;
            int r = eval(n->binop.rhs);

            if (n->binop.op == "+") return l + r;
            else if (n->binop.op == "-") return l - r;
            else if (n->binop.op == "*") return l * r;
            else if (n->binop.op == "**") return pow(l,r);
            else if (n->binop.op == "/") return r ? l / r : (std::cerr << "division by zero\n", exit(1), 0);
            
            else if (n->binop.op == "&")  return l & r;
            else if (n->binop.op == "|")  return l | r;
            else if (n->binop.op == "^") return l ^ r;
            
            else if (n->binop.op == "&&") return (l != 0) && (r != 0);
            else if (n->binop.op == "||") return (l != 0) || (r != 0);

            else if (n->binop.op == "~")  return ~r;        // unary, lhs is nullptr
            else if (n->binop.op == "neg") return -r;      // unary minus
            
            else if (n->binop.op == "!") return !(r != 0);

            else if (n->binop.op == "<")  return l < r;
            else if (n->binop.op == ">")  return l > r;
            else if (n->binop.op == "<=") return l <= r;
            else if (n->binop.op == ">=") return l >= r;
            else if (n->binop.op == "==") return l == r;
            else if (n->binop.op == "!=") return l != r;
            // Compound assignments
            else if (n->binop.op == "&=") { int val = l & r; return assign(n->binop.lhs, val);} // helper that writes into vars}
            else if (n->binop.op == "|=") { int val = l | r; return assign(n->binop.lhs, val);}
            else if (n->binop.op == "^=") { int val = l ^ r; return assign(n->binop.lhs, val);}
            else if (n->binop.op == "+=") { int val = l + r; return assign(n->binop.lhs, val);}
            else if (n->binop.op == "-=") { int val = l - r; return assign(n->binop.lhs, val);}
            else if (n->binop.op == "/=") { int val = l / r; return assign(n->binop.lhs, val);}
            else if (n->binop.op == "*=") { int val = l * r; return assign(n->binop.lhs, val);}

            printf("Unknown binop\n"); exit(1);
            }

        case ASTType::Assign: {
            int val = eval(n->assign.expr);
            set_var(n->assign.name, val);
            return val;
        }
        case ASTType::AssignOp: {
            int rhs_val = eval(n->assign_op.rhs);
            int old_val = get_var(n->assign_op.name);
            int result = 0;
            switch(n->assign_op.op) {
                case '+': result = old_val + rhs_val; break;
                case '-': result = old_val - rhs_val; break;
                case '*': result = old_val * rhs_val; break;
                case '/': result = old_val / rhs_val; break;
                case '&': result = old_val & rhs_val; break;
                case '|': result = old_val | rhs_val; break;
                case '^': result = old_val ^ rhs_val; break;
            
            }
            set_var(n->assign_op.name, result);
        return result;
        }

        case ASTType::IncDec: {
            int old_val = get_var(n->incdec.name);
            int result = (n->incdec.op == '+') ? old_val + 1 : old_val - 1;
            set_var(n->incdec.name, result);
            return result;
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


