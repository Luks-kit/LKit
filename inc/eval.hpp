#ifndef EVAL_HPP
#define EVAL_HPP

#include "ast.hpp"

Value eval(AST *n);
Value eval_check(AST *n);
Value eval_recheck(AST *n);
Value eval_decl(AST *n);
Value eval_assign(AST *n);
Value eval_block(AST *n);
Value eval_inc_dec(AST *n);
Value eval_assign_op(AST *n);
Value eval_binop(AST *n);
void cleanup_scopes();

// evaluator.hpp

#include "expr.hpp"
#include "stmt.hpp"
#include "decl.hpp"
#include <memory>
#include <iostream>

class Evaluator {
public:
    void eval(const Node* node) {
        switch (node->nodeType) {
            case NodeType::Expr: eval_expr(static_cast<const Expr*>(node)); break;
            case NodeType::Stmt: eval_stmt(static_cast<const Stmt*>(node)); break;
            case NodeType::Decl: eval_decl(static_cast<const Decl*>(node)); break;
        }
    }

private:
    void eval_expr(const Expr* e);

    void eval_stmt(const Stmt* s);

    void eval_decl(const Decl* d);
};
#endif

