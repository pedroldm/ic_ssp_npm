#ifndef SOLVER_HPP
#define SOLVER_HPP

/* Eligibilidade - instancias_txt/SSP-NPM-I/ins7_m=2_j=10_t=10_var=7.txt */

#include <sys/stat.h>
#include <random>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <climits>
#include <vector>
#include <chrono>
#include <cstdio>
#include <numeric>
#include <tuple>
#include <functional>
#include <set>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

#define PRINT_MATRIX true

/* Evaluation functions */
int toolsDistances(int machineIndex, int currentJob, int currentTool, int jobAssignedCount);
int GPCA();
int KTNS();
int makespanEvaluation();
int replacedTool(int i);
void fillToolsDistances(int machineIndex, int jobCount);
int fillStartMagazine(int machineIndex, int jobsAssignedCount);
int checkJobEligibility(int machineIndex, int job);
void checkMachinesEligibility();
int flowtimeEvaluation();

/* Local search methods */
/* 0 */ bool jobInsertionLocalSearchCrit(function<int(void)> evaluationFunction, vector<int> evaluationVector, int currentBest);
/* 0 */ bool jobInsertionLocalSearchFull(function<int(void)> evaluationFunction, vector<int> evaluationVector, int currentBest);
/* 1 */ bool twoOptLocalSearch(function<int(void)> evaluationFunction, int currentBest);
/* 2 */ bool jobExchangeLocalSearchCrit(function<int(void)> evaluationFunction, vector<int> evaluationVector, int currentBest);
/* 2 */ bool jobExchangeLocalFull(function<int(void)> evaluationFunction, vector<int> evaluationVector, int currentBest);
/* 3 */ bool swapLocalSearch(function<int(void)> evaluationFunction, int currentBest);
/* 4 */ bool oneBlockLocalSearch(function<int(void)> evaluationFunction, int currentBest);
vector<tuple<int,int>> findOneBlocks(int machineIndex, int tool);

/* Metaheuristics */
void multiStartRandom(function<int(void)> evaluationFunction, vector<int> &evaluationVector);
void VNDCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector);
void ILSCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector);
void VNDFull(function<int(void)> evaluationFunction);
void ILSFull(function<int(void)> evaluationFunction);
void jobInsertionDisturb();
void jobExchangeDisturb();
void twoOptDisturb();
void swapDisturb();

/* Constructive Heuristics */
void constructInitialSolution();
int minTSJ();
int calcJobDifferences(int j1, int j2);
int mostSimilarJob(vector<int> &rj, int machineIndex);

/* I/O */
int singleRun(string inputFileName, ofstream& outputFile, int run);
void readProblem(string fileName);
void parseArguments(vector<string> arguments);
template <typename S>
int printSolution(string inputFileName, double runningTime, int objective, int run, S &s);
template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v);
template<typename T>
ostream& operator<< (ostream& out, const vector<vector<T>>& matrix);
template<typename T>
ostream& operator<<(ostream& out, const set<T>& m);
template <typename T>
ostream& operator<<(ostream& os, const vector<tuple<T, T>>& vector);

/* Initializes and terminates all data structures */
void initialization();
void termination();

int runs, objective, maxIterations, flowtimeSum, flowtimeAux, objectives[] = {1 /*TS*/, 2/*Makespan*/, 3/*Flowtime*/};
int machineCount, toolCount, jobCount, currentBest, best; /* quantity of machines, tools, jobs and current best solution value */
int enableLS1, enableLS2, enableLS3, enableLS4; /* quantity of machines, tools, jobs and current best solution value */
float disturbSize = 0.10;
string instance, inputFileName, ans;
ifstream fpIndex;
ofstream outputFile;
vector<vector<int>> npmJobAssignement, bestSolution; /* current jobs and tools assigned to each machine and best solution assignements */
vector<vector<int>> toolsRequirements; /* jobs and tools requirements */
vector<vector<int>> npmJobTime; /* time cost of each job on each machine */
vector<vector<int>> npmCurrentMagazines; /* current magazines of each machine */
vector<vector<int>> npmToolsNeedDistance; /* current magazines of each machine */
vector<int> npmMagazineCapacity; /* magazine capacity of each machine */
vector<int> npmSwitchCost; /* switch cost of each machine */
vector<int> npmCurrentToolSwitches; /* current switches count of each machine */
vector<int> npmCurrentMakespan; /* current makespan of each machine */
int npmCurrentFlowTime; /* current flowtime of each machine */
vector<set<int>> jobSets; /* tool occurrence for each job */
vector<set<int>> magazines; /* magazines */
set<tuple<int, int>> dist; /* tools distances for replacement on KTNS */
vector<vector<int>> toolsDistancesGPCA; /* tools distances for replacement on GPCA */
vector<vector<int>> jobEligibility; /* eligibility matrix */

