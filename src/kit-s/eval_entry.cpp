#include "eval.hpp"
#include "stmt.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include "interpret.hpp"
#include "error.hpp"
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

Value Evaluator::eval(const Node* node) {
    switch (node->nodeType) {
        case NodeType::Expr: return eval_expr(static_cast<const Expr*>(node));
        case NodeType::Stmt: return eval_stmt(static_cast<const Stmt*>(node));
        case NodeType::Decl: eval_decl(static_cast<const Decl*>(node)); return Value{};
        default: throw RuntimeError("Unknown node type in evaluation");
    }

}   