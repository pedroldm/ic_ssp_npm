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

using namespace std;
using namespace std::chrono;

#define PRINT_MATRIX true

inline int runs = 1, objective, maxIterations = 1000, flowtimeSum, flowtimeAux, iterations, objectives[] = {1 /*TS*/, 2/*Makespan*/, 3/*Flowtime*/};
inline int machineCount, toolCount, jobCount, currentBest, best, beforeSwap1, beforeSwap2, maxTime = 3600;
inline float disturbSize = 0.05, oneBlockPercentage = 0.25;
inline duration<double> time_span;
inline string instance, inputFileName, ans;
inline ifstream fpIndex;
inline ofstream outputFile;
inline vector<vector<int>> npmJobAssignement, bestSolution, toolsRequirements, npmJobTime, npmCurrentMagazines, npmToolsNeedDistance;
inline vector<int> npmMagazineCapacity, npmSwitchCost, npmCurrentToolSwitches, npmCurrentMakespan, npmCurrentFlowTime, mI, randomTools;
inline vector<set<int>> jobSets, magazines;
inline set<tuple<int, int>> dist;
inline vector<vector<int>> toolsDistancesGPCA, jobEligibility;
inline vector<tuple<int,int>> oneBlocks, improvements;
inline high_resolution_clock::time_point t1, t2;

#endif