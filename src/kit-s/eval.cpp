#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <variant>
#include "interpret.hpp"
#include "ast.hpp"
#include "eval.hpp"
#include "value.hpp"



Value eval(AST* n) {
    switch (n->type) {
        case ASTType::Literal:
            return n->as_value();

        case ASTType::Ident:
            return get_var(n->as_string());

        case ASTType::Check: {
            return eval_check(n);
        }

        case ASTType::Recheck: {
            return eval_recheck(n);
        }

        case ASTType::BinOp: {
            return eval_binop(n);
        }
        
        case ASTType::Decl: {
            return eval_decl(n);
        }
                             
        case ASTType::Assign: {
            return eval_assign(n);
        }

        case ASTType::AssignOp: {
            return eval_assign_op(n);
        }

        case ASTType::IncDec: {
            return eval_inc_dec(n);
        }

        case ASTType::Block: {
            return eval_block(n);
        }
    }

    return 0;
}

Value eval_check(AST *n)
{
    auto &c = n->as_check();
    Value cond_val = eval(c.cond);
    if (cond_val.get<bool>())
        return eval(c.ok_branch);
    else if (c.then_branch)
        return eval(c.then_branch);
    return 0;
}

Value eval_recheck(AST *n)
{
    auto &r = n->as_recheck();
    Value result = 0;
    while (eval(r.cond).get<bool>())
        result = eval(r.body);
    return result;
}

Value eval_decl(AST *n)
{
    auto &declared = n->as_decl();
    Value val = declared.expr ? eval(declared.expr) : 0;
    decl_var(declared.name, val);
    // DEBUG: dump current top-level scope after declaration
    // no-op; declaration handled by decl_var
    return val;
}

Value eval_assign(AST *n)
{
    auto &a = n->as_assign();
    Value val = eval(a.expr);
    set_var(a.name, val);
    return val;
}

Value eval_block(AST *n)
{
    auto &b = n->as_block();
    push_scope();
    Value result = 0;
    for (AST *stmt : b.stmts)
        result = eval(stmt);
    pop_scope();
    return result;
}

Value eval_inc_dec(AST *n)
{
    auto &i = n->as_incdec();
    Value old_val = get_var(i.name);
    Value result = (i.op == '+') ? old_val + 1 : old_val - 1;
    set_var(i.name, result);
    return result;
}

Value eval_assign_op(AST *n)
{
    auto &a = n->as_assign_op();
    Value old_val = get_var(a.name);
    Value rhs_val = eval(a.rhs);
    Value result = 0;
    switch (a.op)
    {
    case '+':
        result = old_val + rhs_val;
        break;
    case '-':
        result = old_val - rhs_val;
        break;
    case '*':
        result = old_val * rhs_val;
        break;
    case '/':
        result = old_val / rhs_val;
        break;
    case '&':
        result = old_val & rhs_val;
        break;
    case '|':
        result = old_val | rhs_val;
        break;
    case '^':
        result = old_val ^ rhs_val;
        break;
    }
    set_var(a.name, result);
    return result;
}

Value eval_binop(AST *n)
{
    auto &b = n->as_binop();
    Value l = b.lhs ? eval(b.lhs) : 0;
    Value r = eval(b.rhs);

    if (b.op == "+")
        return l + r;
    else if (b.op == "-")
        return l - r;
    else if (b.op == "*")
        return l * r;
    else if (b.op == "**")
    {
        l.promoteToFloat();
        r.promoteToFloat();
        return pow(l.get<float>(), r.get<float>());
    }
    else if (b.op == "/")
        if (r != Value(0)) return l / r;
        else throw std::runtime_error("division by zero");
    else if (b.op == "&")
        return l & r;
    else if (b.op == "|")
        return l | r;
    else if (b.op == "^")
        return l ^ r;
    else if (b.op == "&&")
        return (l && r);
    else if (b.op == "||")
        return (l || r);
    else if (b.op == "~")
        return ~r;
    else if (b.op == "neg")
        return Value(0) - r;
    else if (b.op == "!")
        return Value(!(r != Value(0)));
    else if (b.op == "<")
        return Value(l < r);
    else if (b.op == ">")
        return Value(l > r);
    else if (b.op == "<=")
        return Value(l <= r);
    else if (b.op == ">=")
        return Value(l >= r);
    else if (b.op == "==")
        return Value(l == r);
    else if (b.op == "!=")
        return Value(l != r);

    throw std::runtime_error("Unknown binop");
}
