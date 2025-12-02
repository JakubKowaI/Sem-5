#include "Algorithms.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

using namespace std;

void dijkstraMode(vector<vector<pair<int,int>>> &graph,string sources,string outputFile,string dataFile,int n,int a,int minc, int maxc){
    bool skip=0; 
    vector<double> times;

    cout << unitbuf;

    if(sources.empty()){
        skip=1;
        for(int i=1;i<graph.size();i++){
            if(!graph[i].empty()){
                times.push_back(Dijsktra(graph,i));
                break;
            }
        }
    }
    ifstream data(sources);
    
    ofstream output(outputFile);
    char c;
    output<<"p res sp ss dikstra"<<endl;
    output<<"f "<<dataFile<<" "<<sources<<endl;
    output<<"g "<<n<<" "<<a<<" "<<minc<<" "<<maxc<<endl;

    int z;
    int count=0;
    
    if(!skip){
        while(data.get(c)){
            if(c=='c'){
                string temp;
                getline(data,temp);
                //cout<<temp<<endl;
            }else if(c=='p'){
                string aux,sp,ss;
                data>>aux>>sp>>ss>>z;
                string temp;
                getline(data,temp);
            }else if(c=='s'){
                int s;
                data>>s;
                string temp;
                getline(data,temp);
                times.push_back(Dijsktra(graph,s));
                count++;
                cerr<<"\r                                                               \rPrzetworzono: "<<count;
            }
        }
    }
    cout<<endl;
    double avg=0.0;
    for(auto it:times){
        avg+=it;
    }
    if(times.size()==0){
        cout<<"Size == 0 BLAD!!!!"<<endl;
    }else{
        avg=avg/times.size();
    }
    
    output<<"t "<<avg<<endl;
    output<<"c przetworzono "<<times.size()<<" zrodel"<<endl;

    data.close();
    output.close();
}

void dijkstraSearchMode(vector<vector<pair<int,int>>> graph,string sources,string outputFile,string dataFile,int n,int a,int minc, int maxc){
    ifstream data(sources);
    ofstream output(outputFile);
    char c;
    output<<"p res sp ss dikstra"<<endl;
    output<<"f "<<dataFile<<" "<<sources<<endl;
    output<<"g "<<n<<" "<<a<<" "<<minc<<" "<<maxc<<endl;

    int z;
    int count=0;
    vector<double> times;
    while(data.get(c)){
        if(c=='c'){
            string temp;
            getline(data,temp);
            //cout<<temp<<endl;
        }else if(c=='p'){
            string aux,sp,ss;
            data>>aux>>sp>>ss>>z;
        }else if(c=='q'){
            if(++count<=z){
            int s,d;
            data>>s>>d;
            output<<"d "<<s<<" "<<d<<" "<<DijsktraSearch(graph,s,d)<<endl;
            cout<<s<<" : "<<d<<" : "<<c<<endl;
            }else{
                cerr<<"Too many nights"<<endl;
            }
        }
    }

    data.close();
    output.close();
}

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
    int minCost=INT_MAX;
    int maxCost=INT_MIN;
    vector<vector<pair<int,int>>> graph;

    while(data.get(c)){
        if(c=='c'){
            string temp;
            getline(data,temp);
            //cout<<temp<<endl;
        }else if(c=='p'){
            string prob;
            data>>prob>>n>>a;
            string temp;
            getline(data,temp);
            if(prob!="sp"){
                cerr<<"Found something different than sp"<<endl;
                return -1;
            }
            //cout<<"Source: "<<s<<" Destination: "<<d<<endl;
            graph.clear();
            graph.resize(n+1);

        }else if(c=='a'){
            if(++count<=a){
                //cout<<"Czytam a "<<a<<endl;
            int s,d,cost;
            data>>s>>d>>cost;
            string temp;
            getline(data,temp);
            graph[s].push_back(pair<int,int>(d,cost));
            if(cost<minCost){
                minCost=cost;
            }
            if(cost>maxCost){
                maxCost=cost;
            }

            //cout<<s<<" : "<<d<<" : "<<c<<endl;
            }else{
                cerr<<"Too much arcs"<<endl;
            }
        }
    }
    string sources;
    string outputPath;
    string p2pSources;
    string p2pOutputPath;
    for(int i=1;i<argc;i++){
        if(string(argv[i])=="-ss" && i+1<argc){
            // cout<<"test: "<<argv[i+1];
            if(argv[i+1][0]!='-'){
                // cout<<"Test2: "<<argv[i]
                sources = argv[i+1];
            }
            break;
        }
    }

    for(int i=1;i<argc;i++){
        if(string(argv[i])=="-p2p" && i+1<argc){
            if(argv[i+1][0]!='-'){
                p2pSources = argv[i+1];
            }
            break;
        }
    }

    for(int i=1;i<argc;i++){
        if(string(argv[i])=="-oss" && i+1<argc){
            if(argv[i+1][0]!='-'){
                outputPath = argv[i+1];
            }
            break;
        }
    }

    for(int i=1;i<argc;i++){
        if(string(argv[i])=="-op2p" && i+1<argc){
            if(argv[i+1][0]!='-'){
                p2pOutputPath = argv[i+1];
            }
            break;
        }
    }

    string prog = filesystem::path(argv[0]).filename().string();
    // cout<<"Prog: "<<prog<<" Data: "<<dataFile<<" Sources: "<<sources<<endl;
    if (prog == "dijkstra") {
        if(!sources.empty()){
            cout<<"Checking whole graph for shortest paths"<<endl;
            dijkstraMode(graph,sources,outputPath,dataFile,n,a,minCost,maxCost);
        }else if(!p2pSources.empty()){
            cout<<"Checking graph for shortest path between nodes"<<endl;
            dijkstraSearchMode(graph,p2pSources,outputPath,dataFile,n,a,minCost,maxCost);
        }else{
            cerr<<"Wrong use"<<endl;
        }
    } else if (prog == "diala") {
        // kod dla diala
    } else if (prog == "radixheap") {
        // kod dla radixheap
    } else {
        cerr<<"I don't even know what happened"<<endl;
    }

    //Dijsktra(graph,1);
    //DijsktraSearch(graph,1,5);
    return 0;
}