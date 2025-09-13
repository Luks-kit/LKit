#pragma once
#include <variant>
#include <string>


enum class ValueType {
    Int,
    Bool,
    String,
    Void,
    Unknown // fallback for untyped
};

