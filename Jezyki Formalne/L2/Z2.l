%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

static FILE *out;

int temp = 0;
%}

%x STRING
%x LONGSTRING
%x COMMENT

%%
<INITIAL>{
[ \t]*#.* ;

\"\"\" BEGIN(COMMENT);
\'\'\' BEGIN(COMMENT);

\"{1} {
        fprintf(out,yytext);
        BEGIN(STRING);
    }
\'{1} BEGIN(LONGSTRING);



.    {if(temp==0){
            fprintf(out,yytext);
            }
        }

[ \n]   {
    if(temp==0){
        fprintf(out,"\n");
    }
    }

}

<COMMENT>{
    \"\"\" BEGIN(INITIAL);
    .|\n {}
}

<STRING>{
    \" {
        fprintf(out,yytext);
        BEGIN(INITIAL);
    }
    \\\" fprintf(out,yytext);
    .|\n fprintf(out,yytext);
}

<LONGSTRING>{
    \'{1} BEGIN(INITIAL);
    .|\n fprintf(out,yytext);
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

    yyin = fopen(names[1],"r");
    if(yyin == NULL){
      fprintf(stderr,"Nie moge otworzyc %s\n",
              names[1]);
      yyterminate();
    }

    out = fopen("outputZ2.txt", "w");
    if (!out) {
        fprintf(stderr, "Nie moge otworzyc wyjscia\n");
        return 1;
    }


    yylex();
    fclose(out);
}