#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <random>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <climits>
#include <vector>
#include <chrono>
#include <cstdio>
#include <tuple>

using namespace std;
using namespace std::chrono;

#define PRINT_MATRIX true

/* I/O */
void readProblem(string fileName);
template <typename T>
void printVector(vector<T> v);
void printMatrix(vector<vector<int>> matrix);
template <typename S>
void printSolution(string inputFileName, double runningTime, int run, S &s);
template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v);
template<typename T>
ostream& operator<< (ostream& out, const vector<vector<T>>& matrix);

/* Initializes and terminates all data structures */
void initialization();
void termination();

/* Evaluation functions */
int fillStartMagazine(int machineIndex, int jobsAssignedCount);
void fillToolsDistances(int machineIndex, int currentJob, int jobAssignedCount);
int toolsDistances (int machineIndex, int currentJob, int currentTool, int jobAssignedCount);
void toolSwitchesEvaluation(int machineIndex);
void flowtimeEvaluation(int machineIndex);
void makespanEvaluation(int machineIndex);

void singleRun(string inputFileName, ofstream& outputFile, int run);

int machineCount, toolCount, jobCount; /* quantity of machines, tools and jobs. Index of the tool to be substituted */
vector<vector<int>> npmJobAssignement; /* current jobs and tools assigned to each machine */
vector<vector<int>> toolsRequirements; /* all tools and jobs */
vector<vector<int>> npmJobTime; /* time cost of each job on each machine */
vector<vector<int>> npmCurrentMagazines; /* current magazines of each machine */
vector<vector<int>> npmToolsNeedDistance; /* current magazines of each machine */
vector<int> npmMagazineCapacity; /* magazine capacity of each machine */
vector<int> npmSwitchCost; /* switch cost of each machine */
vector<int> npmCurrentToolSwitches; /* current switches count of each machine */
vector<int> npmCurrentMakespan; /* current makespan of each machine */
vector<int> npmCurrentFlowTime; /* current flowtime of each machine */

int fillStartMagazine(int machineIndex, int jobsAssignedCount) {
    int i, j, aux = 0;
    
    for(i = 0 ; i < jobsAssignedCount; i++) {
        for(j = 0 ; j < toolCount ; j++) {
            if(toolsRequirements[j][npmJobAssignement[machineIndex][i]]) {
                if(aux < npmMagazineCapacity[machineIndex]) {
                    if(find(npmCurrentMagazines[machineIndex].begin(), npmCurrentMagazines[machineIndex].end(), j) == npmCurrentMagazines[machineIndex].end()) {
                        npmCurrentMagazines[machineIndex][aux++] = j;
                    }
                }
                else {
                    return i;
                }
            }
        }
    } 
    return i;
}

void fillToolsDistances(int machineIndex, int currentJob, int jobAssignedCount) {
    
    for(int i = 0 ; i < npmMagazineCapacity[machineIndex] ; i++) {
        npmToolsNeedDistance[machineIndex][i] = toolsDistances(machineIndex, currentJob, npmCurrentMagazines[machineIndex][i], jobAssignedCount);
    }
}

int toolsDistances (int machineIndex, int currentJob, int currentTool, int jobAssignedCount) {
    int aux = 0;

    for(int i = currentJob ; i < jobAssignedCount ; i++) {
        if(toolsRequirements[currentTool][npmJobAssignement[machineIndex][i]])
            return aux;
        else
            aux++;
    } 

    return INT_MAX;
}

void toolSwitchesEvaluation(int machineIndex) { 
    int jobsAssignedCount = (int)npmJobAssignement[machineIndex].size();
    npmCurrentToolSwitches[machineIndex] = 0;

    int currentJob = fillStartMagazine(machineIndex, jobsAssignedCount);

    for(int i = currentJob ; i < jobsAssignedCount ; i++) {
        fillToolsDistances(machineIndex, i, jobsAssignedCount);
        for(int j = 0 ; j < toolCount ; j++) {
            if(toolsRequirements[j][npmJobAssignement[machineIndex][i]] && find(npmCurrentMagazines[machineIndex].begin(), npmCurrentMagazines[machineIndex].end(), j) == npmCurrentMagazines[machineIndex].end()) {
                int indexToolChange = distance(npmToolsNeedDistance[machineIndex].begin(),max_element(npmToolsNeedDistance[machineIndex].begin(), npmToolsNeedDistance[machineIndex].end()));
                npmCurrentMagazines[machineIndex][indexToolChange] = j;
                npmToolsNeedDistance[machineIndex][indexToolChange] = 0;
                npmCurrentToolSwitches[machineIndex]++;
            } 
        }
    }
}

