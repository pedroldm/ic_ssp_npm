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

/* flowtime = completion time - arrival time */

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
tuple<int,int> fillStartMagazine(int machineIndex);
void KTNS(int machineIndex, int currentJob);
int toolsDistance (int machineIndex, int currentJob, int currentTool);
int toolSwitchesEvaluation(int machineIndex);
int flowtimeEvaluation(int machineIndex);
int makespanEvaluation(int machineIndex);

void singleRun(string inputFileName, ofstream& outputFile, int run);

int machines, tools, jobs, indexChangedTool; /* quantity of machines, tools and jobs. Index of the tool to be substituted */
unsigned int switchCount, jobCount; /* current job and tool after initializing the magazine and switch and job counters */
vector<vector<int>> npmJobSequency; /* current jobs and tools assigned to each machine */
vector<vector<int>> toolsAndJobs; /* all tools and jobs */
vector<vector<int>> npmJobTime; /* time cost of each job on each machine */
vector<vector<int>> npmCurrentMagazines; /* current magazines of each machine */
vector<vector<int>> npmToolsNeededSoonest; /* current magazines of each machine */
vector<int> npmMagazineCapacity; /* magazine capacity of each machine */
vector<int> npmSwitchCost; /* switch cost of each machine */
vector<int> npmCurrentToolSwitches; /* current switches count of each machine */
vector<int> npmCurrentMakespan; /* current makespan of each machine */
vector<int> npmCurrentFlowTime; /* current flowtime of each machine */

tuple<int,int> fillStartMagazine(int machineIndex) {
    unsigned int i;
    int j, aux = 0;

    fill(npmCurrentMagazines[machineIndex].begin(), npmCurrentMagazines[machineIndex].end(), 0);
    
    for(i = 0 ; i < jobCount ; i++) {
        for(j = 1 ; j <= tools ; j++) {
            if(toolsAndJobs[j][npmJobSequency[machineIndex][i]]) {
                if(aux < npmMagazineCapacity[machineIndex]) {
                    if(find(npmCurrentMagazines[machineIndex].begin(), npmCurrentMagazines[machineIndex].end(), j) == npmCurrentMagazines[machineIndex].end()) {
                        npmCurrentMagazines[machineIndex][aux++] = j;
                    }
                }
                else {
                    return tuple<int, int>(i, j);
                }
            }
        }
    } 
    return tuple<int, int>(i, j);
}

void KTNS(int machineIndex, int currentJob) {
    int aux = 0;
    
    for(auto i : npmCurrentMagazines[machineIndex]) {
        (!npmToolsNeededSoonest[machineIndex][aux]) ? npmToolsNeededSoonest[machineIndex][aux] = toolsDistance(machineIndex, currentJob, i) : npmToolsNeededSoonest[machineIndex][aux]--;
        aux++;
    }
}

int toolsDistance (int machineIndex, int currentJob, int currentTool) {
    int aux = 0;

    for(unsigned int i = currentJob ; i <= jobCount ; i++) {
        if(toolsAndJobs[currentTool][i])
            return aux;
        else
            aux++;
    } 

    return INT_MAX;
}

int toolSwitchesEvaluation(int machineIndex) {
    jobCount = npmJobSequency[machineIndex].size();
    switchCount = 0;

    tuple<int, int> fill = fillStartMagazine(machineIndex);

    for(unsigned int i = get<0>(fill) ; i < jobCount ; i++) {
        KTNS(machineIndex, i);
        for(int j = get<0>(fill) ; j <= tools ; j++) {
            if(toolsAndJobs[j][npmJobSequency[machineIndex][i]] && find(npmCurrentMagazines[machineIndex].begin(), npmCurrentMagazines[machineIndex].end(), j) == npmCurrentMagazines[machineIndex].end()) {
                int indexToolChange = distance(begin(npmToolsNeededSoonest[machineIndex]), max_element(begin(npmToolsNeededSoonest[machineIndex]), end(npmToolsNeededSoonest[machineIndex])));
                npmCurrentMagazines[machineIndex][indexToolChange] = j;
                npmToolsNeededSoonest[machineIndex][indexToolChange] = 0;
                switchCount++;  
            }   
        }
    } 

    return switchCount;
}

int flowtimeEvaluation(int machineIndex) {
	int flowtime = 0;

    for(unsigned int i = 0 ; i < npmJobSequency[machineIndex].size() ; i++) {
        flowtime += npmJobTime[machineIndex][npmJobSequency[machineIndex][i]];
    }

    return flowtime;
}

int makespanEvaluation(int machineIndex) {
    return flowtimeEvaluation(machineIndex) + toolSwitchesEvaluation(machineIndex) * npmSwitchCost[machineIndex];
}

