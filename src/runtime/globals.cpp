#include "interpret.hpp"

std::vector<Scope> scopes = { {} };
std::unordered_map<std::string, Subr> subrs;
std::unordered_map<std::string, ValueType> types = {
    {"int", ValueType::INT},
    {"short", ValueType::SHORT},
    {"long", ValueType::LONG},
    {"float", ValueType::FLOAT},
    {"double", ValueType::DOUBLE},
    {"bool", ValueType::BOOL},
    {"char", ValueType::CHAR},
    {"string", ValueType::STRING},
    {"void", ValueType::NONE}
};
std::vector<std::string> user_types;
