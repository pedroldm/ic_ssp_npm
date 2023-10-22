#include "metaheuristics.hpp"
#include "constructive_heuristics.hpp"
#include "local_search.hpp"
#include "evaluation.hpp"
#include "io.hpp"

void ILSFull(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    constructSimilarityMatrix();
    constructInitialSolution();
    improvements.push_back(make_tuple(0, evaluationFunction()));
    bool stillHaveTime = VNDFull(evaluationFunction, evaluationVector);
    int it = evaluationFunction();
    timeTracking[0] = it;
    improvements.push_back(make_tuple(1, it));
    updateBestSolution(evaluationFunction);

    while(iterations++ < maxIterations && stillHaveTime) {
        npmJobAssignement = bestSolution;
        for(int i = 0 ; i < ceil(disturbSize * jobCount) ; i++) {
            jobInsertionDisturb();
        }
        mI.clear();
        evaluationFunction();
        stillHaveTime = VNDFullSim(evaluationFunction, evaluationVector);

        int neighborhoodBest = evaluationFunction();
        if(neighborhoodBest < best)
            improvements.push_back(make_tuple(iterations, neighborhoodBest));

        if (neighborhoodBest <= best)
            updateBestSolution(evaluationFunction);

        registerTimeTracking();
    }
}

void ILSCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    constructSimilarityMatrix();
    constructInitialSolution();
    improvements.push_back(make_tuple(0, evaluationFunction()));
    VNDCrit(evaluationFunction, evaluationVector);
    int it = evaluationFunction();
    timeTracking[0] = it;
    improvements.push_back(make_tuple(1, it));
    updateBestSolution(evaluationFunction);
    bool stillHaveTime = true;

    while(iterations++ < maxIterations && stillHaveTime) {
        npmJobAssignement = bestSolution;
        for(int i = 0 ; i < ceil(disturbSize * jobCount) ; i++) {
            jobInsertionDisturb();
        }
        mI.clear();
        evaluationFunction();
        stillHaveTime = VNDCritSim(evaluationFunction, evaluationVector);
        if(!stillHaveTime)
            break;

        int neighborhoodBest = evaluationFunction();
        if(neighborhoodBest < best)
            improvements.push_back(make_tuple(iterations, neighborhoodBest));

        if (neighborhoodBest <= best)
            updateBestSolution(evaluationFunction);

        registerTimeTracking();
    }
}

void jobInsertionDisturb() {
    vector<int> r(machineCount);
    iota(r.begin(), r.end(), 0);

    mt19937 rng(random_device{}());
    shuffle(r.begin(), r.end(), rng);

    vector<int> jR(npmJobAssignement[r[0]].size());
    iota(jR.begin(), jR.end(), 0);
    shuffle(jR.begin(), jR.end(), rng);

    for(int i = 0 ; i < (int) jR.size() ; i++) {
        if(jobEligibility[r[1]][npmJobAssignement[r[0]][jR[i]]]) {
            npmJobAssignement[r[1]].insert(npmJobAssignement[r[1]].begin() + rand () % (npmJobAssignement[r[1]].size() + 1), npmJobAssignement[r[0]][jR[i]]);
            npmJobAssignement[r[0]].erase(npmJobAssignement[r[0]].begin() + jR[i]);
            return;
        }
    } 
}

void jobExchangeDisturb() {
    vector<int> r(machineCount);
    iota(r.begin(), r.end(), 0);

    mt19937 rng(random_device{}());
    shuffle(r.begin(), r.end(), rng);

    vector<int> jR(npmJobAssignement[r[0]].size()), jI(npmJobAssignement[r[1]].size());
    iota(jR.begin(), jR.end(), 0);
    iota(jI.begin(), jI.end(), 0);
    shuffle(jR.begin(), jR.end(), rng);
    shuffle(jI.begin(), jI.end(), rng);

    for(int i = 0 ; i < (int) jR.size() ; i++) {
        for(int j = 0 ; j < (int) jI.size() ; j++) {
            if(jobEligibility[r[0]][npmJobAssignement[r[1]][jI[j]]] && jobEligibility[r[1]][npmJobAssignement[r[0]][jR[i]]]) {
                swap(npmJobAssignement[r[1]][jI[j]], npmJobAssignement[r[0]][jR[i]]);
                return;
            }
        }
    }
}

void twoOptDisturb() {
    int m = 0;
    mt19937 rng(random_device{}());
    vector<int> mv(machineCount);
    iota(mv.begin(), mv.end(), 0);
    shuffle(mv.begin(), mv.end(), rng);
    for(int i = 0 ; i < (int)mv.size() ; i++) {
        if(npmJobAssignement[i].size() >= 2) {
            m = i;
            break;
        }
    }

    vector<int> r ((int)npmJobAssignement[m].size());
    iota(r.begin(), r.end(), 0);
    shuffle(r.begin(), r.end(), rng);
    
    if (r[1] > r[0])
        reverse(npmJobAssignement[m].begin() + r[0], npmJobAssignement[m].begin() + r[1] + 1);
    else
        reverse(npmJobAssignement[m].begin() + r[1], npmJobAssignement[m].begin() + r[0] + 1);
}

void swapDisturb() {
    int m = 0;
    mt19937 rng(random_device{}());
    vector<int> mv(machineCount);
    iota(mv.begin(), mv.end(), 0);
    shuffle(mv.begin(), mv.end(), rng);
    for(int i = 0 ; i < (int)mv.size() ; i++) {
        if(npmJobAssignement[i].size() >= 2) {
            m = i;
            break;
        }
    }
    vector<int> r ((int)npmJobAssignement[m].size());
    iota(r.begin(), r.end(), 0);
    shuffle(r.begin(), r.end(), rng);
    
    swap(npmJobAssignement[m][r[0]], npmJobAssignement[m][r[1]]);
}

int minTSJ() {
    set<tuple<int, int>> m;
    GPCA();
    
    for(int i = 0 ; i < machineCount ; i++) {
        m.insert(make_tuple(npmCurrentToolSwitches[i]/(int)npmJobAssignement[i].size(), i));
    }
    return get<1>(*m.begin());
}

void updateBestSolution(function<int(void)> evaluationFunction) {
    bestSolution = npmJobAssignement;
    best = evaluationFunction();
}