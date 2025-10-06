#pragma once

#include <stdexcept>
#include <string>

struct ParseError : public std::runtime_error {
    explicit ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

struct RuntimeError : public std::runtime_error {
    explicit RuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};
struct TypeError : public std::runtime_error {
    explicit TypeError(const std::string& msg) : std::runtime_error(msg) {}
};
struct NameError : public std::runtime_error {
    explicit NameError(const std::string& msg) : std::runtime_error(msg) {}
};
struct ValueError : public std::runtime_error {
    explicit ValueError(const std::string& msg) : std::runtime_error(msg) {}
};
struct IndexError : public std::runtime_error {
    explicit IndexError(const std::string& msg) : std::runtime_error(msg) {}
};
struct KeyError : public std::runtime_error {
    explicit KeyError(const std::string& msg) : std::runtime_error(msg) {}
};
struct AttributeError : public std::runtime_error {
    explicit AttributeError(const std::string& msg) : std::runtime_error(msg) {}
};
struct ImportError : public std::runtime_error {
    explicit ImportError(const std::string& msg) : std::runtime_error(msg) {}
};
struct ZeroDivisionError : public std::runtime_error {
    explicit ZeroDivisionError(const std::string& msg) : std::runtime_error(msg) {}
};
struct OverflowError : public std::runtime_error {
    explicit OverflowError(const std::string& msg) : std::runtime_error(msg) {}
};
struct UnderflowError : public std::runtime_error {
    explicit UnderflowError(const std::string& msg) : std::runtime_error(msg) {}
};
struct MemoryError : public std::runtime_error {
    explicit MemoryError(const std::string& msg) : std::runtime_error(msg) {}
};
struct RecursionError : public std::runtime_error {                         
    explicit RecursionError(const std::string& msg) : std::runtime_error(msg) {}
};
struct NotImplementedError : public std::runtime_error {
    explicit NotImplementedError(const std::string& msg) : std::runtime_error(msg) {}
};
struct EOFError : public std::runtime_error {
    explicit EOFError(const std::string& msg) : std::runtime_error(msg) {}
};
struct KeyboardInterrupt : public std::runtime_error {
    explicit KeyboardInterrupt(const std::string& msg) : std::runtime_error(msg) {}
};
struct SystemExit : public std::runtime_error {
    explicit SystemExit(const std::string& msg) : std::runtime_error(msg) {}
};  