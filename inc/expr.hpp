#pragma once
#include "node.hpp"
#include "value.hpp"
#include <string>
#include <memory>

enum class ExprKind { Literal, Ident, Binary, Unary, Call };

struct Expr : Node {
    ExprKind kind;
    explicit Expr(ExprKind k) : Node(NodeType::Expr), kind(k) { }
    virtual ~Expr() = default;
};

// Literal numbers, strings, etc.
struct LiteralExpr : Expr {
    Value literal;
    LiteralExpr(const Value& lit) 
        : Expr(ExprKind::Literal), literal(lit) {}
};

struct IdentExpr : Expr {
    std::string name;
    IdentExpr(const std::string& n) 
        : Expr(ExprKind::Ident), name(n) {}
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

struct CallExpr : Expr {
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> args;
    CallExpr(std::unique_ptr<Expr> c, std::vector<std::unique_ptr<Expr>> a)
        : Expr(ExprKind::Call), callee(std::move(c)), args(std::move(a)) {}
};