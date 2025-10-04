#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <variant>
#include <memory>

struct AST; // forward declaration

using ASTPtr = AST*;

struct BinOp { std::string op; ASTPtr lhs = nullptr; ASTPtr rhs = nullptr; };
struct Assign { std::string name; ASTPtr expr = nullptr; };
struct Decla {std::string name; ASTPtr expr = nullptr; };
struct AssignOp { std::string name; ASTPtr rhs = nullptr; char op; };
struct IncDec { std::string name; char op; };
struct Check { ASTPtr cond; ASTPtr ok_branch; ASTPtr then_branch = nullptr; };
struct Recheck { ASTPtr cond; ASTPtr body; };
struct Block { std::vector<ASTPtr> stmts; };

using ASTValue = std::variant<
    int,           // Number or Char
    std::string,   // Ident
    BinOp,
    Assign,
    Decla,
    AssignOp,
    IncDec,
    Check,
    Recheck,
    Block
>;

enum class ASTType {
    Number,
    Char,
    Ident,
    BinOp,
    Assign,
    Decl,
    AssignOp,
    IncDec,
    Check,
    Recheck,
    Block,
};

struct AST {
    ASTType type;
    ASTValue value;

    // Factory functions
    static AST* make_number(int v) {
        return new AST{ASTType::Number, v};
    }

    static AST* make_char(int v) {
        return new AST{ASTType::Char, v};
    }

    static AST* make_ident(const std::string& name) {
        return new AST{ASTType::Ident, name};
    }

    static AST* make_binop(const std::string& op, AST* lhs, AST* rhs) {
        return new AST{ASTType::BinOp, BinOp{op, lhs, rhs}};
    }

    static AST* make_assign(const std::string& name, AST* expr) {
        return new AST{ASTType::Assign, Assign{name, expr}};
    }
    static AST* make_decl(const std::string& name, AST* expr) {
        return new AST{ASTType::Decl, Decla{name, expr}};
    }

    static AST* make_assign_op(const std::string& name, AST* rhs, char op) {
        return new AST{ASTType::AssignOp, AssignOp{name, rhs, op}};
    }

    static AST* make_incdec(const std::string& name, char op) {
        return new AST{ASTType::IncDec, IncDec{name, op}};
    }

    static AST* make_check(AST* cond, AST* ok_branch, AST* then_branch = nullptr) {
        return new AST{ASTType::Check, Check{cond, ok_branch, then_branch}};
    }

    static AST* make_recheck(AST* cond, AST* body) {
        return new AST{ASTType::Recheck, Recheck{cond, body}};
    }

    static AST* make_block(const std::vector<AST*>& stmts) {
        return new AST{ASTType::Block, Block{stmts}};
    }
    
     int as_int() const { return std::get<int>(value); }
    const std::string& as_string() const { return std::get<std::string>(value); }
    BinOp& as_binop() { return std::get<BinOp>(value); }
    Assign& as_assign() { return std::get<Assign>(value); }
    Decl& as_decl() {return std::get<Decl>(value); }
    AssignOp& as_assign_op() { return std::get<AssignOp>(value); }
    IncDec& as_incdec() { return std::get<IncDec>(value); }
    Check& as_check() { return std::get<Check>(value); }
    Recheck& as_recheck() { return std::get<Recheck>(value); }
    Block& as_block() { return std::get<Block>(value); }

};

#endif

