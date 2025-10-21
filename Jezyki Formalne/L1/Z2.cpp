#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

vector<int> KMPTable(string pattern){
    vector<int> T(pattern.size()+1);
    T[0]=-1;
    int pos=1;
    int cnd=0;
    while(pos<pattern.size()){
        if(pattern[pos]==pattern[cnd]){
            T[pos]=T[cnd];
        }
        else{
            T[pos]=cnd;
            //cnd=T[cnd];
            while(cnd>=0 && pattern[pos]!=pattern[cnd]){
                cnd=T[cnd];
            }
        }
        pos++;
        cnd++;
    }
    T[pos]=cnd;
    return T;
}

vector<int> KMP(string text, string pattern){
    int j=0;
    int k=0;
    vector<int> T=KMPTable(pattern);
    vector<int> wynik;

    while(j<text.size()){
        if(pattern[k]==text[j]){
            j++;
            k++;
            if(k==pattern.size()){
                wynik.push_back(j - k);
                k=T[k];
            }
        }
        else{
            k=T[k];
            if(k<0){
                j++;
                k++;
            }
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