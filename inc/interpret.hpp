#pragma once
#include "stmt.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include "value.hpp"
#include "eval.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <variant>
#include <memory>
#include <cmath>

struct Evaluator; // forward declaration

class EvalRuntime {
public:
    struct Var { std::string name; Value* addr; };
    using Scope = std::vector<Var>;

    EvalRuntime() = default;
    ~EvalRuntime() { cleanup_scopes(); }

    void push_scope(); 
    void push_scope(Var v);
    void pop_scope();
    void cleanup_scopes();
    Scope get_current_scope() const { return scopes.back(); }
    bool is_scope_empty() const { return scopes.empty(); }
    // Subroutines support
    struct Subr {
        std::vector<std::string> params;
        const BlockStmt* body;
    };

    static std::unordered_map<std::string, Subr> subrs;

    // Exception for return statements
    struct ReturnException : public std::exception {
        Value value;
        ReturnException(Value v) : value(v) {}
    };

    void decl_subr(const std::string& name, const std::vector<std::string>& params, const BlockStmt* body);

    Value call_subr(const std::string& name, const std::vector<Value>& args, Evaluator& evaluator);

    Value get_var(const std::string& name);

    void set_var(const std::string& name, const Value& value);
    void decl_var(const std::string& name, const Value& value);
    // Type registry for basic types
    static std::unordered_map<std::string, ValueType> types;
    // Type registry for user defined types
    static std::vector<std::string> user_types;

    static bool is_user_type(const std::string& name) {
        return std::find(user_types.begin(), user_types.end(), name) != user_types.end();
    }
    static std::string valueTypeToString(ValueType t); 
    static ValueType stringToValueType(const std::string& s);

private:
    std::vector<Scope> scopes;
};