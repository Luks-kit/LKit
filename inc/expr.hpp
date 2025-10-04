#pragma once
#include "node.hpp"
#include <string>
#include <memory>

enum class ExprKind { Number, Var, Binary, Unary, Call };

struct Expr : Node {
    ExprKind kind;
    explicit Expr(ExprKind k) : kind(k), nodeType(NodeType::Expr) {}
    virtual ~Expr() = default;
};

// Literal numbers, strings, etc.
struct NumberExpr : Expr {
    std::string literal;
    NumberExpr(const std::string& lit) 
        : Expr(ExprKind::Number), literal(lit) {}
};

struct VarExpr : Expr {
    std::string name;
    VarExpr(const std::string& n) 
        : Expr(ExprKind::Var), name(n) {}
};

struct BinaryExpr : Expr {
    std::string op;
    std::unique_ptr<Expr> lhs, rhs;
    BinaryExpr(const std::string& o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : Expr(ExprKind::Binary), op(o), lhs(std::move(l)), rhs(std::move(r)) {}
};

struct UnaryExpr : Expr {
    std::string op;
    std::unique_ptr<Expr> operand;
    UnaryExpr(const std::string& o, std::unique_ptr<Expr> e)
        : Expr(ExprKind::Unary), op(o), operand(std::move(e)) {}
};
