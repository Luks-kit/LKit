#pragma once

#include <stdexcept>
#include <string>

struct ParseError : public std::runtime_error {
    explicit ParseError(const std::string& msg) : std::runtime_error(msg) {}
};
