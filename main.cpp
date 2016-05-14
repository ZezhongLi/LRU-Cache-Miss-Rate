//
//  main.cpp
//  CacheOpt
//
//  Created by lee Neil on 5/12/16.
//  Copyright © 2016 Neil Li. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

static int blockSize = 32;
static int nsetAss = 8;
static int memSize = 16384;
static int wordSize = 8;

using namespace std;

void pError(string info){
    std::cout<<">>>>>>>> "<<info<<" error<<<<<<<<<";
}

int getTrace(string fpath, vector<int> *trace_data){
    
    fstream bin_file;
    bin_file.open(fpath, ios::binary | ios::in);
    
    if (bin_file.is_open()) {
        while (!bin_file.eof()) {
            char * buf = new char();
            bin_file.read(buf, sizeof(buf));
            (*trace_data).push_back(atoi(buf));
        }
    }
    else{
        return -1;
    }
    bin_file.close();
    return 0;
}

void updateLRU(vector<vector<int>> *lruVec, int setId, int wayNum){

    int n = (*lruVec)[setId][wayNum];
    for (int i = 0; i < nsetAss; ++i) {
        if (i != wayNum && (*lruVec)[setId][i] < n) {
            (*lruVec)[setId][i] += 1;
        }
    }
    (*lruVec)[setId][wayNum] = 0;
}

void updateCache(vector<vector<int>> *cacheVec, vector<vector<int>> *lruVec, int setId, int blockId, int *nmiss){
    
    /* Search for this cache line */
    for (int i = 0; i < nsetAss; ++i) {
        if ((*cacheVec)[setId][i] == blockId) {
            /* cache found */
            return;
        }
    }

    /* Check if there's empty slot */
    for (int n = 0; n < nsetAss; ++n) {
        /* currently its empty */
        if ((*cacheVec)[setId][n] == -1) {
            (*cacheVec)[setId][n] = blockId;
            updateLRU(lruVec, setId, n);
            ++(*nmiss);
            return;
        }
    }
    
    /* replacement happening */
    /* check lru vec and find lru, then replace cache */
    for (int i = 0; i < nsetAss; ++i) {
        if ((*lruVec)[setId][i] == nsetAss - 1) {
            (*cacheVec)[setId][i] = blockId;
            updateLRU(lruVec, setId, i);
            ++(*nmiss);
            return;
        }
    }
    
}

float trace_work(string fpath){
    vector<int> trace_data;
    int nmiss = 0;
    
    int nSets = memSize / (blockSize * nsetAss);
    
    /* A flag vector for LRU */
    vector<vector<int>> lruVec(nSets, vector<int> (nsetAss, nsetAss-1));
    
    /*
     * A vector to simulate the cache,
     * width = number of ways of associate; height = number of associate
     */
    vector<vector<int>> cacheVec(nSets, vector<int> (nsetAss, -1));
    
    if(getTrace(fpath, &trace_data) == -1){
        pError("getTrace");
    }
    
    
    for (int i = 0; i < trace_data.size() - 1; ++i) {
        int traceId = trace_data[i];
        int blockId = traceId / (blockSize / wordSize);
        int setId = blockId % nSets;
        
        updateCache(&cacheVec, &lruVec, setId, blockId, &nmiss);
    }
    
    
    //cout<<"# of miss = "<<nmiss<<endl;
    //cout<<"# of access = "<<trace_data.size() - 1<<endl;
    float missrate = (float)nmiss / (float)(trace_data.size() - 1);
    
    //cout<<"miss rate = "<< missrate<<endl;
    
    return missrate;
}

int main() {
    
    string fpath1 = "/Users/Neil/Desktop/CacheOpt/CacheOpt/trace1.bin";
    string fpath2 = "/Users/Neil/Desktop/CacheOpt/CacheOpt/trace2.bin";
    string fpath3 = "/Users/Neil/Desktop/CacheOpt/CacheOpt/trace3.bin";
    string fpath4 = "/Users/Neil/Desktop/CacheOpt/CacheOpt/trace4.bin";
    string fpath5 = "/Users/Neil/Desktop/CacheOpt/CacheOpt/trace5.bin";
    float mr1 = trace_work(fpath1);
    float mr2 = trace_work(fpath2);
    float mr3 = trace_work(fpath3);
    float mr4 = trace_work(fpath4);
    float mr5 = trace_work(fpath5);
    
    float avg = (mr1+mr2+mr3+mr4+mr5)/5.0;
    cout<<"miss rate = "<< avg<<endl;
    
    return 0;
}