int flowtimeEvaluation() {
    npmCurrentFlowTime = 0;

    for(int machineIndex = 0 ; machineIndex < machineCount ; machineIndex++) {
        flowtimeSum = 0;
        int jobsAssignedCount = (int)npmJobAssignement[machineIndex].size(), empty; /* .size() -> O(1)*/
        npmCurrentToolSwitches[machineIndex] = 0;
        if(!npmJobAssignement[machineIndex].size())
            continue;

        fillToolsDistances(machineIndex, jobsAssignedCount); /* O(tj) */
        int startSwitch = fillStartMagazine(machineIndex, jobsAssignedCount); /* O(j) */
        
        flowtimeAux = (startSwitch > jobsAssignedCount) ? jobsAssignedCount : startSwitch;
        for(int i = 0 ; i < flowtimeAux ; i++) {
            flowtimeSum += npmJobTime[machineIndex][npmJobAssignement[machineIndex][i]];
            npmCurrentFlowTime += flowtimeSum;
        }

        for(int i = startSwitch ; i < jobsAssignedCount ; i++) {
            magazines[1].insert(jobSets[npmJobAssignement[machineIndex][i]].begin(), jobSets[npmJobAssignement[machineIndex][i]].end()); /* O(t) */
            empty = npmMagazineCapacity[machineIndex] - magazines[1].size();
            for(auto t : magazines[0]) { /* O(t) */
                if(!toolsRequirements[t][npmJobAssignement[machineIndex][i]])
                    dist.insert(make_tuple(toolsDistancesGPCA[t][i], t)); /* O(logt) */
            }
            npmCurrentToolSwitches[machineIndex] += dist.size() - empty;

            flowtimeSum += ((dist.size() - empty) * npmSwitchCost[machineIndex]) + npmJobTime[machineIndex][npmJobAssignement[machineIndex][i]];
            npmCurrentFlowTime += flowtimeSum;

            if(empty) {
                for(auto t : dist) {
                    magazines[1].insert(get<1>(t)); /* O(logt) */
                    if(!--empty)
                        break;
                }
            }
            magazines[0] = magazines[1]; /* O(t) */
            magazines[1].clear(); /* O(t) */
            dist.clear(); /* O(t) */
        }
        magazines[0].clear(); /* O(t) */
    }

    return npmCurrentFlowTime;
}

int GPCA() {
    for(int machineIndex = 0 ; machineIndex < machineCount ; machineIndex++) {
        int jobsAssignedCount = (int)npmJobAssignement[machineIndex].size(), empty; /* .size() -> O(1)*/
        npmCurrentToolSwitches[machineIndex] = 0;
        if(!npmJobAssignement[machineIndex].size())
            continue;

        fillToolsDistances(machineIndex, jobsAssignedCount); /* O(tj) */
        int startSwitch = fillStartMagazine(machineIndex, jobsAssignedCount); /* O(j) */

        for(int i = startSwitch ; i < jobsAssignedCount ; i++) {
            magazines[1].insert(jobSets[npmJobAssignement[machineIndex][i]].begin(), jobSets[npmJobAssignement[machineIndex][i]].end()); /* O(t) */
            empty = npmMagazineCapacity[machineIndex] - magazines[1].size();
            for(auto t : magazines[0]) { /* O(t) */
                if(!toolsRequirements[t][npmJobAssignement[machineIndex][i]])
                    dist.insert(make_tuple(toolsDistancesGPCA[t][i], t)); /* O(logt) */
            }
            npmCurrentToolSwitches[machineIndex] += dist.size() - empty;

            if(empty) {
                for(auto t : dist) {
                    magazines[1].insert(get<1>(t)); /* O(logt) */
                    if(!--empty)
                        break;
                }
            }
            magazines[0] = magazines[1]; /* O(t) */
            magazines[1].clear(); /* O(t) */
            dist.clear(); /* O(t) */
        }
        magazines[0].clear(); /* O(t) */
    }
    return accumulate(npmCurrentToolSwitches.begin(),npmCurrentToolSwitches.end(), 0);
}

