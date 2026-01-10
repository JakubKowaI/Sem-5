%require "3.2"
%language "c++"
%defines "parser.hpp"
%define api.value.type variant
%define api.token.constructor

%code requires {
  #include <iostream>
  #include <string>
  #include <vector>
  #include <cmath>
  #include <cstdio>
}

%code {
  namespace yy {
      parser::symbol_type yylex();
  }
  int GF=1234577;
  int k=0;

  unsigned long mod(unsigned long left, unsigned long right){
    return left % right
  }

  int mod2(long long x){
    long long m = GF - 1;
    x %= m;
    if (x < 0) x += m;
    return x;
  }
  
  int power(long long b,long long e){
    //std::cout<<std::endl<<e<<" - "<<std::endl;
    e=mod2(e);
    int c=1;
    for(int i=0;i<e;i++){
      c=(c*b)%GF;
    }
    return c;
  }

    int power2(long long b,long long e){
    //std::cout<<std::endl<<e<<" - "<<std::endl;
    e=mod2(e);
    int c=1;
    for(int i=0;i<e;i++){
      c=(c*b)%(GF-1);
    }
    return c;
  }

  int div(long long a, long long b) {
    b = mod(b);
    if (b == 0) { std::cout<<"Dzielienie przez 0!!!"<<std::endl; return 0; }
    return (a * power(b,GF-2))%GF;
  }

  int div2(long long a, long long b) {
    b = mod2(b);
    if (b == 0) { std::cout<<"Dzielienie przez 0!!!"<<std::endl; return 0; }
    return (a * power2(b,GF-3))%(GF-1);
  }
}

%nterm program_all
%nterm procedures
%nterm main
%nterm commands
%nterm proc_head
%nterm proc_call
%nterm declarations
%nterm args_decl
%nterm type
%nterm args
%nterm <unsigned long> expression
%nterm <bool> condition
%nterm value
%nterm <unsigned long> identifier


%token EOL "end of line"

%token <unsigned long> NUM
%token <string> PIDENTIFIER

%token PLUS "+"
%token MINUS "-"
%token STAR "*"
%token SLASH "/"
%token MOD "%"

%token EQUALS "="
%token NE "!="
%token GT ">"
%token LT "<"
%token GE ">="
%token LE "<="

%token LPAREN "("
%token RPAREN ")"
%token LSQUARE "["
%token RSQUARE "]"
%token COMMA ","

%token PROCEDURE
%token IS
%token IN
%token END

%token PROGRAM

%token SET ":="
%token SEMICOLON ";"
%token IF
%token THEN
%token ELSE
%token ENDIF
%token WHILE
%token DO
%token ENDWHILE
%token REPEAT
%token UNTIL
%token FOR
%token FROM
%token TO
%token ENDFOR
%token DOWNTO
%token READ
%token WRITE

%token COLON ":"

%token T
%token I
%token O


/* %left MINUS PLUS
%left STAR SLASH
%precedence NEG
%right HAT */

%%

input:
%empty
| input line
;

line:
EOL
| error EOL {std::cerr<<"Error"<<std::endl;yyerrok;}
;

program_all:
procedures main
;

procedures:
procedures PROCEDURE proc_head IS declarations IN commands END {}
| procedures PROCEDURE proc_head IS IN commands END {}
;

main:
PROGRAM IS declarations IN commands END {}
| PROGRAM IS IN commands END {}
;

commands:
commands command {}
| commands {}
;

command:
identifier SET expression SEMICOLON {}
| IF condition THEN commands ELSE commands ENDIF {}
| IF condition THEN commands ENDIF {}
| WHILE condition DO commands ENDWHILE {}
| REPEAT commands UNTIL condition SEMICOLON {}
| FOR PIDENTIFIER FROM value TO value DO commands ENDFOR {}
| FOR PIDENTIFIER FROM value DOWNTO value DO commands ENDFOR {}
| proc_call SEMICOLON {}
| READ identifier SEMICOLON {}
| WRITE value SEMICOLON {}
;

proc_head:
PIDENTIFIER LPAREN args_decl RPAREN {}
;

proc_call:
PIDENTIFIER LPAREN args RPAREN {}
;

declarations:
declarations COMMA PIDENTIFIER {}
| declarations COMMA PIDENTIFIER LSQUARE NUM COLON NUM RSQUARE {}
| PIDENTIFIER {}
| PIDENTIFIER LSQUARE NUM COLON NUM RSQUARE {}
;

args_decl:
args_decl COMMA type PIDENTIFIER {}
| type PIDENTIFIER {}
;

type:
T {}
| I {}
| O {}
;

args:
args COMMA PIDENTIFIER {}
| PIDENTIFIER {}
;

expression:
value {}
| value PLUS value {}
| value MINUS value {}
| value STAR value {}
| value SLASH value {}
| value MOD value {}
;

condition:
value EQUALS value {}
| value NE value {}
| value GT value {}
| value LT value {}
| value GE value {}
| value LE value {}
;

value:
NUM {}
| identifier {}
;

identifier:
PIDENTIFIER {}
| PIDENTIFIER LSQUARE PIDENTIFIER RSQUARE {}
| PIDENTIFIER LSQUARE NUM RSQUARE {}
;

//std::cout<<"\n Pojawił się błąd: "<<yyerrok<<std::endl;
/* EXPEXPR:
PRIMARY       {$$=$1;std::cout<<mod2($1)<<" ";}
| exp PLUS exp { $$ = mod2($1 + $3); std::cout<<'+'<<" "; }
| exp MINUS exp { $$ = mod2($1 - $3); std::cout<<'-'<<" "; }
| exp STAR exp { $$ = mod2($1 * $3); std::cout<<'*'<<" "; }
| exp SLASH exp { $$ = div2($1, $3); std::cout<<'/'<<" "; } */
/* | MINUS exp %prec NEG { $$ = mod($2); }  */
/* | exp HAT EXPEXPR { $$ = mod2(power2($1, $3)); std::cout<<'^'<<" "; }
| LPAREN exp RPAREN { $$ = $2; }
; */

/* 
exp: */
/* NUMBER          { $$ = mod($1); std::cout<<mod($1)<<" "; } */
/* | exp HAT MINUS exp {$$ = mod(power($1, mod($4*-1))); std::cout<<'^'<<"t "; } */
/* | MINUS NUMBER %prec NEG {$$ = mod(-$2); std::cout<<mod(-$2)<<" "; } */
/* PRIMARY       {$$=$1;std::cout<<mod($1)<<" ";}
| exp PLUS exp { $$ = mod($1 + $3); std::cout<<'+'<<" "; }
| exp MINUS exp { $$ = mod($1 - $3); std::cout<<'-'<<" "; }
| exp STAR exp { $$ = mod($1 * $3); std::cout<<'*'<<" "; }
| exp SLASH exp { $$ = div($1, $3); std::cout<<'/'<<" "; } */
/* | MINUS exp %prec NEG { $$ = mod($2); }  */
/* | exp HAT EXPEXPR { $$ = mod(power($1, $3)); std::cout<<'^'<<" "; }
| LPAREN exp RPAREN { $$ = $2; }
; */

/* PRIMARY:
  NUMBER { $$ = $1;  }
| MINUS PRIMARY %prec NEG {$$ = -$2;  }
; */
%%

namespace yy {
  void parser::error (const std::string& msg) {
    std::cerr << "Error: " << msg << '\n';
  }
}

int main () {
  yy::parser parse;
  return parse ();
}