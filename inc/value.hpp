#pragma once

#include <variant>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <cmath>

enum class ValueType {
    INT,
    SHORT,
    LONG,
    FLOAT,
    DOUBLE,
    BOOL,
    CHAR,
    STRING,
    NONE
};

using IntegralValue = std::variant<int, short, long, float, double, bool, char, std::string>;

struct Value {
    ValueType type = ValueType::NONE;
    IntegralValue value;

    Value() = default;
    Value(IntegralValue val): value(val) {
        setTypeFromVariant();
    }
    Value(int val): value(val) {setTypeFromVariant();}
    Value(short val): value(val) {setTypeFromVariant();}
    Value(long val): value(val) {setTypeFromVariant();}
    Value(float val): value(val) {setTypeFromVariant();}
    Value(double val): value(val) {setTypeFromVariant();}
    Value(bool val): value(val) {setTypeFromVariant();}
    Value(char val): value(val) {setTypeFromVariant();}
    Value(const std::string& val): value(val) {setTypeFromVariant();}
    Value(ValueType t, IntegralValue v): type(t), value(v) {}

    // Utility: infer type from variant
    void setTypeFromVariant() {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) type = ValueType::INT;
            else if constexpr (std::is_same_v<T, short>) type = ValueType::SHORT;
            else if constexpr (std::is_same_v<T, long>) type = ValueType::LONG;
            else if constexpr (std::is_same_v<T, float>) type = ValueType::FLOAT;
            else if constexpr (std::is_same_v<T, double>) type = ValueType::DOUBLE;
            else if constexpr (std::is_same_v<T, bool>) type = ValueType::BOOL;
            else if constexpr (std::is_same_v<T, char>) type = ValueType::CHAR;
            else if constexpr (std::is_same_v<T, std::string>) type = ValueType::STRING;
            else type = ValueType::NONE;
        }, value);
    }

    // Getters (templated)
    template <typename T>
    T get() const {
        return std::get<T>(value);
    }

    // Setters (auto-detect type)
    template <typename T>
    void set(T v) {
        value = v;
        setTypeFromVariant();
    }

    // Convert int to float
    void promoteToFloat() {
        if (std::holds_alternative<int>(value)) {
            float f = static_cast<float>(std::get<int>(value));
            value = f;
            type = ValueType::FLOAT;
        }
    }

    std::string toString() const {
        return std::visit([](auto&& v) -> std::string {
            using T = std::decay_t<decltype(v)>;

            if constexpr (std::is_same_v<T, std::monostate>)
                return "null";
            else if constexpr (std::is_same_v<T, bool>)
                return v ? "true" : "false";
            else if constexpr (std::is_same_v<T, char>)
                return std::string("'") + v + "'";
            else if constexpr (std::is_arithmetic_v<T>)
                return std::to_string(v);
            else if constexpr (std::is_same_v<T, std::string>)
                return "\"" + v + "\"";
            else
                return "<unknown>";
        }, value);
    }   

    // Arithmetic operators
    Value operator+(const Value& other) const { return arithmeticOp(other, std::plus<>()); }
    Value operator-(const Value& other) const { return arithmeticOp(other, std::minus<>()); }
    Value operator*(const Value& other) const { return arithmeticOp(other, std::multiplies<>()); }
    Value operator/(const Value& other) const { return arithmeticOp(other, std::divides<>()); }
    Value operator&(const Value& other) const { return binaryOp(other, std::bit_and<>()); }
    Value operator|(const Value& other) const { return binaryOp(other, std::bit_or<>()); }
    Value operator^(const Value& other) const { return binaryOp(other, std::bit_xor<>()); }
    Value pow(const Value& other) const {
        // Only for numeric types
        if (!isNumeric() || !other.isNumeric()) {
            throw std::runtime_error("Exponentiation requires numeric types");
        }
        double base = toDouble();
        double exponent = other.toDouble();
        return Value(static_cast<double>(std::pow(base, exponent)));
    }
    Value operator&&(const Value& other) const {
    return std::visit([](auto&& lhs, auto&& rhs) -> Value {
        if constexpr(std::is_same_v<std::decay_t<decltype(lhs)>, bool> &&
                     std::is_same_v<std::decay_t<decltype(rhs)>, bool>)
                return Value(lhs && rhs);
            else throw std::runtime_error("&& requires bools");
        }, value, other.value);
    }

    Value operator||(const Value& other) const {
    return std::visit([](auto&& lhs, auto&& rhs) -> Value {
        if constexpr(std::is_same_v<std::decay_t<decltype(lhs)>, bool> &&
                     std::is_same_v<std::decay_t<decltype(rhs)>, bool>)
                return Value(lhs || rhs);
            else throw std::runtime_error("|| requires bools");
        }, value, other.value);
    }

    Value operator~() const { return unaryOp(std::bit_not<>());}

    // Comparison
    bool operator==(const Value& other) const {
        if (type != other.type) return false;
        return value == other.value;
    }

    bool operator!=(const Value& other) const {
        return !(*this == other);
    }

private:
    template <typename Op>
    Value arithmeticOp(const Value& other, Op op) const {
        return std::visit([&](auto&& lhs, auto&& rhs) -> Value {
        using L = std::decay_t<decltype(lhs)>;
        using R = std::decay_t<decltype(rhs)>;

        // Only allow arithmetic types (int, double, etc.)
        if constexpr (std::is_arithmetic_v<L> && std::is_arithmetic_v<R>) {
            // Compute result in the "wider" type
            using ResultType = std::common_type_t<L, R>;
            return Value(static_cast<ResultType>(op(lhs, rhs)));
        } 
        else {
            throw std::runtime_error("Invalid arithmetic operation");
        }
    }, value, other.value);
    }
    template <typename Op>
    Value binaryOp(const Value& other, Op op) const {
        return std::visit([&](auto&& lhs, auto&& rhs) -> Value {
        using L = std::decay_t<decltype(lhs)>;
        using R = std::decay_t<decltype(rhs)>;

        // Handle only types where & makes sense:
        if constexpr (std::is_integral_v<L> && std::is_integral_v<R>) {
            return Value(op(lhs, rhs));
        } else if constexpr (std::is_same_v<L, bool> && std::is_same_v<R, bool>) {
            // e.g. bitwise/logic for bool
            return Value(op(lhs, rhs));
        } else {
            throw std::runtime_error("Invalid operands for binary operator");
        }
    }, value, other.value);
    }

    template <typename Op>
    Value unaryOp(Op op) const {
        return std::visit([&](auto&& v) -> Value {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_integral_v<T>) {
            // bitwise NOT only for integers
            return Value(op(v));
        } else if constexpr (std::is_same_v<T, bool>) {
            // logical NOT for bools
            return Value(!v);
        } else {
            throw std::runtime_error("Invalid operand for unary operator");
        }
    }, value);
    }

    bool isNumeric() const {
        return std::holds_alternative<int>(value) ||
               std::holds_alternative<short>(value) ||
               std::holds_alternative<long>(value) ||
               std::holds_alternative<float>(value) ||
               std::holds_alternative<double>(value);
    }
    bool isBitFieldable() const {
        return std::holds_alternative<int>(value) ||
               std::holds_alternative<short>(value) ||
               std::holds_alternative<long>(value);
    }

    double toDouble() const {
        return std::visit([](auto&& arg) -> double {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) return arg ? 1.0 : 0.0;
            else if constexpr (std::is_arithmetic_v<T>) return static_cast<double>(arg);
            else throw std::runtime_error("Cannot convert to double");
        }, value);
    }
    
};
