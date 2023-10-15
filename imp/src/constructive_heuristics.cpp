#include "constructive_heuristics.hpp"

void randomInitialSolution() {
    vector<int> r(jobCount);
    iota(r.begin(), r.end(), 0);

    mt19937 rng(random_device{}());
    shuffle(r.begin(), r.end(), rng);

    int i = 0;
    while (!r.empty()) {
        if (jobEligibility[i % machineCount][r[0]]) {
            npmJobAssignement[i % machineCount].push_back(r[0]);
            r.erase(r.begin());
        }
        i++;
    }
}

int calcJobDifferences(int j1, int j2) {
    int sum = 0;
    for(int i = 0 ; i < toolCount ; i++) {
        if(toolsRequirements[i][j1] == toolsRequirements[i][j2])
            sum++;
    }
    return sum;
}

int mostSimilarJob(vector<int> &rj, int machineIndex) {
    set<tuple<int, int>> j;
    for(int i = 0 ; i < (int)rj.size() ; i++)
        j.insert(make_tuple(calcJobDifferences(npmJobAssignement[machineIndex].back(), rj[i]), rj[i]));

    rj.erase(remove(rj.begin(), rj.end(), get<1>(*j.rbegin())), rj.end());
    return get<1>(*j.rbegin());
}

void constructInitialSolution() {
    mt19937 rng(random_device{}());
    vector<int> rj(jobCount);
    set<tuple<int,int>> remainingJobs;
    iota(rj.begin(), rj.end(), 0);
    shuffle(rj.begin(), rj.end(), rng);

    for(int i = 0 ; i < machineCount ; i++) {
        for(int j = 0 ; j < jobCount ; j++) {
            if(jobEligibility[i][rj[j]]) {
                npmJobAssignement[i].push_back(rj[j]);
                rj.erase(rj.begin() + j);
                break;
            }
        }
    }

    int remaining = (int)rj.size();
    for(int i = 0 ; i < remaining ; i++) {
        int machineToReceive = minTSJ();
        int msJob = mostSimilarJob(rj, machineToReceive);
        npmJobAssignement[machineToReceive].push_back(msJob);
    }
}

void constructSimilarityMatrix() {
    for(int i = 0 ; i < jobCount ; i++) {
        for(int j = 0 ; j < jobCount ; j++) 
            similarityMatrix[i][j] = ((calcJobDifferences(i, j) / (double)toolCount) >= similarityPercentage);
    }
}
