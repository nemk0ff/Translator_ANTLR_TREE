parser grammar MiniLangParser;

options { tokenVocab = MiniLangLexer; }

program
    : functionDecl+ callStmt+ EOF
    ;

functionDecl
    : returnType ID LPAREN paramList? RPAREN LBRACE stmt* RBRACE
    ;

returnType
    : VOID
    | INT
    ;

paramList
    : param (COMMA param)*
    ;

param
    : INT ID
    ;

stmt
    : printStmt
    | varDeclStmt
    | returnStmt
    | callStmt
    ;

printStmt
    : PRINT LPAREN expr RPAREN SEMI
    ;

varDeclStmt
    : INT ID SEMI
    ;

returnStmt
    : RETURN expr? SEMI
    ;

callStmt
    : functionCall SEMI
    ;

functionCall
    : ID LPAREN argList? RPAREN
    ;

argList
    : expr (COMMA expr)*
    ;

expr
    : additiveExpr
    ;

additiveExpr
    : primaryExpr ((PLUS | MINUS) primaryExpr)*
    ;

primaryExpr
    : functionCall
    | NUM
    | ID
    | LPAREN expr RPAREN
    ;
