#include "interpret.hpp"
#include <map>
#include <unordered_map>
#include "eval.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "decl.hpp"
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


void EvalRuntime::push_scope() { scopes.push_back({}); }
void EvalRuntime::pop_scope() {
    for (auto& v : scopes.back()) delete v.addr;
    scopes.pop_back();
}
void EvalRuntime::push_scope(Var v) {
    scopes.push_back({v});
}

void EvalRuntime::cleanup_scopes() {
    while (!scopes.empty()) pop_scope();
}

Value EvalRuntime::call_subr(const std::string& name, const std::vector<Value>& args, Evaluator& evaluator) {
    auto it = subrs.find(name);
    if (it == subrs.end())
        throw std::runtime_error("Undefined subroutine: " + name);

    const Subr& subr = it->second;
    if (args.size() != subr.params.size())
        throw std::runtime_error("Argument count mismatch in call to: " + name);

    push_scope();
    for (size_t i = 0; i < args.size(); ++i)
        decl_var(subr.params[i], args[i]);

    Value ret = Value(); // default void return
    try{ evaluator.eval(subr.body); }
     catch (const ReturnException& re) { ret = re.value; }
    pop_scope();
    return ret;
}

Value EvalRuntime::get_var(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        for (auto& v : *it) if (v.name == name) return *(v.addr);
    }
    throw std::runtime_error(std::string("Undefined variable: ") + name);
}

void EvalRuntime::set_var(const std::string& name, const Value& value) {
    for (auto scope  = scopes.rbegin(); scope != scopes.rend(); ++scope) {
        for (auto& v : *scope) {
            if (v.name == name) {
                *(v.addr) = value;
                return;
            }
        }
    }
    throw std::runtime_error(std::string("Undefined variable: ") + name);
}


void EvalRuntime::decl_subr(const std::string& name, const std::vector<std::string>& params, const BlockStmt* body) {
    subrs[name] = Subr{params, body};
}  

void EvalRuntime::decl_var(const std::string& name, const Value& value) {
    // Check if variable already exists in current scope
    for (const auto& v : scopes.back()) {
        if (v.name == name) {
            throw std::runtime_error("Variable already declared in this scope: " + name);
        }
    }
    // Add new variable to current scope
    Value* val_ptr = new Value(value);
    scopes.back().push_back(Var{name, val_ptr});
}

std::string EvalRuntime::valueTypeToString(ValueType t) {
    switch (t) {
        case ValueType::INT: return "int";
        case ValueType::SHORT: return "short";
        case ValueType::LONG: return "long";
        case ValueType::FLOAT: return "float";
        case ValueType::DOUBLE: return "double";
        case ValueType::CHAR: return "char";
        case ValueType::BOOL: return "bool";
        case ValueType::STRING: return "string";
        case ValueType::NONE: return "void";
        case ValueType::USERDEFINED: return "userdefined";
        default: return "unknown";
    }
}
ValueType EvalRuntime::stringToValueType(const std::string& s) {
    auto it = types.find(s);
    if (it != types.end()) return it->second;
    if (is_user_type(s)) return ValueType::USERDEFINED;
    throw std::runtime_error("Unknown type: " + s);
}

std::unordered_map<std::string, EvalRuntime::Subr> EvalRuntime::subrs;
std::unordered_map<std::string, ValueType> EvalRuntime::types;
std::vector<std::string> EvalRuntime::user_types;