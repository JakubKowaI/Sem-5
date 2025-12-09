#include "Algorithms.hpp"
#include <cmath>

struct node
{
    int vertex;
    long dist;

    node(int j,long val){
        vertex=j;
        dist=val;
    }
};


struct kubelek{
    std::vector<node> nodes;
    std::pair<long,long> range;

    kubelek(long a, long b){
        range.first=a;
        range.second=b;
    }

    
};

//rozdzielanie kubelkow
//dzialamy w kubelku k
//og range k =[a,b]
//dmin dla kubelka k
//new range =[dmin,b]
//a=dmin
//range kubelka 0 =[a,a]
//range 1 =[a+1,a+1]
//range 2 = [a+2,a+3]
//range 3 = [a+4,a+7]
//szerokosc kubelka k <= 2^(k-1)

//find first not empty
int findKubelek(std::vector<kubelek> &kubelki){
    for(int i=0;i<kubelki.size();i++){
        if(!kubelki[i].nodes.empty())return i;
    }
    //std::cerr<<"Blad w findKubelek"<<std::endl;
    return -1;
}

long findMin(const kubelek &cur){
    long min=LONG_MAX;
    for(node it:cur.nodes){
        if(it.dist<=min)min=it.dist;
    }
    if(min==LONG_MAX){
      std::cerr<<"Blad w findMin: "<<cur.nodes.size()<<std::endl;
    }
    return min;
}


void pushNode(int n,long dist,std::vector<kubelek> &kub){
    //std::cout<<"SZUKAM!"<<std::endl;
    for(int i=0;i<kub.size();i++){
        //std::cout<<"\n["<<kub[i].range.first<<","<<kub[i].range.second<<"] - "<<dist<<std::endl;
        if(kub[i].range.first<=dist&&dist<=kub[i].range.second){
            kub[i].nodes.push_back(node(n,dist));
            //std::cout<<"FOUND!"<<std::endl;
            return;
        }
        //std::cerr<<"["<<kub[i].range.first<<","<<kub[i].range.second<<"] - "<<dist<<std::endl;
    }
    std::cerr<<"Blad w pushNode"<<std::endl;
}

std::vector<long> RadixHeap(std::vector<std::vector<std::pair<int,int>>> & graph,int s,int maxcost,int n,int dest){
    //deklarujemy ilosc kubelkow ceil(log(nC))+1
    //startujemy z kubelka 1
    //szukamy niepustego kubelka k
    //jesli rozmiar k == 1 to liczymy d[k[0]] ig?

    std::vector<kubelek> kubelki;
    kubelki.push_back(kubelek(0,0));
    kubelki.push_back(kubelek(1,1));
    for(int i=1;i<ceil(log2(graph.size()*maxcost));i++){
        long start = 1L << i;
        long end = (1L << (i+1))-1;
        kubelki.push_back(kubelek(start,end));
    }
    //std::cerr<<std::endl<<kubelki[kubelki.size()-1].range.first<<":"<<kubelki[kubelki.size()-1].range.second<< " --- "<<ceil(log(n*maxcost))<<std::endl;

    std::vector<long> d(graph.size(), LONG_MAX);
    d[s]=0;

    kubelki[0].nodes.push_back(node(s,0));

    int ck=findKubelek(kubelki);
    while(ck!=-1){
        if(ck==0){
            for(node i:kubelki[ck].nodes){
                //if(kubelki[ck].range.first>d[i.vertex])continue;
                if(d[i.vertex]>=kubelki[ck].range.first){
                    d[i.vertex]=kubelki[ck].range.first;
                
                
                    if(i.vertex==dest)return d;
                    for(auto& [j,c] : graph[i.vertex]){
                        long newDist=kubelki[ck].range.first + c;
                        if (newDist < d[j]) {
                            d[j] = newDist; 
                            pushNode(j, newDist, kubelki);
                        }
                    }
                }
            }
            kubelki[ck].nodes.clear();
        }else{
            long a=findMin(kubelki[ck]);
            kubelki[0].range={a,a};
            kubelki[1].range={a+1,a+1};
            for(int i=2;i<ck;i++){
                kubelki[i].range={a+(1L << (i-1)),a+(1L << i)-1};
            }
            kubelki[ck-1].range.second=kubelki[ck].range.second;
            //std::cerr<<"\nOG: "<<kubelki[ck].range.second<< " - NEW: "<<kubelki[ck-1].range.second<<std::endl;
            std::vector<node> nodesToMove = kubelki[ck].nodes;
            kubelki[ck].nodes.clear();
            kubelki[ck].range={kubelki[ck].range.second,kubelki[ck].range.second};
            for(node it : nodesToMove){
                int a = it.vertex;
                long b = it.dist;
                for(int i=0;i<ck;i++){
                    if(kubelki[i].range.first<=b&&b<=kubelki[i].range.second){
                        kubelki[i].nodes.push_back(node(a,b));
                        break;
                    }
                }
            }
        }

        ck=findKubelek(kubelki);
    }
    return d;
}