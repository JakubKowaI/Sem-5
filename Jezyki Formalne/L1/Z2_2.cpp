#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

vector<int> KMPTable(string pattern){
    int m =pattern.size();
    vector<int> T(m);
    int j=0;
    int i=1;
    while(i<m){
        if(pattern[i]==pattern[j]){
            j++;
            T[i]=j;
            i++;
        }else{
            if(j!=0){
                j=T[j-1];
            }else{
                T[i]=0;
                i++;
            }
        }
    }

    return T;
}

vector<int> KMP(string text, string pattern){
    int n=text.size();
    int m=pattern.size();
    vector<int> T=KMPTable(pattern);
    vector<int> wynik;
    int i=0;
    int j=0;

    while(i<n){
        if(pattern[j]==text[i]){
            i++;
            j++;
            if(j==m){
                wynik.push_back(i - j);
                j=T[j-1];
            }else{
                if(i<n&&pattern[j]!=text[i]){
                    if(j!=0){
                        j=T[j-1];
                    }else{
                        i++;
                    }
                }
            }
        }else{
            i++;
        }
    }
    return wynik;
}

int main(int argc, char* argv[]){

    string pattern, text, link;
    if(argc == 3){
        pattern = argv[1];
        link = argv[2];
    } else {
        cerr << "Brak argumentow" << endl;
        return -1;
    }
    
    ifstream file(link);
    while(getline(file, text)){
        //KMP(text, pattern);
        for(auto pos : KMP(text, pattern)){
            cout << "Wzorzec występuje z przesunięciem " << pos << endl;
        }
    }

    return 0;
}