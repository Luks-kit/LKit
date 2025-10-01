#include "ast.hpp"

AST* AST::make_number(int v) {
    AST* n = new AST();
    n->type = ASTType::Number;
    n->value = v;
    return n;
}

AST* AST::make_char(int v) {
    AST* n = new AST();
    n->type = ASTType::Char;
    n->value = v;
    return n;
}

AST* AST::make_ident(const std::string& name) {
    AST* n = new AST();
    n->type = ASTType::Ident;
    n->name = name;
    return n;
}

AST* AST::make_binop(const std::string& op, AST* l, AST* r) {
    AST* n = new AST();
    n->type = ASTType::BinOp;
    n->binop.op = op;
    n->binop.lhs = l;
    n->binop.rhs = r;
    return n;
}

AST* AST::make_assign(const std::string& name, AST* expr) {
    AST* n = new AST();
    n->type = ASTType::Assign;
    n->name = name;
    n->assign.expr = expr;
    return n;
}

