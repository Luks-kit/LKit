#include <vector>
#include "eval.hpp"
#include "stmt.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include "interpret.hpp"
#include <iostream>
#include <stdexcept>

Value Evaluator::eval_stmt(const Stmt* s) {
    switch (s->kind) {
        case StmtKind::ExprStmt:
            return eval_expr_stmt(static_cast<const ExprStmt*>(s));
        case StmtKind::Block:
            return eval_block(static_cast<const BlockStmt*>(s));
        case StmtKind::If:
            return eval_if(static_cast<const IfStmt*>(s));
        case StmtKind::While:
            return eval_while(static_cast<const WhileStmt*>(s));
        case StmtKind::Return:
            return eval_return(static_cast<const ReturnStmt*>(s));
        case StmtKind::Check:
            return eval_check(static_cast<const CheckStmt*>(s));
        case StmtKind::Recheck:
            return eval_recheck(static_cast<const RecheckStmt*>(s));
    }
    return Value{}; // Default return if no case matches
}

Value Evaluator::eval_block(const BlockStmt* b){
    // Create a new scope for the block
    Value ret;
    runtime.push_scope();
    // Add any declarations to the current scope
    for (const auto& decl : b->decl) {
        if (decl) eval_decl(decl.get());
    }
    // Evaluate each statement in the block
    // If there's a return statement at the end, evaluate it and return its value
    // Ensure the return type matches the block's expected type if specified
    for (const auto& stmt : b->stmts) {
        ret = eval_stmt(stmt.get());
    } 
    runtime.pop_scope();
    return ret;
}

Value Evaluator::eval_expr_stmt(const ExprStmt* es){
    // Evaluate expression and discard result, but return void-equivalent Value
    (void)eval_expr(es->expr.get());
    return Value(); // void
}

Value Evaluator::eval_if(const IfStmt* i){
    Value ret;
    Value cond = eval_expr(i->cond.get());
    if (cond.get<bool>()) {
        ret = eval_stmt(i->then_branch.get());
    } else if (i->else_branch) {
        ret = eval_stmt(i->else_branch.get());
    }
    return ret;
}   

Value Evaluator::eval_while(const WhileStmt* w){
    Value ret;
    while (eval_expr(w->cond.get()).get<bool>()) {
        ret = eval_stmt(w->body.get());
    }
    return ret;
}

Value Evaluator::eval_check(const CheckStmt* c){
    Value expr_val = eval_expr(c->expr.get());
    Value ret; bool matched = false;
    for (const auto& arm : c->arms.arms) {
        Value arm_val = eval_expr(arm.first.get());
        if (expr_val == arm_val) {
            matched = true; 
            ret = eval_stmt(arm.second.get());
            if (c->execute_first_match) return ret; // Exit after first match if "only" mode
        }
    }
    if (c->arms.else_arm && !matched) { // Execute else arm if no match found
        ret = eval_stmt(c->arms.else_arm.get());
    }
    return ret;
}   

Value Evaluator::eval_recheck(const RecheckStmt* r){
    // Re-evaluate the expression and re-check the arms
    Value expr_val = eval_expr(r->expr.get());
    Value ret; bool matched = false;
    for (const auto& arm : r->arms.arms) {
        Value arm_val = eval_expr(arm.first.get());
        if (expr_val == arm_val) {
            matched = true;
            ret = eval_stmt(arm.second.get());
            if (r->execute_first_match) return ret; // Exit after first match if "only" mode
        }
    }
    if (r->arms.else_arm && !matched) { // Execute else arm if no match found 
                            // should never exist in recheck unless it contains a break statement (loop ends on no matches found)
        eval_stmt(r->arms.else_arm.get());
    }
    return ret;
}



Value Evaluator::eval_return(const ReturnStmt* r){
    if (r->expr) {
        throw EvalRuntime::ReturnException(eval_expr(r->expr.get()));
    }
    else throw EvalRuntime::ReturnException(Value()); // or std::monostate{} if Value is a variant including monostate
}


