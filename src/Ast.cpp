#include "Ast.h"

namespace ast {

void NumberExpr::print(std::ostream &os, int indent) const {
  printIndent(os, indent);
  os << "Number(" << value << ")\n";
}

void IdExpr::print(std::ostream &os, int indent) const {
  printIndent(os, indent);
  os << "Id(" << name << ")\n";
}

void CallExpr::print(std::ostream &os, int indent) const {
  printIndent(os, indent);
  os << "CallExpr(" << callee << ")\n";
  for (const auto &arg : args) {
    arg->print(os, indent + 1);
  }
}

void BinaryExpr::print(std::ostream &os, int indent) const {
  printIndent(os, indent);
  os << "BinaryExpr(" << op << ")\n";
  if (left) {
    left->print(os, indent + 1);
  }
  if (right) {
    right->print(os, indent + 1);
  }
}

void PrintStmt::print(std::ostream &os, int indent) const {
  printIndent(os, indent);
  os << "PrintStmt\n";
  if (expr) {
    expr->print(os, indent + 1);
  }
}

void VarDeclStmt::print(std::ostream &os, int indent) const {
  printIndent(os, indent);
  os << "VarDeclStmt(" << typeName << " " << varName << ")\n";
}

void ReturnStmt::print(std::ostream &os, int indent) const {
  printIndent(os, indent);
  os << "ReturnStmt\n";
  if (expr) {
    expr->print(os, indent + 1);
  } else {
    printIndent(os, indent + 1);
    os << "VoidReturn\n";
  }
}

void CallStmt::print(std::ostream &os, int indent) const {
  printIndent(os, indent);
  os << "CallStmt\n";
  if (call) {
    call->print(os, indent + 1);
  }
}

void FunctionDecl::print(std::ostream &os, int indent) const {
  printIndent(os, indent);
  os << "FunctionDecl(" << returnType << " " << name << ")\n";
  printIndent(os, indent + 1);
  os << "Params\n";
  for (const auto &param : params) {
    printIndent(os, indent + 2);
    os << param.typeName << " " << param.name << "\n";
  }
  printIndent(os, indent + 1);
  os << "Body\n";
  for (const auto &stmt : body) {
    stmt->print(os, indent + 2);
  }
}

void Program::print(std::ostream &os, int indent) const {
  printIndent(os, indent);
  os << "Program\n";
  printIndent(os, indent + 1);
  os << "Functions\n";
  for (const auto &fn : functions) {
    fn->print(os, indent + 2);
  }
  printIndent(os, indent + 1);
  os << "Calls\n";
  for (const auto &call : calls) {
    call->print(os, indent + 2);
  }
}

}