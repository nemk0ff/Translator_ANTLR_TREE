#include "Interpreter.h"

#include <iostream>
#include <stdexcept>

// Специальное исключение для возврата из функции через return
struct ReturnException {
  std::optional<int> value;
};

// Специальное исключение для семантических ошибок (не прерывает parse)
struct SemanticError {
  std::string message;
};

int Interpreter::execute(const ast::Program &program) {
  // Регистрируем все функции по имени
  for (const auto &fn : program.functions) {
    functions_[fn->name] = fn.get();
  }

  try {
    for (const auto &callStmt : program.calls) {
      std::vector<int> args;
      for (const auto &argExpr : callStmt->call->args) {
        Env emptyEnv;
        args.push_back(evalExpr(*argExpr, emptyEnv));
      }
      callFunction(callStmt->call->callee, args);
    }
  } catch (const SemanticError &e) {
    std::cout << "Semantic error: " << e.message << "\n";
    return 2;
  }

  return 0;
}

std::optional<int> Interpreter::callFunction(const std::string &name,
                                             const std::vector<int> &args) {
  auto it = functions_.find(name);
  if (it == functions_.end()) {
    throw SemanticError{"call to undefined function '" + name + "'"};
  }

  const ast::FunctionDecl *fn = it->second;

  if (args.size() != fn->params.size()) {
    throw SemanticError{"function '" + name + "' expects " +
                        std::to_string(fn->params.size()) + " argument(s), got " +
                        std::to_string(args.size())};
  }

  Env env;
  for (size_t i = 0; i < fn->params.size(); ++i) {
    env[fn->params[i].name] = args[i];
  }

  try {
    return executeBody(fn->body, env);
  } catch (const ReturnException &ret) {
    return ret.value;
  }
}

std::optional<int> Interpreter::executeBody(
    const std::vector<std::unique_ptr<ast::Stmt>> &body, Env &env) {
  for (const auto &stmt : body) {
    auto result = executeStmt(*stmt, env);
    if (result.has_value()) {
      return result;
    }
  }
  return std::nullopt;
}

std::optional<int> Interpreter::executeStmt(const ast::Stmt &stmt, Env &env) {
  if (const auto *ps = dynamic_cast<const ast::PrintStmt *>(&stmt)) {
    int value = evalExpr(*ps->expr, env);
    std::cout << value << "\n";
    return std::nullopt;
  }

  if (const auto *vs = dynamic_cast<const ast::VarDeclStmt *>(&stmt)) {
    // Объявление переменной без инициализации — ставим 0
    env[vs->varName] = 0;
    return std::nullopt;
  }

  if (const auto *rs = dynamic_cast<const ast::ReturnStmt *>(&stmt)) {
    if (rs->expr) {
      return evalExpr(*rs->expr, env);
    }
    return std::nullopt;
  }

  if (const auto *cs = dynamic_cast<const ast::CallStmt *>(&stmt)) {
    std::vector<int> args;
    for (const auto &argExpr : cs->call->args) {
      args.push_back(evalExpr(*argExpr, env));
    }
    callFunction(cs->call->callee, args);
    return std::nullopt;
  }

  throw SemanticError{"unsupported statement type"};
}

int Interpreter::evalExpr(const ast::Expr &expr, const Env &env) {
  if (const auto *ne = dynamic_cast<const ast::NumberExpr *>(&expr)) {
    return std::stoi(ne->value);
  }

  if (const auto *ie = dynamic_cast<const ast::IdExpr *>(&expr)) {
    auto it = env.find(ie->name);
    if (it == env.end()) {
      throw SemanticError{"undefined variable '" + ie->name + "'"};
    }
    return it->second;
  }

  if (const auto *be = dynamic_cast<const ast::BinaryExpr *>(&expr)) {
    int left = evalExpr(*be->left, env);
    int right = evalExpr(*be->right, env);
    if (be->op == "+") return left + right;
    if (be->op == "-") return left - right;
    throw SemanticError{"unknown operator '" + be->op + "'"};
  }

  if (const auto *ce = dynamic_cast<const ast::CallExpr *>(&expr)) {
    std::vector<int> args;
    for (const auto &argExpr : ce->args) {
      args.push_back(evalExpr(*argExpr, env));
    }
    auto result = callFunction(ce->callee, args);
    if (!result.has_value()) {
      throw SemanticError{"function '" + ce->callee +
                          "' used as expression but returns void"};
    }
    return result.value();
  }

  throw SemanticError{"unsupported expression type"};
}
