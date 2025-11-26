%{
  #include <iostream>
  #include <string>
  #include "parser.hpp"

  namespace yy {
    parser::symbol_type yylex();
  }

  #undef  YY_DECL
  #define YY_DECL yy::parser::symbol_type yy::yylex()
%}

%option noyywrap nounput noinput debug


DIGIT   [0-9]
WS      [ \t\r]+

%%

\#.*                      { BEGIN(INITIAL); }
\\{WS}*"\n"               { }
\n                        { return yy::parser::make_EOL(); }

{WS}                      { }

"+"                       { return yy::parser::make_PLUS(); }
"-"                       { return yy::parser::make_MINUS(); }
"*"                       { return yy::parser::make_STAR(); }
"/"                       { return yy::parser::make_SLASH(); }
"^"                       { return yy::parser::make_HAT(); }
"("                       { return yy::parser::make_LPAREN(); }
")"                       { return yy::parser::make_RPAREN(); }

{DIGIT}+                  { return yy::parser::make_NUMBER(std::stoi(yytext)); }

. {
  std::cerr << "Nieznany znak: " << yytext << std::endl;
}

<<EOF>> {
  return yy::parser::make_YYEOF();
}

%%