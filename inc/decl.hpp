#pragma once
#include "node.hpp"
#include <string>
#include <vector>
#include <memory>
#include <utility>

// Forward declarations to avoid circular includes between decl.hpp and stmt.hpp
struct Expr;
struct Stmt;

enum class DeclKind { Var, Subr, Struct, Enum, Union, Tool, Kit };

struct Decl : Node {
    DeclKind kind;
    explicit Decl(DeclKind k) : Node(NodeType::Decl), kind(k) { }
    virtual ~Decl() = default;
};

struct VarDecl : Decl {
    std::string name;
    std::string type_name; 
    std::unique_ptr<Expr> init;
    VarDecl(std::string n, std::string t, std::unique_ptr<Expr> i)
        : Decl(DeclKind::Var), name(std::move(n)), type_name(std::move(t)), init(std::move(i)) {}
};

struct SubrDecl : Decl {
    std::string name;
    std::string return_type;
    std::vector<std::pair<std::string, std::string>> params;
    std::unique_ptr<Stmt> body;
    SubrDecl(std::string n, std::string rt)
        : Decl(DeclKind::Subr), name(std::move(n)), return_type(std::move(rt)) {}
};

struct StructDecl : Decl {
    std::string name;
    std::vector<std::pair<std::string, std::string>> fields;
    StructDecl(std::string n) : Decl(DeclKind::Struct), name(std::move(n)) {}
};

struct EnumDecl : Decl {
    std::string name;
    std::vector<std::string> values;
    EnumDecl(std::string n) : Decl(DeclKind::Enum), name(std::move(n)) {}
};

struct UnionDecl : Decl {
    std::string name;
    std::vector<std::pair<std::string, std::string>> variants;
    UnionDecl(std::string n) : Decl(DeclKind::Union), name(std::move(n)) {}
};

struct ToolDecl : Decl {
    std::string name;
    std::vector<std::unique_ptr<Decl>> methods;
    ToolDecl(std::string n) : Decl(DeclKind::Tool), name(std::move(n)) {}
};

struct KitDecl : Decl {
    std::string name;
    std::vector<std::unique_ptr<Decl>> exports;
    KitDecl(std::string n) : Decl(DeclKind::Kit), name(std::move(n)) {}
};

