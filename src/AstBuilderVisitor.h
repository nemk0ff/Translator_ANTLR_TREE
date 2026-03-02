#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Ast.h"
#include "generated/MiniLangParser.h"

class AstBuilderVisitor {
 public:
  std::unique_ptr<ast::Program> buildProgram(MiniLangParser::ProgramContext *ctx);

 private:
  std::unique_ptr<ast::FunctionDecl> buildFunctionDecl(MiniLangParser::FunctionDeclContext *ctx);
  std::unique_ptr<ast::Stmt> buildStmt(MiniLangParser::StmtContext *ctx);
  std::unique_ptr<ast::PrintStmt> buildPrintStmt(MiniLangParser::PrintStmtContext *ctx);
  std::unique_ptr<ast::VarDeclStmt> buildVarDeclStmt(MiniLangParser::VarDeclStmtContext *ctx);
  std::unique_ptr<ast::ReturnStmt> buildReturnStmt(MiniLangParser::ReturnStmtContext *ctx);
  std::unique_ptr<ast::CallStmt> buildCallStmt(MiniLangParser::CallStmtContext *ctx);
  std::unique_ptr<ast::CallExpr> buildFunctionCall(MiniLangParser::FunctionCallContext *ctx);
  std::unique_ptr<ast::Expr> buildExpr(MiniLangParser::ExprContext *ctx);
  std::unique_ptr<ast::Expr> buildAdditiveExpr(MiniLangParser::AdditiveExprContext *ctx);
  std::unique_ptr<ast::Expr> buildPrimaryExpr(MiniLangParser::PrimaryExprContext *ctx);
};
