BUILD_DIR = build
SRC_DIR = src
GEN_DIR = generated
TEST_DIR = tests

GRAMMARS = MiniLangLexer.g4 MiniLangParser.g4
ANTLR4 ?= antlr4
ANTLR4_JAR ?= /opt/antlr/antlr-4.13.1-complete.jar

RUNTIME_INCLUDE ?= /usr/local/include/antlr4-runtime
RUNTIME_LIBDIR ?= /usr/local/lib

CXX = g++
CXX_FLAGS = -std=c++17 -Wall -Wextra -I$(RUNTIME_INCLUDE) -I. -I$(SRC_DIR) -I$(GEN_DIR)
CXX_ANTLR4_DYNAMIC_LIB = -lantlr4-runtime
LINK_FLAGS = -L$(RUNTIME_LIBDIR) $(CXX_ANTLR4_DYNAMIC_LIB)

TARGET = $(BUILD_DIR)/minilang

SRC_OBJS = \
	$(BUILD_DIR)/Ast.o \
	$(BUILD_DIR)/AstBuilderVisitor.o \
	$(BUILD_DIR)/Interpreter.o \
	$(BUILD_DIR)/main.o

GEN_OBJS = \
	$(BUILD_DIR)/MiniLangLexer.o \
	$(BUILD_DIR)/MiniLangParser.o \
	$(BUILD_DIR)/MiniLangParserBaseVisitor.o \
	$(BUILD_DIR)/MiniLangParserVisitor.o

.PHONY: all grammar src generate link test clean docker-build docker-test

all: grammar src generate link

grammar: $(BUILD_DIR)/grammar.stamp

$(BUILD_DIR)/grammar.stamp: $(GRAMMARS)
	mkdir -p "$(BUILD_DIR)"
	if [ -f "$(ANTLR4_JAR)" ]; then java -jar "$(ANTLR4_JAR)" -Dlanguage=Cpp -visitor -no-listener MiniLangLexer.g4 MiniLangParser.g4 -o $(GEN_DIR); else $(ANTLR4) -Dlanguage=Cpp -visitor -no-listener MiniLangLexer.g4 MiniLangParser.g4 -o $(GEN_DIR); fi
	touch "$(BUILD_DIR)/grammar.stamp"

src: $(SRC_OBJS)

$(BUILD_DIR)/Ast.o: $(SRC_DIR)/Ast.cpp $(SRC_DIR)/Ast.h
	mkdir -p "$(BUILD_DIR)"
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/Ast.cpp -o $(BUILD_DIR)/Ast.o

$(BUILD_DIR)/AstBuilderVisitor.o: $(SRC_DIR)/AstBuilderVisitor.cpp $(SRC_DIR)/AstBuilderVisitor.h $(SRC_DIR)/Ast.h
	mkdir -p "$(BUILD_DIR)"
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/AstBuilderVisitor.cpp -o $(BUILD_DIR)/AstBuilderVisitor.o

$(BUILD_DIR)/Interpreter.o: $(SRC_DIR)/Interpreter.cpp $(SRC_DIR)/Interpreter.h $(SRC_DIR)/Ast.h
	mkdir -p "$(BUILD_DIR)"
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/Interpreter.cpp -o $(BUILD_DIR)/Interpreter.o

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/AstBuilderVisitor.h $(SRC_DIR)/SyntaxErrorListener.h
	mkdir -p "$(BUILD_DIR)"
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

generate: grammar $(GEN_OBJS)

$(BUILD_DIR)/MiniLangLexer.o: $(GEN_DIR)/MiniLangLexer.cpp
	$(CXX) $(CXX_FLAGS) -c $(GEN_DIR)/MiniLangLexer.cpp -o $(BUILD_DIR)/MiniLangLexer.o

$(BUILD_DIR)/MiniLangParser.o: $(GEN_DIR)/MiniLangParser.cpp
	$(CXX) $(CXX_FLAGS) -c $(GEN_DIR)/MiniLangParser.cpp -o $(BUILD_DIR)/MiniLangParser.o

$(BUILD_DIR)/MiniLangParserBaseVisitor.o: $(GEN_DIR)/MiniLangParserBaseVisitor.cpp
	$(CXX) $(CXX_FLAGS) -c $(GEN_DIR)/MiniLangParserBaseVisitor.cpp -o $(BUILD_DIR)/MiniLangParserBaseVisitor.o

$(BUILD_DIR)/MiniLangParserVisitor.o: $(GEN_DIR)/MiniLangParserVisitor.cpp
	$(CXX) $(CXX_FLAGS) -c $(GEN_DIR)/MiniLangParserVisitor.cpp -o $(BUILD_DIR)/MiniLangParserVisitor.o

link: $(TARGET)

$(TARGET): src generate
	$(CXX) $(BUILD_DIR)/*.o $(LINK_FLAGS) -o $(TARGET)

test: all
	$(TARGET) $(TEST_DIR)/positive_basic.txt
	$(TARGET) $(TEST_DIR)/positive_nested_calls.txt
	$(TARGET) $(TEST_DIR)/positive_arithmetic.txt
	$(TARGET) $(TEST_DIR)/positive_var_decl.txt
	-$(TARGET) $(TEST_DIR)/negative_missing_semicolon.txt
	-$(TARGET) $(TEST_DIR)/negative_bad_token.txt
	-$(TARGET) $(TEST_DIR)/negative_undefined_function.txt
	-$(TARGET) $(TEST_DIR)/negative_wrong_argc.txt

docker-build:
	docker build -t minilang-lab6 .

docker-test: docker-build
	docker run --rm minilang-lab6 make test

clean:
	rm -rf "$(BUILD_DIR)" "$(GEN_DIR)"