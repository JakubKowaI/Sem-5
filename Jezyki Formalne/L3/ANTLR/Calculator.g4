grammar Calculator;

input
    : line* EOF
    ;

line
    : expr NEWLINE            # printExpr
    | NEWLINE                 # blank
    ;

// expr -> add -> mul -> pow -> unary -> primary (priorytet)
expr
    : addExpr
    ;

addExpr
    : mulExpr (op=(PLUS | MINUS) right=mulExpr)*
    ;

mulExpr
    : powExpr (op=(STAR | SLASH) right=powExpr)*
    ;

powExpr
    : left=unaryExpr (HAT right=powExpr)?
    ;

unaryExpr
    : MINUS unaryExpr         # unaryMinus
    | primary                 # primaryExpr
    ;

primary
    : NUMBER                  # number
    | LPAREN expr RPAREN      # parens
    ;

// lexer
PLUS    : '+';
MINUS   : '-';
STAR    : '*';
SLASH   : '/';
HAT     : '^';
LPAREN  : '(';
RPAREN  : ')';

NUMBER  : [0-9]+ ;

COMMENT : '#' ~[\r\n]* -> skip ;
WS      : [ \t\r]+ -> skip ;
NEWLINE : '\r'? '\n' ;