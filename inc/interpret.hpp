#pragma once
#include "eval.hpp"
#include "stmt.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <variant>
#include <memory>
#include <cmath>


struct Var { std::string name; Value* addr; };
using Scope = std::vector<Var>;
extern std::vector<Scope> scopes;

inline void push_scope() { scopes.push_back({}); }
inline void pop_scope() {
    for (auto& v : scopes.back()) delete v.addr;
    scopes.pop_back();
}
inline void cleanup_scopes() { while (!scopes.empty()) pop_scope(); }

inline void decl_var(const std::string& name, Value value) {
    scopes.back().push_back({name, new Value(value)});
}

// Subroutines support
struct Subr {
    std::vector<std::string> params;
    const BlockStmt* body;
};

extern std::unordered_map<std::string, Subr> subrs;

// Exception for return statements
struct ReturnException : public std::exception {
    Value value;
    ReturnException(Value v) : value(v) {}
};

inline void decl_subr(const std::string& name, const std::vector<std::string>& params, const BlockStmt* body) {
    subrs[name] = Subr{params, body};
}

inline Value call_subr(const std::string& name, const std::vector<Value>& args) {
    auto it = subrs.find(name);
    if (it == subrs.end())
        throw std::runtime_error("Undefined subroutine: " + name);

    const Subr& subr = it->second;
    if (args.size() != subr.params.size())
        throw std::runtime_error("Argument count mismatch in call to: " + name);

    push_scope();
    for (size_t i = 0; i < args.size(); ++i)
        decl_var(subr.params[i], args[i]);

    Value ret = Evaluator().eval(subr.body);
    pop_scope();
    return ret;
}




inline Value get_var(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        for (auto& v : *it) if (v.name == name) return *(v.addr);
    }
    throw std::runtime_error(std::string("Undefined variable: ") + name);
}

inline void set_var(const std::string& name, Value value) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Scope &scope_ref = *it; // local alias to the current scope (most-recent first)
        auto found = std::find_if(scope_ref.begin(), scope_ref.end(), [&](const Var& v){ return v.name == name; });
        if (found != scope_ref.end()) { *(found->addr) = value; return; }
    }

    throw std::runtime_error(std::string("Undefined variable: ") + name);  // "let" works, no auto decl for now
}

// Type registry for basic types
extern std::unordered_map<std::string, ValueType> types;
// Type registry for user defined types

extern std::vector <std::string> user_types;
inline bool is_user_type(const std::string& name) {
    return std::find(user_types.begin(), user_types.end(), name) != user_types.end();
}
inline std::string valueTypeToString(ValueType t) {
    switch (t) {
        case ValueType::INT: return "int";
        case ValueType::SHORT: return "short";
        case ValueType::LONG: return "long";
        case ValueType::FLOAT: return "float";
        case ValueType::DOUBLE: return "double";
        case ValueType::BOOL: return "bool";
        case ValueType::CHAR: return "char";
        case ValueType::STRING: return "string";
        case ValueType::USERDEFINED: return "userdefined";
        case ValueType::NONE: return "void";
        default: return "unknown";
    }
}
inline ValueType stringToValueType(const std::string& s) {
    auto it = types.find(s);
    if (it != types.end()) return it->second;
    if (is_user_type(s)) return ValueType::USERDEFINED; 
    throw std::runtime_error("Unknown type: " + s);
}