#include "AstBuilderVisitor.h"

#include <stdexcept>

std::unique_ptr<ast::Program> AstBuilderVisitor::buildProgram(MiniLangParser::ProgramContext *ctx) {
  auto program = std::make_unique<ast::Program>();

  // Сначала собираем все объявления функций
  for (auto *fnCtx : ctx->functionDecl()) {
    program->functions.push_back(buildFunctionDecl(fnCtx));
  }

  // Потом верхнеуровневые вызовы, как в грамматике program
  for (auto *callCtx : ctx->callStmt()) {
    program->calls.push_back(buildCallStmt(callCtx));
  }

  return program;
}

std::unique_ptr<ast::FunctionDecl> AstBuilderVisitor::buildFunctionDecl(
    MiniLangParser::FunctionDeclContext *ctx) {
  auto fn = std::make_unique<ast::FunctionDecl>();
  fn->returnType = ctx->returnType()->getText();
  fn->name = ctx->ID()->getText();

  if (ctx->paramList() != nullptr) {
    for (auto *paramCtx : ctx->paramList()->param()) {
      ast::Param p;
      p.typeName = paramCtx->INT()->getText();
      p.name = paramCtx->ID()->getText();
      fn->params.push_back(std::move(p));
    }
  }

  for (auto *stmtCtx : ctx->stmt()) {
    fn->body.push_back(buildStmt(stmtCtx));
  }

  return fn;
}

std::unique_ptr<ast::Stmt> AstBuilderVisitor::buildStmt(MiniLangParser::StmtContext *ctx) {
  // Тут обычный диспетчер: смотрим, какая ветка stmt реально сработала
  if (ctx->printStmt() != nullptr) {
    auto derived = buildPrintStmt(ctx->printStmt());
    std::unique_ptr<ast::Stmt> base = std::move(derived);
    return base;
  }
  if (ctx->varDeclStmt() != nullptr) {
    auto derived = buildVarDeclStmt(ctx->varDeclStmt());
    std::unique_ptr<ast::Stmt> base = std::move(derived);
    return base;
  }
  if (ctx->returnStmt() != nullptr) {
    auto derived = buildReturnStmt(ctx->returnStmt());
    std::unique_ptr<ast::Stmt> base = std::move(derived);
    return base;
  }
  if (ctx->callStmt() != nullptr) {
    auto derived = buildCallStmt(ctx->callStmt());
    std::unique_ptr<ast::Stmt> base = std::move(derived);
    return base;
  }

  throw std::runtime_error("Unsupported statement");
}

std::unique_ptr<ast::PrintStmt> AstBuilderVisitor::buildPrintStmt(
    MiniLangParser::PrintStmtContext *ctx) {
  auto st = std::make_unique<ast::PrintStmt>();
  st->expr = buildExpr(ctx->expr());
  return st;
}

std::unique_ptr<ast::VarDeclStmt> AstBuilderVisitor::buildVarDeclStmt(
    MiniLangParser::VarDeclStmtContext *ctx) {
  auto st = std::make_unique<ast::VarDeclStmt>();
  st->typeName = ctx->INT()->getText();
  st->varName = ctx->ID()->getText();
  return st;
}

std::unique_ptr<ast::ReturnStmt> AstBuilderVisitor::buildReturnStmt(
    MiniLangParser::ReturnStmtContext *ctx) {
  auto st = std::make_unique<ast::ReturnStmt>();
  if (ctx->expr() != nullptr) {
    st->expr = buildExpr(ctx->expr());
  }
  return st;
}

std::unique_ptr<ast::CallStmt> AstBuilderVisitor::buildCallStmt(MiniLangParser::CallStmtContext *ctx) {
  auto st = std::make_unique<ast::CallStmt>();
  st->call = buildFunctionCall(ctx->functionCall());
  return st;
}

std::unique_ptr<ast::CallExpr> AstBuilderVisitor::buildFunctionCall(
    MiniLangParser::FunctionCallContext *ctx) {
  auto call = std::make_unique<ast::CallExpr>();
  call->callee = ctx->ID()->getText();

  if (ctx->argList() != nullptr) {
    for (auto *exprCtx : ctx->argList()->expr()) {
      call->args.push_back(buildExpr(exprCtx));
    }
  }

  return call;
}

std::unique_ptr<ast::Expr> AstBuilderVisitor::buildExpr(MiniLangParser::ExprContext *ctx) {
  return buildAdditiveExpr(ctx->additiveExpr());
}

std::unique_ptr<ast::Expr> AstBuilderVisitor::buildAdditiveExpr(MiniLangParser::AdditiveExprContext *ctx) {
  auto current = buildPrimaryExpr(ctx->primaryExpr(0));

  // Склеиваем левую цепочку a+b-c в бинарные узлы слева направо
  const size_t count = ctx->primaryExpr().size();
  for (size_t i = 1; i < count; ++i) {
    auto right = buildPrimaryExpr(ctx->primaryExpr(i));
    auto binary = std::make_unique<ast::BinaryExpr>();
    binary->left = std::move(current);
    binary->right = std::move(right);
    binary->op = ctx->children[2 * i - 1]->getText();
    current = std::move(binary);
  }

  return current;
}

std::unique_ptr<ast::Expr> AstBuilderVisitor::buildPrimaryExpr(MiniLangParser::PrimaryExprContext *ctx) {
  // Приоритет такой же, как в grammar: call, num, id, (expr)
  if (ctx->functionCall() != nullptr) {
    auto call = buildFunctionCall(ctx->functionCall());
    std::unique_ptr<ast::Expr> asExpr = std::move(call);
    return asExpr;
  }

  if (ctx->NUM() != nullptr) {
    return std::make_unique<ast::NumberExpr>(ctx->NUM()->getText());
  }

  if (ctx->ID() != nullptr) {
    return std::make_unique<ast::IdExpr>(ctx->ID()->getText());
  }

  if (ctx->expr() != nullptr) {
    return buildExpr(ctx->expr());
  }

  throw std::runtime_error("Unsupported primary expression");
}
