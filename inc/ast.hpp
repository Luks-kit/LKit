#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>


enum class ASTType {
    Number,
    Char,
    Ident,
    BinOp,
    Assign,
    AssignOp,
    IncDec,
    Check,
    Recheck,
    Block,
};

struct AST {
    ASTType type;
    union{
        // Values
        int value = 0;            // for number or char
        std::string name;         
        struct { std::string op; AST* lhs = nullptr; AST* rhs = nullptr; } binop;
        struct { std::string name; AST* expr = nullptr; } assign;
        struct { AST* cond, *ok_branch, *then_branch;} checkstmt;
        struct { std::string name; AST* rhs; char op; } assign_op;
        struct { std::string name; char op;  } incdec;
        struct { AST* cond; AST* body;} recheckstmt;
        struct {std::vector<AST*> stmts; } block;
    };
    // Constructors
    static AST* make_number(int v);
    static AST* make_char(int v);
    static AST* make_ident(const std::string& name);
    static AST* make_binop(const std::string& op, AST* l, AST* r);
    static AST* make_assign(const std::string& name, AST* expr);
    
    static AST* make_check(AST* cond, AST* ok_branch, AST* then_branch = nullptr) {
        AST* n = new AST();
        n->type = ASTType::Check;
        n->checkstmt.cond = cond;
        n->checkstmt.ok_branch = ok_branch;
        n->checkstmt.then_branch = then_branch;
        return n;
    }
    static AST* make_block(const std::vector<AST*>& stmts) {
        AST* n = new AST();
        n->type = ASTType::Block;
        n->block.stmts = stmts;
        return n;
    }

   static AST* make_recheck(AST* cond, AST* body) {
        AST* n = new AST;
        n->type = ASTType::Recheck;
        n->recheckstmt.cond = cond;
        n->recheckstmt.body = body;
        return n;
    }

   static AST* make_assign_op(const std::string& name, AST* rhs, char op) {
        AST* n = new AST;
        n->type = ASTType::AssignOp;
        n->assign_op.name = name;
        n->assign_op.rhs = rhs;
        n->assign_op.op = op; // '+', '-', '*', '/'
        return n;
    }

    static AST* make_incdec(const std::string& name, char op) {
        AST* n = new AST;
        n->type = ASTType::IncDec;
        n->incdec.name = name;
        n->incdec.op = op; // '+' for ++, '-' for --
        return n;
    }

};

#endif

