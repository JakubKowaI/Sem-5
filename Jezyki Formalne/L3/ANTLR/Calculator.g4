grammar Calculator;

input
    : line* EOF
    ;

line
    : expr NEWLINE            
    | NEWLINE                 
    ;

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
    : MINUS unaryExpr         
    | primary                 
    ;

primary
    : NUMBER                  
    | LPAREN expr RPAREN      
    ;

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