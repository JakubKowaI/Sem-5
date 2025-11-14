%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

static FILE *out;

int pustaLinia=1, pierwszyZnak=0;
int wc = 0, lc = 0;
int yywrap() { return 1; }
%}

ws    [ \t]
nl    [ \n]
nonws [^ \t\n]

%%



{nonws}+	{++wc; pierwszyZnak=1;pustaLinia=0;fprintf(out,yytext);}

{ws}+\n     {
                +lc;if(pustaLinia==0){
                    fprintf(out,"\n");
                }
                pustaLinia=1;
                pierwszyZnak=0;
            }

{ws}+		{
                if(pierwszyZnak==1)
                    fprintf(out, " ");
            }

\n		    {++lc;if(pustaLinia==0){
                fprintf(out,"\n");
            }
            pustaLinia=1;
            pierwszyZnak=0;
            }

<<EOF>>		{
		fprintf(out, "\n%8d %8d \n", lc, wc );
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

    out = fopen("outputZ1.txt", "w");
    if (!out) {
        fprintf(stderr, "Nie moge otworzyc wyjscia\n");
        return 1;
    }


    yylex();
    fclose(out);
}