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
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    while(iterations++ < maxIterations && stillHaveTime) {
        npmJobAssignement = bestSolution;
        for(int i = 0 ; i < ceil(disturbSize * jobCount) ; i++) {
            if (dis(gen) < criticJobPercentage) {
                if(!criticJobDisturb())
                    jobInsertionDisturb();
            }
            else
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
    bool stillHaveTime = VNDCrit(evaluationFunction, evaluationVector);
    int it = evaluationFunction();
    timeTracking[0] = it;
    improvements.push_back(make_tuple(1, it));
    updateBestSolution(evaluationFunction);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    while(iterations++ < maxIterations && stillHaveTime) {
        npmJobAssignement = bestSolution;
        for(int i = 0 ; i < ceil(disturbSize * jobCount) ; i++) {
            if (dis(gen) < criticJobPercentage)
                if(!criticJobDisturb())
                    jobInsertionDisturb();
            else
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
    makespanEvaluation();

    int criticalMachine = distance(npmCurrentMakespan.begin(),max_element(npmCurrentMakespan.begin(), npmCurrentMakespan.end()));
    random_device rd;
    mt19937 gen(rd());
    mt19937 rng(random_device{}());
    vector<int>otherMachines;
    for(int i = 0 ; i < machineCount ; i++)
        if (i != criticalMachine)
            otherMachines.push_back(i);
    shuffle(otherMachines.begin(), otherMachines.end(), rng);

    uniform_int_distribution<> distribution(0, npmJobAssignement[criticalMachine].size() - 1);
    int randomIndex = distribution(gen);
    int jobToReassign = npmJobAssignement[criticalMachine][randomIndex];

    for(int i = 0 ; i < (int) otherMachines.size() ; i++) {
        if(jobEligibility[i][jobToReassign]) {
            uniform_int_distribution<> destDistribution(0, npmJobAssignement[i].size() - 1);
            int destRandomIndex = destDistribution(gen);
            npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + randomIndex);
            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + destRandomIndex, jobToReassign);
            break;
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

bool criticJobDisturb() {
    vector<set<tuple<int,int>>> cJ = criticJobEvaluation();
    for(int i = 0 ; i < machineCount ; i++) {
        npmCurrentMakespan[i] = npmCurrentToolSwitches[i] * npmSwitchCost[i];
        for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
            npmCurrentMakespan[i] += npmJobTime[i][npmJobAssignement[i][j]];
        }
    }
    mt19937 rng(random_device{}());
    vector<int> mv(machineCount);

    int criticalMachine = distance(npmCurrentMakespan.begin(),max_element(npmCurrentMakespan.begin(), npmCurrentMakespan.end()));
    if (cJ[criticalMachine].empty())
        return false;
    tuple<int, int> jobToRemove = *cJ[criticalMachine].rbegin();
    int jobToReassign = npmJobAssignement[criticalMachine][get<1>(jobToRemove)];
    vector<int> otherMachines;
    for(int i = 0 ; i < machineCount ; i++){
        if (i != criticalMachine && jobEligibility[i][jobToReassign])
            otherMachines.push_back(i);
    }
    if (otherMachines.empty())
        return false;
    npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + get<1>(jobToRemove));
    
    uniform_real_distribution<double> distribution(0.0, 1.0);
    double randomValue = distribution(rng);
    if (randomValue <= lowestMakespanPercentage){
        vector<int>makespanSubset;
        for (const auto& m: otherMachines){
            makespanSubset.push_back(npmCurrentMakespan[m]);
        }
        int lowestMakespan = distance(makespanSubset.begin(),min_element(makespanSubset.begin(), makespanSubset.end()));
        npmJobAssignement[lowestMakespan].push_back(jobToReassign);
    } else{
        shuffle(otherMachines.begin(), otherMachines.end(), rng);
        npmJobAssignement[otherMachines[0]].push_back(jobToReassign);
    }

    return true;
}

void updateBestSolution(function<int(void)> evaluationFunction) {
    bestSolution = npmJobAssignement;
    best = evaluationFunction();
}