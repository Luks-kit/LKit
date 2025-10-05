#pragma once
#include "node.hpp"
#include "expr.hpp"
#include <vector>
#include <memory>

enum class StmtKind { ExprStmt, Block, If, While, Return, Check, Recheck };

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

struct ForEachStmt : Stmt {
    std::unique_ptr<Expr> iterable;
    std::unique_ptr<Stmt> body;
    std::string var_name;
    ForEachStmt(std::string v, std::unique_ptr<Expr> iter, std::unique_ptr<Stmt> b)
        : Stmt(StmtKind::While), iterable(std::move(iter)), body(std::move(b)), var_name(std::move(v)) {}
};

struct CheckArms {
    std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Stmt>>> arms;
    std::unique_ptr<Stmt> else_arm;
    CheckArms() = default;
};

struct CheckStmt : Stmt {
    std::unique_ptr<Expr> expr;
    CheckArms arms;
    bool execute_first_match = false; // true if "check(expr) only", false if "check(expr) on"
    CheckStmt(std::unique_ptr<Expr> e, CheckArms a)
        : Stmt(StmtKind::If), expr(std::move(e)), arms(std::move(a)) {}
};

struct RecheckStmt : Stmt {
    std::unique_ptr<Expr> comp;
    CheckArms arms;
    RecheckStmt(std::unique_ptr<Expr> c, CheckArms a)
        : Stmt(StmtKind::Recheck), comp(std::move(c)), arms(std::move(a)) {}
};