int fillStartMagazine(int machineIndex, int jobsAssignedCount) {
    for(int i = 0 ; i < jobsAssignedCount ; i++) {
        for(auto s : jobSets[npmJobAssignement[machineIndex][i]]) {
            if(npmMagazineCapacity[machineIndex] - magazines[0].size())
                magazines[0].insert(s);
            else    
                return i;
        }
    }
    return INT_MAX;
}

void fillToolsDistances(int machineIndex, int jobsAssignedCount) {
    for(int i = 0 ; i < toolCount ; i++) {
        if(toolsRequirements[i][npmJobAssignement[machineIndex][jobsAssignedCount - 1]])
            toolsDistancesGPCA[i][jobsAssignedCount - 1] = jobsAssignedCount - 1;
        else
            toolsDistancesGPCA[i][jobsAssignedCount - 1] = INT_MAX;
    }

    for(int i = jobsAssignedCount - 2 ; i >= 0 ; i--) {
        for(int j = 0 ; j < toolCount ; j++) {
            if(toolsRequirements[j][npmJobAssignement[machineIndex][i]])
                toolsDistancesGPCA[j][i] = i;
            else 
                toolsDistancesGPCA[j][i] = toolsDistancesGPCA[j][i + 1];
        }
    }
}

int replacedTool (int i) {
    for(auto t : magazines[0])
        dist.insert(make_tuple(toolsDistancesGPCA[t][i], t));

    return get<1>(*dist.rbegin());
}

int KTNS() {
    for(int machineIndex = 0 ; machineIndex < machineCount ; machineIndex++) {
        npmCurrentToolSwitches[machineIndex] = 0;
        int jobsAssignedCount = (int)npmJobAssignement[machineIndex].size();

        fillToolsDistances(machineIndex, jobsAssignedCount);
        int startSwitch = fillStartMagazine(machineIndex, jobsAssignedCount);

        for(int i = startSwitch ; i < jobsAssignedCount ; i++) {
            for(int j = 0 ; j < toolCount ; j++) {
                if(toolsRequirements[j][npmJobAssignement[machineIndex][i]] && find(magazines[0].begin(), magazines[0].end(), j) == magazines[0].end()) {
                    magazines[0].insert(j);
                    magazines[0].erase(replacedTool(i));
                    npmCurrentToolSwitches[machineIndex]++;
                    dist.clear();
                }
            }
        }
        magazines[0].clear();
    }
    return accumulate(npmCurrentToolSwitches.begin(),npmCurrentToolSwitches.end(),0);
}

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

bool jobInsertionLocalSearchFull(function<int(void)> evaluationFunction, vector<int> evaluationVector, int currentBest) {
    for(int l = 0 ; l < machineCount ; l++) {
        for (int i = 0 ; i < (int)npmJobAssignement[l].size() ; i++) {
            int jobIndex = npmJobAssignement[l][i];
            npmJobAssignement[l].erase(npmJobAssignement[l].begin() + i);

            for(int j = 0 ; j < machineCount ; j++) {
                if (j == l || !jobEligibility[j][jobIndex])
                    continue;
                npmJobAssignement[j].insert(npmJobAssignement[j].begin(), jobIndex);
                if(evaluationFunction() < currentBest)
                    return true;
                for(int k = 1 ; k < (int)npmJobAssignement[j].size() ; k++) {
                    swap(npmJobAssignement[j][k - 1], npmJobAssignement[j][k]);
                    if(evaluationFunction() < currentBest)
                        return true;
                }
                npmJobAssignement[j].pop_back();
            }
            
            npmJobAssignement[l].insert(npmJobAssignement[l].begin() + i, jobIndex);
        }
    }

    return false;
}

