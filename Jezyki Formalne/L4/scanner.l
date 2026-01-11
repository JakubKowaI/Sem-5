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


NUM   [0-9]
WS      [ \t\r]+
PIDENTIFIER [_a-zA-Z]

%%

\#.*                      { BEGIN(INITIAL); }
\\{WS}*"\n"               { }
\n                        { return yy::parser::make_EOL(); }

{WS}                      { }

"+"                       { return yy::parser::make_PLUS(); }
"-"                       { return yy::parser::make_MINUS(); }
"*"                       { return yy::parser::make_STAR(); }
"/"                       { return yy::parser::make_SLASH(); }
"%"                       { return yy::parser::make_MOD(); }


"="                       { return yy::parser::make_EQUALS(); }
"!="                      { return yy::parser::make_NE(); }
">"                       { return yy::parser::make_GT(); }
"<"                       { return yy::parser::make_LT(); }
">="                      { return yy::parser::make_GE(); }
"<="                      { return yy::parser::make_LE(); }


"("                       { return yy::parser::make_LPAREN(); }
")"                       { return yy::parser::make_RPAREN(); }
"["                       { return yy::parser::make_LSQUARE(); }
"]"                       { return yy::parser::make_RSQUARE(); }
","                       { return yy::parser::make_COMMA(); }


"PROCEDURE"               { return yy::parser::make_PROCEDURE();}
"IS"                      { return yy::parser::make_IS();}
"IN"                      { return yy::parser::make_IN();}
"END"                     { return yy::parser::make_END();}


"PROGRAM"                 { return yy::parser::make_PROGRAM();}


":="                      { return yy::parser::make_SET();}
";"                       { return yy::parser::make_SEMICOLON();}
"IF"                      { return yy::parser::make_IF();}
"THEN"                    { return yy::parser::make_THEN();}
"ELSE"                    { return yy::parser::make_ELSE();}
"ENDIF"                   { return yy::parser::make_ENDIF();}
"WHILE"                   { return yy::parser::make_WHILE();}
"DO"                      { return yy::parser::make_DO();}
"ENDWHILE"                { return yy::parser::make_ENDWHILE();}
"REPEAT"                  { return yy::parser::make_REPEAT();}
"UNTIL"                   { return yy::parser::make_UNTIL();}
"FOR"                     { return yy::parser::make_FOR();}
"FROM"                    { return yy::parser::make_FROM();}
"TO"                      { return yy::parser::make_TO();}
"ENDFOR"                  { return yy::parser::make_ENDFOR();}
"DOWNTO"                  { return yy::parser::make_DOWNTO();}
"READ"                    { return yy::parser::make_READ();}
"WRITE"                   { return yy::parser::make_WRITE();}


":"                       { return yy::parser::make_COLON();}


"T"                       { return yy::parser::make_T(); }
"I"                       { return yy::parser::make_I(); }
"O"                       { return yy::parser::make_O(); }


{NUM}+                    { return yy::parser::make_NUM(std::stoul(yytext)); }
{PIDENTIFIER}+            { return yy::parser::make_PIDENTIFIER(yytext);}

. {
  std::cerr << "Nieznany znak: " << yytext << std::endl;
}

<<EOF>> {
  return yy::parser::make_YYEOF();
}

%%