#pragma once

#include <iostream>

#include "antlr4-runtime.h"

class SyntaxErrorListener : public antlr4::BaseErrorListener {
 public:
  int errors = 0;

  void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const std::string &msg,
                   std::exception_ptr e) override {
    // Эти параметры тут не нужны, но сигнатуру метода менять нельзя
    (void)recognizer;
    (void)offendingSymbol;
    (void)e;
    ++errors;
    std::cerr << "Syntax error at " << line << ":" << charPositionInLine << " - " << msg << '\n';
  }
};
