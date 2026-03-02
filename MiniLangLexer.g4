lexer grammar MiniLangLexer;

VOID: 'void';
INT: 'int';
PRINT: 'print';
RETURN: 'return';

LPAREN: '(';
RPAREN: ')';
LBRACE: '{';
RBRACE: '}';
COMMA: ',';
SEMI: ';';
PLUS: '+';
MINUS: '-';

ID: [a-zA-Z_][a-zA-Z0-9_]*;
NUM: [0-9]+;

LINE_COMMENT: '//' ~[\r\n]* -> skip;
BLOCK_COMMENT: '/*' .*? '*/' -> skip;
WS: [ \t\r\n]+ -> skip;
