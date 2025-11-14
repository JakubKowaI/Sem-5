%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

static FILE *out;

int opcja=0;
int temp=0;
int zapis=0;
%}

%%

"/\\*"   { temp = 1; printf("Wykryto: %s\n",yytext);}


"/""*""*"[^ "*""/"]*"*""/" { if(opcja==1){
                    fprintf(out,yytext);
                }
                printf("Wykryto1: %s\n",yytext);
                }

"/""*""!"[^ "*""/"]*"*""/" { if(opcja==1){
                    fprintf(out,yytext);
                }
                printf("Wykryto: %s\n",yytext);
                }

.*"*""/"  {
     temp = 0; 
     if(zapis==1){
        fprintf(out,yytext);
        zapis=0;
     }
     printf("Wykryto: %s\n",yytext);
     }

[ \t]*"///".* { if(opcja==1){
            fprintf(out,yytext);
        }
        printf("Wykryto: %s\n",yytext);
        }

[ \t]*"//!".* { if(opcja==1){
            fprintf(out,yytext);
        }
        printf("Wykryto: %s\n",yytext);
        }

[ \t]*"//".* { }

"/**".* {
        if(opcja==0){
            temp = 1;
        }else{
            zapis=1;
            fprintf(out,yytext);
        }
        printf("Wykryto: %s\n",yytext);
        }

"/*!".* {
        if(opcja==0){
            temp = 1;
        }else{
            zapis=1;
            fprintf(out,yytext);
        }
        printf("Wykryto: %s\n",yytext);
        }






.*      {if(temp==0){
            fprintf(out,yytext);
            }
            printf("Przerabiam: %s\n",yytext);
        }

[\n]   {
    if(temp==0){
        fprintf(out,"\n");
    }
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