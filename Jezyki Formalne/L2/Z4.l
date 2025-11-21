%{
    #include <iostream>
    #include <fstream>
    #include <stack>
    #include <cmath>

    using namespace std;

    stack<int> Stos;
    int wyn=0;

    void licz(const char* symbol){
        switch(symbol[0]){
            case '+':
            if(Stos.size()<2){
                cout<<"Brak argumentow"<<endl;
                BEGIN(PANIC);
                return;
            }else{
                int a = Stos.top();
                Stos.pop();
                int b = Stos.top();
                Stos.pop();
                Stos.push(a+b);
            }
            break;
            case '-':
            if(Stos.size()<2){
                cout<<"Brak argumentow"<<endl;
                return;
            }else{
                int a = Stos.top();
                Stos.pop();
                int b = Stos.top();
                Stos.pop();
                Stos.push(b-a);
            }
            break;
            case '*':
            if(Stos.size()<2){
                cout<<"Brak argumentow"<<endl;
                return;
            }else{
                int a = Stos.top();
                Stos.pop();
                int b = Stos.top();
                Stos.pop();
                Stos.push(a*b);
            }
            break;
            case '/':
            if(Stos.size()<2){
                cout<<"Brak argumentow"<<endl;
                return;
            }else{
                int a = Stos.top();
                Stos.pop();
                int b = Stos.top();
                Stos.pop();
                Stos.push(b/a);
            }
            break;
            case '^':
            if(Stos.size()<2){
                cout<<"Brak argumentow"<<endl;
                return;
            }else{
                int a = Stos.top();
                Stos.pop();
                int b = Stos.top();
                Stos.pop();
                Stos.push(pow(b,a));
            }
            break;
            case '%':
            if(Stos.size()<2){
                cout<<"Brak argumentow"<<endl;
                return;
            }else{
                int a = Stos.top();
                Stos.pop();
                int b = Stos.top();
                Stos.pop();
                Stos.push(b%a);
            }
            break;
            default:
            cout<<"Problem z symbolem"<<endl;
            break;
        }
    }

    int wynik(){
        if(Stos.size()!=1){
            cout<<"Blad koncowy"<<endl;
        }else{
            wyn=Stos.top();
            Stos = stack<int>();
        }

        int temp = wyn;
        wyn = 0;
        return temp;
    }
%}

%option noyywrap c++

liczba [0-9]
symbol [+|\-|*|/|^|%]


%x PANIC

%%
<INITIAL>{
-{0,1}{liczba}+ {
    Stos.push(stoi((string)YYText()));
    yyout<<YYText();
}

{symbol} {
    licz(YYText());
    yyout<<YYText();
}

\n {yyout<<endl<<"= "<<wynik()<<endl;}
}

<PANIC>{
    . ;
    \n BEGIN(INITIAL);
}


<<EOF>>		{
    yyout<<endl<<"= "<<wynik()<<endl;
		yyterminate();
		}


%%

int main( int argc , char** argv)
    {
        ifstream in(argv[1]);
        if (!in) {
        cerr << "Cannot open file\n";
        return 1;
        }   
        ofstream out("Z4output.txt");
        yyFlexLexer lexer(&in,&out);
        lexer.yylex();
        return 0;
    }