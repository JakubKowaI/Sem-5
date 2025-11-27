grammar Calculator;

// Główny punkt wejścia
input
    : line* EOF
    ;

line
    : expr NEWLINE        # printExpr
    | NEWLINE             # blank
    ;

// Reguły parsowania (priorytety są ustalane przez kolejność - im wyżej, tym silniej wiąże)
expr
    : LPAREN expr RPAREN              # parens
    | MINUS NUMBER                    # negativeNumber
    | left=expr HAT right=expr        # power
    | left=expr op=(STAR|SLASH) right=expr  # mulDiv
    | left=expr op=(PLUS|MINUS) right=expr  # addSub
    | NUMBER                          # number
    ;

// Reguły leksera (Tokeny)
PLUS    : '+';
MINUS   : '-';
STAR    : '*';
SLASH   : '/';
HAT     : '^';
LPAREN  : '(';
RPAREN  : ')';

NUMBER  : [0-9]+;

// Obsługa komentarzy (Bison: #.*)
COMMENT : '#' ~[\r\n]* -> skip;

// Białe znaki (Bison: WS)
WS      : [ \t]+ -> skip;

// Koniec linii (Bison: EOL)
NEWLINE : '\r'? '\n';