void flowtimeEvaluation(int machineIndex) {
	npmCurrentFlowTime[machineIndex] = 0;

    for(int i = 0 ; i < (int)npmJobAssignement[machineIndex].size() ; i++) {
        npmCurrentFlowTime[machineIndex] += npmJobTime[machineIndex][npmJobAssignement[machineIndex][i]];
    }
}

void makespanEvaluation(int machineIndex) {
    toolSwitchesEvaluation(machineIndex);
    flowtimeEvaluation(machineIndex);
    npmCurrentMakespan[machineIndex] = npmCurrentFlowTime[machineIndex] + npmCurrentMakespan[machineIndex] * npmSwitchCost[machineIndex];
}

void makeInitialRandomSolution() {
    vector<int> r(jobCount);
    iota(r.begin(), r.end(), 0);

    mt19937 rng(std::random_device{}());
    shuffle(r.begin(), r.end(), rng);

    for (int i = 0 ; i < jobCount ; i++)
        npmJobAssignement[i % machineCount].push_back(r[i]);
}

void singleRun(string inputFileName, ofstream& outputFile, int run) {
    double runningTime;
    readProblem(inputFileName);

    makeInitialRandomSolution();

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

    /* solve */

	high_resolution_clock::time_point t2 = high_resolution_clock::now(); 

  	duration<double> time_span = duration_cast<duration<double> >(t2 - t1);
	runningTime =  time_span.count();											

	printSolution(inputFileName, runningTime, run, cout);		
	printSolution(inputFileName, runningTime, run, outputFile);		

	termination();																
}

/* Reads the problem from a file specified by fileName */
void readProblem(string fileName) {
	ifstream fpIn(fileName);
    int aux;

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
        npmCurrentFlowTime.push_back(INT_MAX);
    }
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

    npmJobAssignement.clear();
    toolsRequirements.clear();
    npmJobTime.clear();
    npmCurrentMagazines.clear();
    npmToolsNeedDistance.clear();

    npmMagazineCapacity.clear();
    npmSwitchCost.clear();
    npmCurrentToolSwitches.clear();
    npmCurrentMakespan.clear();
    npmCurrentFlowTime.clear();

    machineCount = 0;
    toolCount = 0;
    jobCount = 0;
}

template <typename S>
void printSolution(string inputFileName, double runningTime, int run, S &s) {
    int totalToolSwitches = 0, totalFlowtime = 0, totalMakespan = 0;

    s << "RUN : " << run << " - " << inputFileName << "\n\n";
    for(int i = 0 ; i < machineCount ; i++) {
        s << "- MACHINE [" << i << "]" << "\n\n";

        if(PRINT_MATRIX) {
            for(int j = 0 ; j < toolCount ; j++) {
                for(int k : npmJobAssignement[i]) {
                    s << toolsRequirements[j][k] << " ";
                }
                s << endl;
            }
        }

        s << "\nJOBS " << npmJobAssignement[i];

        toolSwitchesEvaluation(i);
        totalToolSwitches += npmCurrentToolSwitches[i];
        flowtimeEvaluation(i);
        totalFlowtime += npmCurrentFlowTime[i]; 
        //makespanEvaluation(i);

        if(npmCurrentMakespan[i] > totalMakespan) 
            totalMakespan = npmCurrentMakespan[i];

        s << "TOOL SWITCHES : " << npmCurrentToolSwitches[i] << endl;
        s << "FLOWTIME : " << npmCurrentFlowTime[i] << endl;
        s << "MAKESPAN : " << npmCurrentMakespan[i] << "\n\n";
    }

    s << "# -------------------------- #" << "\n\n";
    s << "--- TOTAL TOOL SWITCHES : " << totalToolSwitches << endl;
    s << "--- TOTAL FLOWTIME : " << totalFlowtime << endl;
    s << "--- TOTAL MAKESPAN : " << totalMakespan << endl;
    s << "--- RUNNING TIME : " << runningTime << "\n\n";
    s << "# -------------------------- #\n";
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

template<typename T>
ostream& operator<<(ostream& out, const vector<vector<T>>& matrix) {
    for(int i = 0 ; i < (int)matrix.size() ; i++) {
        for(int j = 0 ; j < (int)matrix[0].size() ; j++) {
            out << matrix[i][j] << " ";
        }
        out << endl;
    }
    out << endl;
    return out;
}

#endif