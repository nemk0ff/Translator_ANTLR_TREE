#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "Ast.h"

class Interpreter {
 public:
  // Возвращает 0 если всё ок, 2 если семантическая ошибка
  int execute(const ast::Program &program);

 private:
  std::map<std::string, const ast::FunctionDecl *> functions_;

  using Env = std::map<std::string, int>;

  std::optional<int> callFunction(const std::string &name,
                                  const std::vector<int> &args);
  std::optional<int> executeBody(
      const std::vector<std::unique_ptr<ast::Stmt>> &body, Env &env);
  std::optional<int> executeStmt(const ast::Stmt &stmt, Env &env);
  int evalExpr(const ast::Expr &expr, const Env &env);
};