bool jobInsertionLocalSearchCrit(function<int(void)> evaluationFunction, vector<int> evaluationVector, int currentBest) {
    int criticalMachine = distance(evaluationVector.begin(),max_element(evaluationVector.begin(), evaluationVector.end()));

    for (int i = 0 ; i < (int)npmJobAssignement[criticalMachine].size() ; i++) {
        int jobIndex = npmJobAssignement[criticalMachine][i];
        npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + i);

        for(int j = 0 ; j < machineCount ; j++) {
            if (j == criticalMachine || !jobEligibility[j][jobIndex])
                continue;
            npmJobAssignement[j].insert(npmJobAssignement[j].begin(), jobIndex);
            if(evaluationFunction() < currentBest)
                return true;
            for(int k = 1 ; k < (int)npmJobAssignement[j].size() ; k++) {
                swap(npmJobAssignement[j][k - 1], npmJobAssignement[j][k]);
                if(evaluationFunction() < currentBest)
                    return true;
            }
            npmJobAssignement[j].pop_back();
        }
        
        npmJobAssignement[criticalMachine].insert(npmJobAssignement[criticalMachine].begin() + i, jobIndex);
    }

    return false;
}

bool jobExchangeLocalSearchCrit(function<int(void)> evaluationFunction, vector<int> evaluationVector, int currentBest) {
    int criticalMachine = distance(evaluationVector.begin(),max_element(evaluationVector.begin(), evaluationVector.end()));

    for(int i = 0 ; i < (int)npmJobAssignement[criticalMachine].size() ; i++) {
        for(int l = 0 ; l < machineCount; l++) {
            for (int j = 0 ; j < (int)npmJobAssignement[l].size() ; j++) {
                if (jobEligibility[criticalMachine][npmJobAssignement[l][j]] && jobEligibility[l][npmJobAssignement[criticalMachine][i]]) {
                    swap(npmJobAssignement[criticalMachine][i], npmJobAssignement[l][j]);
                    if(evaluationFunction() < currentBest)
                            return true;
                    else
                        swap(npmJobAssignement[criticalMachine][i], npmJobAssignement[l][j]);
                }
            }
        }
    }

    return false;
}

bool jobExchangeLocalSearchFull(function<int(void)> evaluationFunction, int currentBest) {
    for (int l = 0 ; l < machineCount ; l++) {
        for (int k = 0 ; k < machineCount ; k++) {
            if (k == l)
                continue;
            for(int i = 0 ; i < (int)npmJobAssignement[l].size() ; i++) {
                for (int j = 0 ; j < (int)npmJobAssignement[k].size() ; j++) {
                    if (jobEligibility[l][npmJobAssignement[k][j]] && jobEligibility[k][npmJobAssignement[l][i]]) {
                        swap(npmJobAssignement[l][i], npmJobAssignement[k][j]);
                        if(evaluationFunction() < currentBest)
                                return true;
                        else
                            swap(npmJobAssignement[l][i], npmJobAssignement[k][j]);
                    }
                }
            }
        }
    }

    return false;
}

bool twoOptLocalSearch(function<int(void)> evaluationFunction, int currentBest) {
    for(int i = 0 ; i < machineCount ; i++) {
        for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
            for (int k = j + 1 ; k < (int)npmJobAssignement[i].size() ; k++) {
                reverse(npmJobAssignement[i].begin() + j, npmJobAssignement[i].end() - k + 1);
                if(evaluationFunction() < currentBest)
                    return true;
                else
                    reverse(npmJobAssignement[i].begin() + j, npmJobAssignement[i].end() - k + 1);
            }
        }
    }

    return false;
}

bool swapLocalSearch(function<int(void)> evaluationFunction, int currentBest) {
    for(int i = 0 ; i < machineCount ; i++) {
        for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
            for (int k = j + 1 ; k < (int)npmJobAssignement[i].size() ; k++) {
                swap(npmJobAssignement[i][j], npmJobAssignement[i][k]);
                if(evaluationFunction() < currentBest)
                    return true;
                else
                    swap(npmJobAssignement[i][j], npmJobAssignement[i][k]);
            }
        }
    }

    return false;
}

vector<tuple<int,int>> findOneBlocks(int machineIndex, int tool) {
    vector<tuple<int,int>> oneBlocks;
    int blockStart;

    for(int i = 0 ; i < (int)npmJobAssignement[machineIndex].size() ; i++) {
        if(toolsRequirements[tool][npmJobAssignement[machineIndex][i]]) {
            blockStart = i;
            while(i < (int)npmJobAssignement[machineIndex].size() && toolsRequirements[tool][npmJobAssignement[machineIndex][i]]) { i++; }
            oneBlocks.push_back(make_tuple(blockStart, i - 1));
        }
    }

    return oneBlocks;
}

