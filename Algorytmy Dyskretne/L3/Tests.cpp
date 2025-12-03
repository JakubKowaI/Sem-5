#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <chrono>
#include <random>
#include <thread>

using namespace std;

void run_command(const string& command) {
    system(command.c_str());
}

int main(){
    mt19937 mt{
        static_cast<std::mt19937::result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    )    };

    int k=20;
    for (int i = 10000; i <= 100000; i += 10000) {
        for (int j = 0; j < k; j++) {
            string gen = "./RandGen " + to_string(i) + " > temp.txt";
            system(gen.c_str());

            string bst_cmd = "./BST " + to_string(i) + " < temp.txt";
            string rbt_cmd = "./RBT " + to_string(i) + " < temp.txt";
            string st_cmd  = "./ST "  + to_string(i) + " < temp.txt";

            thread t1(run_command, bst_cmd);
            thread t2(run_command, rbt_cmd);
            thread t3(run_command, st_cmd);

            t1.join();
            t2.join();
            t3.join();
        }
    }
    return 0;
}