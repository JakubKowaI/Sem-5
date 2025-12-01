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
  #include <cmath>
  #include <cstdio>
}

%code {
  // Deklaracja yylex dla parsera (implementacja będzie we Flexie)
  namespace yy {
      parser::symbol_type yylex();
  }
  int GF=1234577;

  int mod(long long x){
    if(x<0)return (GF+(x%GF))%GF;
    else return x%GF;
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

  int div(long long a, long long b) {
    b = mod(b);
    if (b == 0) { std::cout<<"Dzielienie przez 0!!!"<<std::endl; return 0; }
    return (a * power(b,GF-2))%GF;
  }
}
// Definicja typów semantycznych
%token <long long> NUMBER
%nterm <long long> exp
%nterm <long long> PRIMARY

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
| exp EOL { std::cout << "\nWynik: " << mod($1) << std::endl; }
| error EOL {std::cout<<std::endl;yyerrok;}
;

//std::cout<<"\n Pojawił się błąd: "<<yyerrok<<std::endl;

exp:
/* NUMBER          { $$ = mod($1); std::cout<<mod($1)<<" "; } */
/* | exp HAT MINUS exp {$$ = mod(power($1, mod($4*-1))); std::cout<<'^'<<"t "; } */
/* | MINUS NUMBER %prec NEG {$$ = mod(-$2); std::cout<<mod(-$2)<<" "; } */
PRIMARY       {$$=$1;std::cout<<mod($1)<<" ";}
| exp PLUS exp { $$ = mod($1 + $3); std::cout<<'+'<<" "; }
| exp MINUS exp { $$ = mod($1 - $3); std::cout<<'-'<<" "; }
| exp STAR exp { $$ = mod($1 * $3); std::cout<<'*'<<" "; }
| exp SLASH exp { $$ = div($1, $3); std::cout<<'/'<<" "; }
/* | MINUS exp %prec NEG { $$ = mod($2); }  */
| exp HAT exp { $$ = mod(power($1, $3)); std::cout<<'^'<<" "; }
| LPAREN exp RPAREN { $$ = $2; }
;

PRIMARY:
  NUMBER { $$ = mod($1);  }
| MINUS PRIMARY %prec NEG {$$ = -$2;  }//std::cout<<mod(-$2)<<" ";
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