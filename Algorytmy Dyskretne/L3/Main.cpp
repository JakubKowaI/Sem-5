#include "Algorithms.hpp"
#include <iostream>
#include <vector>

using namespace std;

int main(int argc,char** argv){
    if (argc < 2) {
        cerr << "Using: " << argv[0] << endl;
        return -1;
    }

    string dataFile;

    for(int i=1;i<argc;i++){
        if(string(argv[i])=="-d" && i+1<argc){
            dataFile = argv[i+1];
            break;
        }
    }
    if(dataFile.empty()){
        cerr<<"No data file provided"<<endl;
        return -1;
    }
    ifstream data(dataFile, ios::binary);

    if(!data){
        cerr<<"Problems with file"<<endl;
    }

    char c;
    int n,a;
    int count=0;
    vector<vector<int>> graph;

    while(data.get(c)){
        if(c=='c'){
            string temp;
            getline(data,temp);
            //cout<<temp<<endl;
        }else if(c=='p'){
            string prob;
            data>>prob>>n>>a;
            if(prob!="sp"){
                cerr<<"Found something different than sp"<<endl;
                return -1;
            }
            //cout<<"Source: "<<s<<" Destination: "<<d<<endl;
            graph.assign(n+1, vector<int>(n+1, -1));

        }else if(c=='a'){
            if(++count<=a){
            int s,d,c;
            data>>s>>d>>c;
            graph[s][d]=c;
            //cout<<s<<" : "<<d<<" : "<<c<<endl;
            }else{
                cerr<<"Too much arcs"<<endl;
            }
        }
    }

    // MinHeap h(n);
    //  h.insertKey(3);
    // h.insertKey(2);
    // h.deleteKey(1);
    // h.insertKey(15);
    // h.insertKey(5);
    // h.insertKey(4);
    // h.insertKey(45);
    // cout << h.extractMin() << " ";
    // cout << h.getMin() << " ";
    // h.decreaseKey(2, 1);
    // cout << h.getMin();
    Dijsktra(graph,1);
    return 0;
}