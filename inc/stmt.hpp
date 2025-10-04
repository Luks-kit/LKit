#pragma once
#include "node.hpp"
#include "expr.hpp"
#include <vector>
#include <memory>

enum class StmtKind { ExprStmt, Block, If, While, Return };

struct Stmt : Node {
    StmtKind kind;
    explicit Stmt(StmtKind k) : Node(NodeType::Stmt), kind(k) {}
    virtual ~Stmt() = default;
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
    ExprStmt(std::unique_ptr<Expr> e) 
        : Stmt(StmtKind::ExprStmt), expr(std::move(e)) {}
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> stmts;
    BlockStmt() : Stmt(StmtKind::Block) {}
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> then_branch, else_branch;
    IfStmt(std::unique_ptr<Expr> c, std::unique_ptr<Stmt> t, std::unique_ptr<Stmt> e)
        : Stmt(StmtKind::If), cond(std::move(c)), then_branch(std::move(t)), else_branch(std::move(e)) {}
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> body;
    WhileStmt(std::unique_ptr<Expr> c, std::unique_ptr<Stmt> b)
        : Stmt(StmtKind::While), cond(std::move(c)), body(std::move(b)) {}
};
