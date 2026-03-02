#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace ast {

struct Node {
  virtual ~Node() = default;
  virtual void print(std::ostream &os, int indent = 0) const = 0;
};

inline void printIndent(std::ostream &os, int indent) {
  for (int i = 0; i < indent; ++i) {
    os << "  ";
  }
}

struct Expr : Node {};

struct NumberExpr final : Expr {
  std::string value;
  explicit NumberExpr(std::string v) : value(std::move(v)) {}
  void print(std::ostream &os, int indent) const override;
};

struct IdExpr final : Expr {
  std::string name;
  explicit IdExpr(std::string n) : name(std::move(n)) {}
  void print(std::ostream &os, int indent) const override;
};

struct CallExpr final : Expr {
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;
  void print(std::ostream &os, int indent) const override;
};

struct BinaryExpr final : Expr {
  std::string op;
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  void print(std::ostream &os, int indent) const override;
};

struct Stmt : Node {};

struct PrintStmt final : Stmt {
  std::unique_ptr<Expr> expr;
  void print(std::ostream &os, int indent) const override;
};

struct VarDeclStmt final : Stmt {
  std::string typeName;
  std::string varName;
  void print(std::ostream &os, int indent) const override;
};

struct ReturnStmt final : Stmt {
  std::unique_ptr<Expr> expr;  // Если тут nullptr, значит это просто "return;"
  void print(std::ostream &os, int indent) const override;
};

struct CallStmt final : Stmt {
  std::unique_ptr<CallExpr> call;
  void print(std::ostream &os, int indent) const override;
};

struct Param {
  std::string typeName;
  std::string name;
};

struct FunctionDecl final : Node {
  std::string returnType;
  std::string name;
  std::vector<Param> params;
  std::vector<std::unique_ptr<Stmt>> body;
  void print(std::ostream &os, int indent) const override;
};

struct Program final : Node {
  std::vector<std::unique_ptr<FunctionDecl>> functions;
  std::vector<std::unique_ptr<CallStmt>> calls;
  void print(std::ostream &os, int indent) const override;
};

}