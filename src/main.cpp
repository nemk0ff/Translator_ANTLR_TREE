#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Ast.h"
#include "AstBuilderVisitor.h"
#include "Interpreter.h"
#include "SyntaxErrorListener.h"
#include "antlr4-runtime.h"
#include "generated/MiniLangLexer.h"
#include "generated/MiniLangParser.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: minilang <input-file>\n";
    return 1;
  }

  const std::string path = argv[1];
  std::ifstream input(path);
  if (!input.is_open()) {
    std::cerr << "Cannot open file: " << path << '\n';
    return 1;
  }

  std::stringstream buffer;
  buffer << input.rdbuf();

  antlr4::ANTLRInputStream stream(buffer.str());
  MiniLangLexer lexer(&stream);
  antlr4::CommonTokenStream tokens(&lexer);
  MiniLangParser parser(&tokens);

  // Меняем стандартные ошибки ANTLR на свой формат, чтобы было читаемо
  SyntaxErrorListener errorListener;
  lexer.removeErrorListeners();
  parser.removeErrorListeners();
  lexer.addErrorListener(&errorListener);
  parser.addErrorListener(&errorListener);

  auto *tree = parser.program();
  if (errorListener.errors > 0) {
    std::cerr << "Parsing failed with " << errorListener.errors << " error(s)\n";
    return 2;
  }

  std::cout << "=== PARSE TREE ===\n";
  std::cout << tree->toStringTree(&parser) << "\n" << std::flush;

  // Поверх parse tree строим более компактный AST
  AstBuilderVisitor builder;
  auto program = builder.buildProgram(tree);

  std::cout << "\n=== AST ===\n";
  program->print(std::cout, 0);

  std::cout << "\n=== OUTPUT ===\n" << std::flush;
  Interpreter interpreter;
  int exitCode = interpreter.execute(*program);
  if (exitCode != 0) {
    return exitCode;
  }

  return 0;
}