bool oneBlockLocalSearch(function<int(void)> evaluationFunction, int currentBest) {
    int job;
    vector<tuple<int,int>> oneBlocks;

    for(int i = 0 ; i < machineCount ; i++) {
        for(int j = 0 ; j < toolCount ; j++) {
            oneBlocks = findOneBlocks(i, j);
            for(int remove = 0 ; remove < (int)oneBlocks.size() ; remove++) {
                for(int insert = 0 ; insert < (int)oneBlocks.size() ; insert++) {
                    if (remove == insert)
                        continue;
                    for(int m = get<0>(oneBlocks[remove]) ; m <= get<1>(oneBlocks[remove]) ; m++) {
                        job = npmJobAssignement[i][m];
                        if(insert > remove) {
                            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + get<0>(oneBlocks[insert]), job);
                            npmJobAssignement[i].erase(npmJobAssignement[i].begin() + m);
                            if(evaluationFunction() < currentBest)
                                return true;
                                
                            for(int n = get<0>(oneBlocks[insert]) - 1; n < get<1>(oneBlocks[insert]); n++) {
                                swap(npmJobAssignement[i][n], npmJobAssignement[i][n + 1]);
                                if(evaluationFunction() < currentBest)
                                    return true;
                            }

                            npmJobAssignement[i].erase(npmJobAssignement[i].begin() + get<1>(oneBlocks[insert]));
                            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + m, job);
                        }
                        else {
                            npmJobAssignement[i].erase(npmJobAssignement[i].begin() + m);
                            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + get<0>(oneBlocks[insert]), job);
                            if(evaluationFunction() < currentBest)
                                return true;

                            for(int n = get<0>(oneBlocks[insert]); n <= get<1>(oneBlocks[insert]); n++) {
                                swap(npmJobAssignement[i][n], npmJobAssignement[i][n + 1]);
                                if(evaluationFunction() < currentBest)
                                    return true;
                            }

                            npmJobAssignement[i].erase(npmJobAssignement[i].begin() + get<1>(oneBlocks[insert]) + 1);
                            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + m, job);
                        }
                    }
                }
            }
        }
    }
    return false;
}

int makespanEvaluation() {
    GPCA();

    for(int i = 0 ; i < machineCount ; i++) {
        npmCurrentMakespan[i] = npmCurrentToolSwitches[i] * npmSwitchCost[i];
        for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
            npmCurrentMakespan[i] += npmJobTime[i][npmJobAssignement[i][j]];
        }
    }

    return *max_element(npmCurrentMakespan.begin(), npmCurrentMakespan.end());
}

void multiStartRandom(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    for (int i = 0 ; i < 100 ; i++) {
        randomInitialSolution();
        currentBest = evaluationFunction();
        while(jobInsertionLocalSearchCrit(evaluationFunction, evaluationVector, currentBest)  || 
                jobExchangeLocalSearchCrit(evaluationFunction, evaluationVector, currentBest) || 
                twoOptLocalSearch(evaluationFunction, currentBest) ||
                swapLocalSearch(evaluationFunction, currentBest) ||
                oneBlockLocalSearch(evaluationFunction, currentBest)) { currentBest = evaluationFunction();}
        if (evaluationFunction() < best) {
            best = evaluationFunction();
            bestSolution = npmJobAssignement;
        }
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

void updateBestSolution(function<int(void)> evaluationFunction) {
    bestSolution = npmJobAssignement;
    best = evaluationFunction();
}

void ILSFull(function<int(void)> evaluationFunction) {
    constructInitialSolution();
    VNDFull(evaluationFunction);
    updateBestSolution(evaluationFunction);

    int iterations = 0;
    while(iterations++ < maxIterations) {
        npmJobAssignement = bestSolution;
        for(int i = 0 ; i < ceil(disturbSize * jobCount) ; i++)
            jobInsertionDisturb();
        VNDFull(evaluationFunction);
        if (evaluationFunction() <= best)
            updateBestSolution(evaluationFunction);
    }
}

void VNDFull(function<int(void)> evaluationFunction) {
    int k = 1;
    while (k != 4) {
        currentBest = evaluationFunction();
        switch(k) {
            case 1 :
                k = jobExchangeLocalSearchFull(evaluationFunction, currentBest) ? 1 : k + 1;
                break;
            case 2 : 
                k = swapLocalSearch(evaluationFunction, currentBest) ? 1 : k + 1;
                break;
            case 3 :
                k = twoOptLocalSearch(evaluationFunction, currentBest) ? 1 : k + 1;
                break;
        }
    }
}

void ILSCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    constructInitialSolution();
    VNDCrit(evaluationFunction, evaluationVector);
    updateBestSolution(evaluationFunction);

    int iterations = 0;
    while(iterations++ < maxIterations) {
        npmJobAssignement = bestSolution;
        for(int i = 0 ; i < ceil(disturbSize * jobCount) ; i++)
            jobInsertionDisturb();
        VNDCrit(evaluationFunction, evaluationVector);
        if (evaluationFunction() <= best)
            updateBestSolution(evaluationFunction);
    }
}

void VNDCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    int k = 1;
    while (k != 4) {
        currentBest = evaluationFunction();
        switch(k) {
            case 1 :
                k = jobExchangeLocalSearchFull(evaluationFunction, currentBest) ? 1 : k + 1;
                break;
            case 2 : 
                k = swapLocalSearch(evaluationFunction, currentBest) ? 1 : k + 1;
                break;
            case 3 :
                k = twoOptLocalSearch(evaluationFunction, currentBest) ? 1 : k + 1;
                break;
        }
    }
}

