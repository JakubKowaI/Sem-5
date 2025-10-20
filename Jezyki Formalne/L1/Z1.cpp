#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

vector<char> obliczAlfabet(string &pattern){
    vector<char> alfabet;
    for(char c : pattern){
        if(find(alfabet.begin(), alfabet.end(), c) == alfabet.end()){
            alfabet.push_back(c);
        }
    }
    return alfabet;
}

int idZnaku(vector<char> &alfabet, char c){
    for(int i = 0; i < alfabet.size(); i++){
        if(alfabet[i] == c) return i;
    }
    return -1;
}

bool czySuffix(const string &a, const string &b){
    if(a.size() > b.size()) return false;
    return b.compare(b.size() - a.size(), a.size(), a) == 0;
}

vector<vector<int>> obliczSigme(string pattern, vector<char> alfabet, int m){
    vector<vector<int>> sigma(pattern.size()+1, vector<int>(alfabet.size(), 0));

    for(int q = 0; q <= m; q++){
        for(int a : alfabet){
            int k = min(m, q + 1);
            while(!czySuffix(pattern.substr(0, k), pattern.substr(0, q) + (char)a)&& k > 0){
                k--;
            }
            sigma[q][idZnaku(alfabet,a)] = k;
        }
    }
    return sigma;
}

int solve(string pattern, string text){
    int m = pattern.size();
    int n = text.size();
    int last = 0;
    vector<char> alfabet = obliczAlfabet(pattern);
    vector<vector<int>> sigma = obliczSigme(pattern, alfabet, m);
    
    // {
    //     {1,0,0},
    //     {1,2,0},
    //     {3,0,0},
    //     {1,4,0},
    //     {5,0,0},
    //     {1,4,6},
    //     {7,0,0},
    //     {1,2,0}
    // };
    int q = 0;
    for(int i = 0; i < n; i++){

        char a = text[i];
        int id = idZnaku(alfabet,a);
        if(id == -1){
            q = 0;
            continue;
        }
        
        q = sigma[q][id];


        //cout << "q: " << q << endl;
        if(q == m){
            cout << "Wzorzec występuje z przesunięciem " << i - m + 1 << endl; 
            last = i;
        }
    }

    return last;
}

int main(int argc, char** argv){
    // string pattern = "ababaca";
    // string text = "abababacaba";
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
        solve(pattern, text);
    }
    //auto m = obliczSigme(pattern, {'a','b','c'}, pattern.size());
    // for(auto row : m){
    //     for(auto val : row){
    //         cout << val << " ";
    //     }
    //     cout << endl;
    // }
    return 0;
}