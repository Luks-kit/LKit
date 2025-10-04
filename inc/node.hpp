#pragma once
#include <string>

enum class NodeType { Expr, Stmt, Decl };

struct Node {
  NodeType nodeType;
  explicit Node(NodeType t): nodeType(t) {}
  virtual ~Node() = default;
};
