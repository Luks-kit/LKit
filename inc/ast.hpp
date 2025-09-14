#ifndef AST_HPP
#define AST_HPP

#include <string>

enum class ASTType {
    Number,
    Char,
    Ident,
    BinOp,
    Assign,
    If,
    While,
    Block,
};

struct AST {
    ASTType type;
    union{
        // Values
        int value = 0;            // for number or char
        std::string name;         
        struct {
            // Binop
            std::string op;
            AST* left = nullptr;
            AST* right = nullptr;
        } binop;
        struct {
            // Assign
            std::string name;
            AST* expr = nullptr;
        } assign;
        struct {
            AST* cond, *then_branch, *else_branch;
        } ifstmt;
        struct {std::vector<AST*> stmts; } block;
    };
    // Constructors
    static AST* make_number(int v);
    static AST* make_char(int v);
    static AST* make_ident(const std::string& name);
    static AST* make_binop(const std::string& op, AST* l, AST* r);
    static AST* make_assign(const std::string& name, AST* expr);
    static AST* make_if(AST* cond, AST* then_branch, AST* else_branch = nullptr) {
        AST* n = new AST();
        n->type = ASTType::If;
        n->ifstmt.cond = cond;
        n->ifstmt.then_branch = then_branch;
        n->ifstmt.else_branch = else_branch;
        return n;
    }
    static AST* make_block(const std::vector<AST*>& stmts) {
        AST* n = new AST();
        n->type = ASTType::Block;
        n->block.stmts = stmts;
        return n;
    }
};

#endif