int singleRun(string inputFileName, ofstream& outputFile, int run, int objective) {
    double runningTime;
    readProblem(inputFileName);
    checkMachinesEligibility();

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

    switch(objective) {
        case 1 : 
            ILSFull(GPCA);
            break;
        case 2 :
            ILSCrit(makespanEvaluation, npmCurrentMakespan);
            break;
        case 3 :
            ILSFull(flowtimeEvaluation);
            break;
    }

	high_resolution_clock::time_point t2 = high_resolution_clock::now(); 

  	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	runningTime = time_span.count();											

    npmJobAssignement = bestSolution;
	printSolution(inputFileName, runningTime, objective, run, cout);		
	printSolution(inputFileName, runningTime, objective, run, outputFile);		

    switch(objective) {
        case 1 : 
            return GPCA();
        case 2 :
            return makespanEvaluation();
        case 3 :
            return flowtimeEvaluation();
        default :
            throw invalid_argument("ERROR : Objective not well informed");
    }												
}

void readProblem(string fileName) {
    int aux;
	ifstream fpIn(fileName);
    if(!fpIn)
        throw invalid_argument("ERROR : Instance " + fileName + "doesn't exist");

    fpIn >> machineCount >> jobCount >> toolCount;

    for(int i = 0 ; i < machineCount ; i++) {
        fpIn >> aux;
        npmMagazineCapacity.push_back(aux);
    }

    initialization();

    for(int i = 0 ; i < machineCount ; i++) {
        fpIn >> aux;
        npmSwitchCost.push_back(aux);
    }

    for(int i = 0 ; i < machineCount ; i++) {
        for(int j = 0 ; j < jobCount ; j++) {
            fpIn >> aux;
            npmJobTime[i].push_back(aux);
        }
    }
    
    for(int i = 0 ; i < toolCount ; i++) {
        for(int j = 0 ; j < jobCount ; j++) {
            fpIn >> aux;
            toolsRequirements[i].push_back(aux);
            if(aux)
                jobSets[j].insert(i);
        }
    }

}

int checkJobEligibility(int machineIndex, int job) {
    int sum = 0;
    for(int i = 0 ; i < toolCount ; i++)
        sum += toolsRequirements[i][job];
    return (npmMagazineCapacity[machineIndex] >= sum) ? 1 : 0;
}

void checkMachinesEligibility() {
    for(int i = 0 ; i < machineCount ; i++) {
        for(int j = 0 ; j < jobCount ; j++) {
            jobEligibility[i][j] = checkJobEligibility(i, j);
        }
    }
}

