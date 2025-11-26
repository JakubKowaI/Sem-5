%require "3.2"
%language "c++"
// Zapisz nagłówek do pliku, aby Flex mógł go dołączyć
%defines "parser.hpp"
%define api.value.type variant
%define api.token.constructor

%code requires {
  #include <iostream>
  #include <string>
  #include <vector>
  #include <cmath>  // dla pow
  #include <cstdio> // dla printf
}

%code {
  // Deklaracja yylex dla parsera (implementacja będzie we Flexie)
  namespace yy {
      parser::symbol_type yylex();
  }
  int GF=1234577;

  int mod(int x){
    if(x<0)return (GF+(x%GF))%GF;
    else return x%GF;
  }

}
// Definicja typów semantycznych
%token <int> NUMBER
%nterm <int> exp

%token PLUS "+"
%token MINUS "-"
%token STAR "*"
%token SLASH "/"
%token LPAREN "("
%token RPAREN ")"
%token HAT "^"
%token EOL "end of line"

%left MINUS PLUS
%left STAR SLASH
%precedence NEG
%right HAT

%%

input:
%empty
| input line
;

line:
EOL
| exp EOL { std::cout << "\nWynik: " << $1 << std::endl; }
;

exp:
NUMBER          { $$ = $1; std::cout<<$1<<" "; }
| MINUS NUMBER %prec NEG {$$ = mod(-$2); std::cout<<mod(-$2)<<" "; }
| exp PLUS exp { $$ = mod($1 + $3); std::cout<<'+'<<" "; }
| exp MINUS exp { $$ = mod($1 - $3); std::cout<<'-'<<" "; }
| exp STAR exp { $$ = mod($1 * $3); std::cout<<'*'<<" "; }
| exp SLASH exp { $$ = mod($1 / $3); std::cout<<'/'<<" "; }
/* | MINUS exp %prec NEG { $$ = (GF-$2)%GF; } */
| exp HAT exp { $$ = mod(pow($1, $3)); std::cout<<'^'<<" "; }
| LPAREN exp RPAREN { $$ = $2; }
;
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