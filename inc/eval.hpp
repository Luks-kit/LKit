#pragma once

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
inline void cleanup_scopes();

// evaluator.hpp

#include "expr.hpp"
#include "stmt.hpp"
#include "decl.hpp"
#include <memory>
#include <iostream>

class Evaluator {
public:
    Evaluator() = default;
    ~Evaluator() = default;
    Value eval(const Node* node);
private:
    Value eval_expr(const Expr* e);
    Value eval_literal(const LiteralExpr* l);
    Value eval_ident(const IdentExpr* i);
    Value eval_binary(const BinaryExpr* b);
    Value eval_unary(const UnaryExpr* u);
    Value eval_call(const CallExpr* c);
    Value eval_stmt(const Stmt* s);
    Value eval_expr_stmt(const ExprStmt* es);
    Value eval_block(const BlockStmt* b);
    Value eval_if(const IfStmt* i);
    Value eval_while(const WhileStmt* w);
    Value eval_for(const ForEachStmt* f);
    Value eval_check(const CheckStmt* c);
    Value eval_recheck(const RecheckStmt* r);
    Value eval_return(const ReturnStmt* r);
    void eval_decl(const Decl* d);
    void eval_var_decl(const VarDecl* v);
    void eval_subr_decl(const SubrDecl* s);
    /* void eval_subr_decl(const SubrDecl* s);
    void eval_struct_decl(const StructDecl* s);
    void eval_enum_decl(const EnumDecl* e);
    void eval_union_decl(const UnionDecl* u);
    void eval_tool_decl(const ToolDecl* t);
    void eval_kit_decl(const KitDecl* k); */ // To be implemented in the near future  
};