void initialization() {
    toolsRequirements.resize(toolCount);
    npmJobTime.resize(machineCount);
    npmJobAssignement.resize(machineCount);

    npmCurrentMagazines.resize(machineCount);
    npmToolsNeedDistance.resize(machineCount);
    for(int i = 0 ; i < machineCount ; i++) {
        npmCurrentMagazines[i].resize(npmMagazineCapacity[i]);
        npmToolsNeedDistance[i].resize(npmMagazineCapacity[i]);
    }
    
    for(int i = 0 ; i < machineCount ; i++) {
        npmCurrentToolSwitches.push_back(INT_MAX);
        npmCurrentMakespan.push_back(INT_MAX);
    }

    jobEligibility.resize(machineCount);
    for(int i = 0 ; i < machineCount ; i++)
        jobEligibility[i].resize(jobCount);

    jobSets.resize(jobCount);
    toolsDistancesGPCA.resize(toolCount);
    for(int i = 0 ; i < toolCount ; i++)
        toolsDistancesGPCA[i].resize(jobCount);
    magazines.resize(2);

    best = INT_MAX;
}

void termination() {
    for(auto& v : npmJobAssignement) {
        v.clear();
    }
    for(auto& v : toolsRequirements) {
        v.clear();
    }
    for(auto& v : npmJobTime) {
        v.clear();
    }
    for(auto & v : npmCurrentMagazines) {
        v.clear();
    }
    for(auto & v : npmToolsNeedDistance) {
        v.clear();
    }
    for(auto & v : jobSets) {
        v.clear();
    }

    npmJobAssignement.clear();
    toolsRequirements.clear();
    npmJobTime.clear();
    npmCurrentMagazines.clear();
    npmToolsNeedDistance.clear();

    npmMagazineCapacity.clear();
    npmSwitchCost.clear();
    npmCurrentToolSwitches.clear();
    npmCurrentMakespan.clear();

    machineCount = 0;
    toolCount = 0;
    jobCount = 0;
}

template <typename S>
int printSolution(string inputFileName, double runningTime, int objective, int run, S &s) {
    int totalToolSwitches = GPCA(), makespan = makespanEvaluation(), flowtime = flowtimeEvaluation();

    s << "RUN : " << run << " - " << inputFileName << "\n\n";
    for(int i = 0 ; i < machineCount ; i++) {
        s << "- MACHINE [" << i << "]" << "\n\n";

        if(PRINT_MATRIX) {
            for(int j = 0 ; j < toolCount ; j++) {
                for(int k : bestSolution[i]) {
                    s << toolsRequirements[j][k] << " ";
                }
                s << endl;
            }
            s << endl;
        }

        s << "JOBS " << bestSolution[i];
        s << "TOOL SWITCHES : " << npmCurrentToolSwitches[i] << endl;
        s << "MAKESPAN : " << npmCurrentMakespan[i] << "\n\n";
    }

    s << "# -------------------------- #" << "\n\n";
    s << "--- TOTAL TOOL SWITCHES : " << totalToolSwitches << endl;
    s << "--- HIGHEST MAKESPAN : " << makespan << endl;
    s << "--- TOTAL FLOWTIME : " << flowtime << endl;
    s << "--- RUNNING TIME : " << runningTime << "\n\n";
    s << "# -------------------------- #\n";

    return best;
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& vector) {
    os << "[" << vector.size() << "] - [";
    for(int i = 0; i < (int)vector.size(); ++i) {
        os << vector[i];
        if(i != (int)vector.size() - 1)
            os << ", ";
    }
    os << "]\n";
    return os;
}

template <typename T>
ostream& operator<<(ostream& os, const vector<tuple<T, T>>& vector) {
    os << "[" << vector.size() << "] - [";
    for(int i = 0; i < (int)vector.size(); ++i) {
        os << "(" << get<0>(vector[i]) << ", " << get<1>(vector[i]) << ")";
        if(i != (int)vector.size() - 1)
            os << " ; ";
    }
    os << "]\n";
    return os;
}

template<typename T>
ostream& operator<<(ostream& out, const vector<vector<T>>& matrix) {
    for(int i = 0 ; i < (int)matrix.size() ; i++) {
        for(int j = 0 ; j < (int)matrix[i].size() ; j++) {
            out << matrix[i][j] << " ";
        }
        out << endl;
    }
    out << endl;
    return out;
}

template<typename T>
ostream& operator<<(ostream& out, const set<T>& m) {
    out << "[ ";
    for (auto& c : m) {
        out << c << ' ';
    }
    out << "]" << endl;
    return out;
}

bool fileExists(const std::string& filename) {
    struct stat buf;
    return (stat(filename.c_str(), &buf) != -1);
}

#endif