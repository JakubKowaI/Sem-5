#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <chrono>
#include <random>
#include <thread>
#include <utility>

using namespace std;

void run_command(const string& command) {
    system(command.c_str());
}

int main(){
    string families[]={"Long-C","Long-n","Random4-C","Random4-n","Square-C","Square-n"};
    pair<int,int> limits[]={{0,15},{10,21},{0,15},{10,21},{0,15},{10,21}};
    for(int i=0;i<families->size();i++){
        cout<<"Testy dla "+families[i]<<endl;
        for (int j=limits[i].first;j<=limits[i].second;j++) {
            string family=families[i];
            string path="\"ch9-1.1/inputs/" + family + "/" + family + "."+to_string(i)+".0";

            string dijkstra_cmd_ss = "./dijkstra -d "+path+".gr\"" + " -ss " + path + ".ss\"" + " -oss " + "\"wyniki/dijkstra/" + family + + "."+to_string(i)+".0.ss.res\"";
            string diala_cmd_ss = "./diala -d "+path+".gr\"" + " -ss " + path + ".ss\"" + " -oss " + "\"wyniki/diala/" + family + + "."+to_string(i)+".0.ss.res\"";
            string radixheap_cmd_ss  = "./radixheap -d "+path+".gr\"" + " -ss " + path + ".ss\"" + " -oss " + "\"wyniki/radixheap/" + family + + "."+to_string(i)+".0.ss.res\"";

            string dijkstra_cmd_p2p = "./dijkstra -d "+path+".gr\"" + " -p2p " + path + ".p2p\"" + " -op2p " + "\"wyniki/dijkstra/" + family + + "."+to_string(i)+".0.p2p.res\"";
            string diala_cmd_p2p = "./diala -d "+path+".gr\"" + " -p2p " + path + ".p2p\"" + " -op2p " + "\"wyniki/diala/" + family + + "."+to_string(i)+".0.p2p.res\"";
            string radixheap_cmd_p2p  = "./radixheap -d "+path+".gr\"" + " -p2p " + path + ".p2p\"" + " -op2p " + "\"wyniki/radixheap/" + family + + "."+to_string(i)+".0.p2p.res\"";


            thread t1(run_command, dijkstra_cmd_ss);
            thread t2(run_command, diala_cmd_ss);
            thread t3(run_command, radixheap_cmd_ss);
            thread t4(run_command, dijkstra_cmd_p2p);
            thread t5(run_command, diala_cmd_p2p);
            thread t6(run_command, radixheap_cmd_p2p);

            t1.join();
            t2.join();
            t3.join();
            t4.join();
            t5.join();
            t6.join();
        }
    }

    //Testy dla USA Road

    return 0;
}