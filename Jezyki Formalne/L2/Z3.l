%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

static FILE *out;

int opcja=0;
int temp=0;
int zapis=0;
%}

%x STRING
%x COMMENT
%x DOC
%x LINECOM

%%
<INITIAL>{

\/\* {BEGIN(COMMENT);}

\" {
    fprintf(out,yytext);
    BEGIN(STRING);
    }

"///".* { if(opcja==1){
            fprintf(out,yytext);
        }
        }

"//!".* { if(opcja==1){
            fprintf(out,yytext);
        }
        }

"//" {BEGIN(LINECOM);}

"/**" {
        if(opcja==0){
            BEGIN(COMMENT);
        }else{
            BEGIN(DOC);
        }
        }

"/*!" {
        if(opcja==0){
            BEGIN(COMMENT);
        }else{
            BEGIN(DOC);
        }
        }

\/\*\*\/ {}

.|\n    fprintf(out,yytext);
            
}

<COMMENT>{
    \*\/ {BEGIN(INITIAL);}
    .|\n ;
}

<DOC>{
    "*/" {BEGIN(INITIAL);}
    .|\n fprintf(out,yytext);
}

<STRING>{
    \\\" fprintf(out,yytext);
    \" {
        fprintf(out,yytext);
        BEGIN(INITIAL);
    }
    .|\n fprintf(out,yytext);
}

<LINECOM>{
    \\[ \t]*\n ;
    \n BEGIN(INITIAL);
    .   ;
}

<<EOF>>		{
		yyterminate();
		}

%%

int main(int argc, char **argv)
{
    if(argc < 2){
       fprintf(stderr,"Cos jest zle \n");
       exit(1);
    }

    char **names = argv;

    if(argc == 3){
        opcja = 1;
    }

    yyin = fopen(names[1],"r");
    if(yyin == NULL){
      fprintf(stderr,"Nie moge otworzyc %s\n",
              names[1]);
      yyterminate();
    }

    out = fopen("outputZ3.txt", "w");
    if (!out) {
        fprintf(stderr, "Nie moge otworzyc wyjscia\n");
        return 1;
    }


    yylex();
    fclose(out);
}