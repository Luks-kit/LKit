#include <iostream>
#include "eval.hpp"
#include "error.hpp"
#include "parser.hpp"
#include "lexer.hpp"
#include "node.hpp"
#include "value.hpp"
#include "decl.hpp"
#include "stmt.hpp"
#include "expr.hpp"
#include "interpret.hpp"
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

Value Evaluator::eval_expr(const Expr* e){
    switch (e->kind) {
        case ExprKind::Literal:
            return eval_literal(static_cast<const LiteralExpr*>(e));
        case ExprKind::Ident:
            return eval_ident(static_cast<const IdentExpr*>(e));
        case ExprKind::Binary:
            return eval_binary(static_cast<const BinaryExpr*>(e));
        case ExprKind::Unary:
            return eval_unary(static_cast<const UnaryExpr*>(e));
        case ExprKind::Call:
            return eval_call(static_cast<const CallExpr*>(e));  
    }
    // Should not reach here; return void Value as fallback
    return Value();
}

Value Evaluator::eval_literal(const LiteralExpr* l){
    return l->literal;
}

Value Evaluator::eval_ident(const IdentExpr* i){
    // Lookup variable in current scope
    Scope current_scope = scopes.back();
    auto it = std::find_if(current_scope.begin(), current_scope.end(),
        [&](const Var& v) { return v.name == i->name; });
    if (it != current_scope.end()) {
        return *(it->addr); // Return the value of the found variable
    }
    throw RuntimeError("Undefined variable: " + i->name);
}

Value Evaluator::eval_binary(const BinaryExpr* b){
    Value left = eval_expr(b->lhs.get());
    Value right = eval_expr(b->rhs.get());

    if (b->op == "+") return left + right;
    if (b->op == "-") return left - right;
    if (b->op == "*") return left * right;
    if (b->op == "/") {
        if (right == 0) throw RuntimeError("Division by zero");
        return left / right;
    }
    if (b->op == "==") return left == right;
    if (b->op == "!=") return left != right;
    if (b->op == "<") return left < right;
    if (b->op == "<=") return left <= right;
    if (b->op == ">") return left > right;
    if (b->op == ">=") return left >= right;
    if (b->op == "&&") return left && right;
    if (b->op == "||") return left || right;

    // Assignment and compound assignment handling
    if (b->op == "=") {
        // lhs must be an identifier
        const IdentExpr* id = dynamic_cast<const IdentExpr*>(b->lhs.get());
        if (!id) throw RuntimeError("Assignment target must be an identifier");
        set_var(id->name, right);
        return right;
    }
    if (b->op == "+=") {
        const IdentExpr* id = dynamic_cast<const IdentExpr*>(b->lhs.get());
        if (!id) throw RuntimeError("Assignment target must be an identifier");
        Value newv = left + right;
        set_var(id->name, newv);
        return newv;
    }
    if (b->op == "-=") {
        const IdentExpr* id = dynamic_cast<const IdentExpr*>(b->lhs.get());
        if (!id) throw RuntimeError("Assignment target must be an identifier");
        Value newv = left - right;
        set_var(id->name, newv);
        return newv;
    }
    if (b->op == "*=") {
        const IdentExpr* id = dynamic_cast<const IdentExpr*>(b->lhs.get());
        if (!id) throw RuntimeError("Assignment target must be an identifier");
        Value newv = left * right;
        set_var(id->name, newv);
        return newv;
    }
    if (b->op == "/=") {
        const IdentExpr* id = dynamic_cast<const IdentExpr*>(b->lhs.get());
        if (!id) throw RuntimeError("Assignment target must be an identifier");
        if (right == 0) throw RuntimeError("Division by zero");
        Value newv = left / right;
        set_var(id->name, newv);
        return newv;
    }

    throw RuntimeError("Unknown binary operator: " + b->op);
}

Value Evaluator::eval_unary(const UnaryExpr* u){
    Value operand = eval_expr(u->operand.get());

    if (u->op == "-") return -operand;
    if (u->op == "!") return !operand;

    throw RuntimeError("Unknown unary operator: " + u->op);
}

Value Evaluator::eval_call(const CallExpr* c){
    Value callee = eval_expr(c->callee.get());
    if (!std::holds_alternative<std::string>(callee.value)) {
        throw RuntimeError("Attempted to call a non-function value");
    }
    std::string func_name = std::get<std::string>(callee.value);

    std::vector<Value> arg_values;
    for (const auto& arg : c->args) {
        arg_values.push_back(eval_expr(arg.get()));
    }

    return call_subr(func_name, arg_values);

}