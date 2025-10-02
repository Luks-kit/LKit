#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <variant>
#include "ast.hpp"
#include "eval.hpp"


struct Var { std::string name; int* addr; };
static std::vector<std::vector<Var>> scopes = { {} };

static void push_scope() { scopes.push_back({}); }
static void pop_scope() {
    for (auto& v : scopes.back()) delete v.addr;
    scopes.pop_back();
}
void cleanup_scopes() { while (!scopes.empty()) pop_scope(); }

static void decl_var(const std::string& name, int value = 0) {
    scopes.back().push_back({name, new int(value)});

}


static int get_var(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        for (auto& v : *it) if (v.name == name) return *(v.addr);
    }
    std::cerr << "Undefined variable: " << name << "\n";
    exit(1);
}

static void set_var(const std::string& name, int value) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        for (auto& v : *it) if (v.name == name) { *(v.addr) = value; return; }
    }

     std::cerr << "Undefined variable: " << name << "\n";
    exit(1);  // "let" works, no auto decl for now
}

static int assign(AST* lhs, int val) {
    if (lhs->type != ASTType::Ident) {
        std::cerr << "Left-hand side of assignment must be an identifier\n";
        exit(1);
    }
    set_var(lhs->as_string(), val);
    return val;
}

int eval(AST* n) {
    switch (n->type) {
        case ASTType::Number:
        case ASTType::Char:
            return n->as_int();

        case ASTType::Ident:
            return get_var(n->as_string());

        case ASTType::Check: {
            auto& c = n->as_check();
            int cond_val = eval(c.cond);
            if (cond_val) return eval(c.ok_branch);
            else if (c.then_branch) return eval(c.then_branch);
            return 0;
        }

        case ASTType::Recheck: {
            auto& r = n->as_recheck();
            int result = 0;
            while (eval(r.cond)) result = eval(r.body);
            return result;
        }

        case ASTType::BinOp: {
            auto& b = n->as_binop();
            int l = b.lhs ? eval(b.lhs) : 0;
            int r = eval(b.rhs);

            if (b.op == "+") return l + r;
            else if (b.op == "-") return l - r;
            else if (b.op == "*") return l * r;
            else if (b.op == "**") return pow(l, r);
            else if (b.op == "/") return r ? l / r : (std::cerr << "division by zero\n", exit(1), 0);
            else if (b.op == "&") return l & r;
            else if (b.op == "|") return l | r;
            else if (b.op == "^") return l ^ r;
            else if (b.op == "&&") return (l != 0) && (r != 0);
            else if (b.op == "||") return (l != 0) || (r != 0);
            else if (b.op == "~") return ~r;
            else if (b.op == "neg") return -r;
            else if (b.op == "!") return !(r != 0);
            else if (b.op == "<") return l < r;
            else if (b.op == ">") return l > r;
            else if (b.op == "<=") return l <= r;
            else if (b.op == ">=") return l >= r;
            else if (b.op == "==") return l == r;
            else if (b.op == "!=") return l != r;

            std::cerr << "Unknown binop\n"; exit(1);
        }
        
        case ASTType::Decl: {
            auto& declared = n->as_decl();
            int val = declared.expr? eval (declared.expr) : 0;
            decl_var(declared.name, val);
            return val; 
        }
                             
        case ASTType::Assign: {
            auto& a = n->as_assign();
            int val = eval(a.expr);
            set_var(a.name, val);
            return val;
        }

        case ASTType::AssignOp: {
            auto& a = n->as_assign_op();
            int old_val = get_var(a.name);
            int rhs_val = eval(a.rhs);
            int result = 0;
            switch(a.op) {
                case '+': result = old_val + rhs_val; break;
                case '-': result = old_val - rhs_val; break;
                case '*': result = old_val * rhs_val; break;
                case '/': result = old_val / rhs_val; break;
                case '&': result = old_val & rhs_val; break;
                case '|': result = old_val | rhs_val; break;
                case '^': result = old_val ^ rhs_val; break;
            }
            set_var(a.name, result);
            return result;
        }

        case ASTType::IncDec: {
            auto& i = n->as_incdec();
            int old_val = get_var(i.name);
            int result = (i.op == '+') ? old_val + 1 : old_val - 1;
            set_var(i.name, result);
            return result;
        }

        case ASTType::Block: {
            auto& b = n->as_block();
            push_scope();
            int result = 0;
            for (AST* stmt : b.stmts) result = eval(stmt);
            pop_scope();
            return result;
        }
    }

    return 0;
}

