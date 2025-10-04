#pragma once
#include <variant>
#include <string>


enum class ValueType {
    Int,
    Double,
    Bool,
    String,
    Void,
    Unknown // fallback for untyped
};

using ValueStore = std::variant<int, double, bool,std::string>;

struct Value {
    ValueType type;
    ValueStore actualvalue;

    template<typename T> 
    T get_as() { return std::get<T>(actualvalue); }

        
};