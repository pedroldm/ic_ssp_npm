#ifndef GLOBAL_VARS_HPP
#define GLOBAL_VARS_HPP

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
#include <cmath>
#include <iomanip>

using namespace std;
using namespace std::chrono;

#define PRINT_MATRIX false

inline int runs = 1, objective, maxIterations = 1000, flowtimeSum, flowtimeAux, iterations, objectives[] = {1 /*TS*/, 2/*Makespan*/, 3/*Flowtime*/};
inline int machineCount, toolCount, jobCount, currentBest, best, beforeSwap1, beforeSwap2, maxTime = 3600;
inline float disturbSize = 0.05, oneBlockPercentage = 0.25;
inline duration<double> time_span;
inline string instance, inputFileName, ans;
inline ifstream fpIndex;
inline ofstream outputFile;
inline vector<int> npmMagazineCapacity, npmSwitchCost, npmCurrentToolSwitches, npmCurrentMakespan, npmCurrentFlowTime, mI, randomTools;
inline vector<set<int>> jobSets, magazines;
inline set<tuple<int, int>> dist;
inline vector<tuple<int,int>> oneBlocks, improvements;
inline vector<vector<int>> npmJobAssignement, bestSolution, toolsRequirements, npmJobTime, npmCurrentMagazines, npmToolsNeedDistance;
inline vector<vector<int>> toolsDistancesGPCA, jobEligibility;
inline high_resolution_clock::time_point t1, t2;

class Instance {
    public:
        vector<vector<int>> assignement;
        int toolSwitches;
        int makespan;
        int flowtime;
        double runningTime;
        vector<tuple<int,int>> improvements;

        Instance(vector<vector<int>> _assignement, int _toolSwitches, int _makespan, int _flowtime, vector<tuple<int,int>> _improvements, double _runningTime) {
            assignement = _assignement;
            toolSwitches = _toolSwitches;
            makespan = _makespan;
            flowtime = _flowtime;
            improvements = _improvements;
            runningTime = _runningTime;
        }
};

class Results {
    public:
        vector<Instance> solutions;

    void addSolution(Instance s) { solutions.push_back(s); }
    
    int getBestResult(int objective) {
        int best = INT_MAX;
        switch(objective) {
            case 1:
                for(int i = 0 ; i < (int)solutions.size() ; i++) {
                    if (solutions[i].toolSwitches < best)
                        best = solutions[i].toolSwitches;
                }
                break;
            case 2:
                for(int i = 0 ; i < (int)solutions.size() ; i++) {
                    if (solutions[i].makespan < best)
                        best = solutions[i].makespan;
                }
                break;
            case 3:
                for(int i = 0 ; i < (int)solutions.size() ; i++) {
                    if (solutions[i].flowtime < best)
                        best = solutions[i].flowtime;
                }
                break;           
        }

        return best;
    }

    double getMean(int objective) {
        int sum = 0;
        switch(objective) {
            case 1:
                for(int i = 0 ; i < (int)solutions.size() ; i++) {
                    sum += solutions[i].toolSwitches;
                }
                break;
            case 2:
                for(int i = 0 ; i < (int)solutions.size() ; i++) {
                    sum += solutions[i].makespan;
                }
                break;
            case 3:
                for(int i = 0 ; i < (int)solutions.size() ; i++) {
                    sum += solutions[i].flowtime;
                }
                break;           
        }

        return (double)sum / (double)solutions.size();
    }

    double getStandardDeviation(int objective) {
        double standardDeviation = 0, mean;
        switch(objective) {
            case 1:
                mean = this->getMean(1);
                for(int i = 0 ; i < (int)solutions.size() ; i++) {
                    standardDeviation += pow(solutions[i].toolSwitches - mean, 2);
                }
                break;
            case 2:
                mean = this->getMean(2);
                for(int i = 0 ; i < (int)solutions.size() ; i++) {
                    standardDeviation += pow(solutions[i].makespan - mean, 2);
                }
                break;
            case 3:
                mean = this->getMean(3);
                for(int i = 0 ; i < (int)solutions.size() ; i++) {
                    standardDeviation += pow(solutions[i].flowtime - mean, 2);
                }
                break;           
        }

        return sqrt(standardDeviation / (double)solutions.size());
    }

    double getMeanExecutionTime() {
        double sum = 0;
        for (int i = 0 ; i < solutions.size() ; i++) {
            sum += solutions[i].runningTime;
        }
        
        return sum / (double)solutions.size();
    }
};

class Summary {
    public:
        vector<Results*> results;

    void addResults(Results* r) { results.push_back(r); }

    double getGeneralMean(int objective) {
        long sum = 0;
        for (int i = 0 ; i < results.size() ; i++) {
            sum += results[i]->getBestResult(objective);
        }

        return (double)sum / (double)results.size() ;
    }

    double getMeanStandardDeviation(int objective) {
        long sum = 0;
        for (int i = 0 ; i < results.size() ; i++) {
            sum += results[i]->getStandardDeviation(objective);
        }

        return (double)sum / (double)results.size() ;
    }

    double getMeanExecutionTime() {
        long sum = 0;
        for (int i = 0 ; i < results.size() ; i++) {
            sum += results[i]->getMeanExecutionTime();
        }

        return (double)sum / (double)results.size() ;
    }
};

#endif