void singleRun(string inputFileName, ofstream& outputFile, int run) {
    double runningTime;
    readProblem(inputFileName);

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

    fpIn >> machines >> jobs >> tools;

    for(int i = 0 ; i < machines ; i++) {
        fpIn >> aux;
        npmMagazineCapacity.push_back(aux);
    }

    initialization();

    for(int i = 0 ; i < machines ; i++) {
        fpIn >> aux;
        npmSwitchCost.push_back(aux);
    }

    for(int i = 0 ; i < machines ; i++) {
        for(int j = 0 ; j < jobs ; j++) {
            fpIn >> aux;
            npmJobTime[i].push_back(aux);
        }
    }
    
    for(int i = 1 ; i < tools + 1 ; i++) {
        for(int j = 0 ; j < jobs ; j++) {
            fpIn >> aux;
            toolsAndJobs[i].push_back(aux);
        }
    }
}

void initialization() {
    toolsAndJobs.resize(tools + 1); /* Primeira linha = índice {0,..., j-1} de cada tarefa */
    toolsAndJobs[0].push_back(INT_MAX); /* [0][0] */
    for(int i = 0 ; i < jobs ; i++) { /* Adicionando os índices das tarefas à primeira linha */
        toolsAndJobs[0].push_back(i);
    }

    npmJobTime.resize(machines);

    npmJobSequency.resize(machines);
    for(int i = 0 ; i < machines ; i++) { /* Atribuindo todos os j a todas as máquinas P/ TESTE */
        for(int j = 0 ; j < jobs ; j ++) {
            npmJobSequency[i].push_back(j);
        }
    }

    npmCurrentMagazines.resize(machines);
    for(int i = 0 ; i < machines ; i++) {
        npmCurrentMagazines[i].resize(npmMagazineCapacity[i]);
    }
    npmToolsNeededSoonest.resize(machines);
    for(int i = 0 ; i < machines ; i++) {
        npmToolsNeededSoonest[i].resize(npmMagazineCapacity[i]);
    }
    
    for(int i = 0 ; i < machines ; i++) {
        npmCurrentToolSwitches.push_back(INT_MAX);
        npmCurrentMakespan.push_back(INT_MAX);
        npmCurrentFlowTime.push_back(INT_MAX);
    }
}

void termination() {
    for(auto& v : npmJobSequency) {
        v.clear();
    }
    for(auto& v : npmJobTime) {
        v.clear();
    }
    for(auto& v : toolsAndJobs) {
        v.clear();
    }
    for(auto & v : npmCurrentMagazines) {
        v.clear();
    }
    for(auto & v : npmToolsNeededSoonest) {
        v.clear();
    }

    npmJobSequency.clear();
    npmJobTime.clear();
    npmSwitchCost.clear();
    npmCurrentToolSwitches.clear();
    npmCurrentMagazines.clear();
    npmToolsNeededSoonest.clear();
    npmCurrentMakespan.clear();
    npmCurrentFlowTime.clear();
    toolsAndJobs.clear();

    machines = 0;
    tools = 0;
    jobs = 0;
    switchCount = 0;
    jobCount = 0;
    indexChangedTool = 0;
}

template <typename S>
void printSolution(string inputFileName, double runningTime, int run, S &s) {
    int totalToolSwitches = 0, totalFlowtime = 0, totalMakespan = 0;

    s << "RUN : " << run << " - " << inputFileName << "\n\n";
    for(int i = 0 ; i < machines ; i++) {
        s << "- MACHINE [" << i << "]" << "\n\n";

        if(PRINT_MATRIX) {
            for(int j = 1 ; j <= tools ; j++) {
                for(int k : npmJobSequency[i]) {
                    s << toolsAndJobs[j][k] << " ";
                }
                s << endl;
            }
        }

        s << "\nJOBS " << npmJobSequency[i];

        totalToolSwitches += npmCurrentToolSwitches[i] = toolSwitchesEvaluation(i);
        totalFlowtime += npmCurrentFlowTime[i] = flowtimeEvaluation(i);
        npmCurrentMakespan[i] = makespanEvaluation(i);

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
    for(unsigned int i = 0; i < vector.size(); ++i) {
        os << vector[i];
        if(i != vector.size() - 1)
            os << ", ";
    }
    os << "]\n";
    return os;
}

template<typename T>
ostream& operator<<(ostream& out, const vector<vector<T>>& matrix) {
    for(unsigned int i = 1 ; i < matrix.size() ; i++) {
        for(unsigned int j = 0 ; j < matrix[0].size() ; j++) {
            out << matrix[i][j] << " ";
        }
        out << endl;
    }
    out << endl;
    return out;
}

#endif