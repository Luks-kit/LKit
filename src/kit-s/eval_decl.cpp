#include "eval.hpp"
#include "stmt.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include "interpret.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <string>




void Evaluator::eval_decl(const Decl* d) {
    switch (d->kind) {
        case DeclKind::Var:
            eval_var_decl(static_cast<const VarDecl*>(d));
            break;
        case DeclKind::Subr:
            eval_subr_decl(static_cast<const SubrDecl*>(d)  );
            break; /*
        case DeclKind::Struct:
            eval_struct_decl(static_cast<const StructDecl*>(d));
            break;
        case DeclKind::Enum:
            eval_enum_decl(static_cast<const EnumDecl*>(d)); 
            break;
        case DeclKind::Union:
            eval_union_decl(static_cast<const UnionDecl*>(d));
            break;
        case DeclKind::Tool:
            eval_tool_decl(static_cast<const ToolDecl*>(d));
            break;
        case DeclKind::Kit:
            eval_kit_decl(static_cast<const KitDecl*>(d));
            break; */ // To be implemented in the near future
    }
    

}

void Evaluator::eval_var_decl(const VarDecl* v){
    Value init_val; // default initialization
    if (EvalRuntime::stringToValueType(v->type_name) == ValueType::NONE && !v->init)
        throw RuntimeError("Variable " + v->name + " declared with void type");
    if (EvalRuntime::is_user_type(v->type_name) && !v->init)
        throw RuntimeError("Variable " + v->name + " of user-defined type " + v->type_name + " must be initialized");
    init_val.type = EvalRuntime::stringToValueType(v->type_name);
    if (init_val.type == ValueType::USERDEFINED && !EvalRuntime::is_user_type(v->type_name))
        throw RuntimeError("Unknown type for variable " + v->name + ": " + v->type_name);
    if (init_val.type != ValueType::USERDEFINED && v->init && EvalRuntime::stringToValueType(v->type_name) == ValueType::USERDEFINED)
        throw RuntimeError("Type mismatch in initialization of variable " + v->name + 
            ": expected user-defined type but got basic type");
    if (v->init && init_val.type == ValueType::USERDEFINED) {
        Value expr_val = eval_expr(v->init.get());
        if (expr_val.type != ValueType::USERDEFINED || 
            std::get<userdefined>(expr_val.value).type_name != v->type_name) {
            throw RuntimeError("Type mismatch in initialization of variable " + v->name + 
                ": expected user-defined type " + v->type_name + 
                " but got " + std::get<userdefined>(expr_val.value).type_name);
        }
        init_val = expr_val;
    } else if (v->init) {
        Value expr_val = eval_expr(v->init.get());
        if (expr_val.type != init_val.type) {
            // Allow implicit conversion from int to float/double
            if (init_val.type == ValueType::FLOAT && expr_val.type == ValueType::INT) {
                expr_val.promoteToFloat();
            } else if (init_val.type == ValueType::DOUBLE && expr_val.type == ValueType::INT) {
                double d = static_cast<double>(std::get<int>(expr_val.value));
                expr_val = Value(d);
                expr_val.type = ValueType::DOUBLE;
            } else {
                throw RuntimeError("Type mismatch in initialization of variable " + v->name + 
                    ": expected " + runtime.valueTypeToString(init_val.type) + 
                    " but got " + runtime.valueTypeToString(expr_val.type));
            }
        }
        init_val = expr_val;
    }
    // Add variable to current scope
    runtime.decl_var(v->name, init_val);
}

void Evaluator::eval_subr_decl(const SubrDecl* s){
    // Register subroutine in the global subr registry
    std::vector<std::string> param_names;
    for (const auto &p : s->params) param_names.push_back(p.first);
    // Expect the body to be a BlockStmt; store pointer for calls
    const BlockStmt* body = nullptr;
    if (s->body) {
        body = dynamic_cast<const BlockStmt*>(s->body.get());
        if (!body) {
            // If body is not a BlockStmt, we still store nullptr and let calls fail later
            body = nullptr;
        }
    }
    runtime.decl_subr(s->name, param_names, body);
 }


 /*
void eval_struct_decl(const StructDecl* s);

void eval_enum_decl(const EnumDecl* e);

void eval_union_decl(const UnionDecl* u);

void eval_tool_decl(const ToolDecl* t);

void eval_kit_decl(const KitDecl* k);  */  // To be implemented in the near future