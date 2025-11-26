%option noyywrap
%{
#include "calc.tab.h"
#include <string.h>
%}

DIGIT   [0-9]
WS      [ \t\r]+

%%
"#".*                      { BEGIN(INITIAL); }
\\{WS}*"\n"                { }
\n                         { return EOL; }
{WS}                      { }
"+"                       { return '+'; }
"-"                       { return '-'; }
"*"                       { return '*'; }
"/"                       { return '/'; }
"^"                       { return '^'; }
"("                       { return '('; }
")"                       { return ')'; }
{DIGIT}+                  {
                             yylval.ival = strtoull(yytext, NULL, 10);
                             return NUMBER;
                          }
.                         { fprintf(stderr, "Lex error: nieznany znak '%s'\n", yytext); }
%%
