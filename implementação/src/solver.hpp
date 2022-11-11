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

using namespace std;
using namespace std::chrono;

#define PRINT_MATRIX true

void readProblem(string fileName);
void initialization();
void termination();

void fillStartMagazine(int machineIndex);
void KTNS(int machineIndex, int currentJob);
int toolsDistance (int machineIndex, int currentJob, int currentTool);
int toolSwitchesEvaluation(int machineIndex);
int flowtimeEvaluation(int machineIndex);
int makespanEvaluation(int machineIndex);

void singleRun(string inputFileName, string fpOut, int run);

template <typename T>
void printVector(vector<T> v);
void printMatrix(vector<vector<int>> matrix);
/* Prints the solution information to the output file */
void printSolution(double runningTime, int run);

int machines, tools, jobs; /* quantity of machines, tools and jobs */
unsigned int fillJob, fillTool, switchCount, jobCount;
vector<vector<vector<int>>> npmJobSequency; /* current jobs and tools assigned to each machine */
vector<vector<int>> toolsAndJobs; /* all tools and jobs */
vector<vector<int>> npmJobTime; /* time cost of each job on each machine */
vector<vector<int>> npmCurrentMagazines; /* current magazines of each machine */
vector<vector<int>> npmToolsNeededSoonest; /* current magazines of each machine */
vector<int> npmMagazineCapacity; /* magazine capacity of each machine */
vector<int> npmSwitchCost; /* switch cost of each machine */
vector<int> npmCurrentToolSwitches; /* current switches count of each machine */
vector<int> npmCurrentMakespan; /* current makespan of each machine */
vector<int> npmCurrentFlowTime; /* current flowtime of each machine */

void fillStartMagazine(int machineIndex) {
    int aux = 0;
    
    for(unsigned int i = 0 ; i <= jobCount ; i++) {
        fillJob = i;
        for (int j = 1 ; j <= tools ; j++) {
            fillTool = j;
            if(npmJobSequency[machineIndex][j][i]) {
                if(aux < npmMagazineCapacity[machineIndex]) {
                    if (find(npmCurrentMagazines[machineIndex].begin(), npmCurrentMagazines[machineIndex].end(), j) == npmCurrentMagazines[machineIndex].end()) {
                        npmCurrentMagazines[machineIndex][aux++] = j;
                    }
                }
                else {
                    return;
                }
            }
        }
    }
    return;
}

void KTNS(int machineIndex, int currentJob) {
    int aux = 0;
    
    for (auto i : npmCurrentMagazines[machineIndex]) {
        npmToolsNeededSoonest[machineIndex][aux++] = toolsDistance(machineIndex, currentJob, i);
    }
}

int toolsDistance (int machineIndex, int currentJob, int currentTool) {
    int aux = 0;

    for (unsigned int i = currentJob ; i <= jobCount ; i++) {
        if (npmJobSequency[machineIndex][currentTool][i])
            return aux;
        else
            aux++;
    }

    return INT_MAX;
}

int toolSwitchesEvaluation(int machineIndex)
{
    jobCount = npmJobSequency[machineIndex][0].size();
    switchCount = 0;

    fillStartMagazine(machineIndex);

    for (unsigned int i = fillJob ; i <= jobCount ; i++) {
        KTNS(machineIndex, i);
        for(int j = 1 ; j <= tools ; j++) {
            if(npmJobSequency[machineIndex][j][i] && find(npmCurrentMagazines[machineIndex].begin(), npmCurrentMagazines[machineIndex].end(), j) == npmCurrentMagazines[machineIndex].end()) {         
                int indexChangedTool = distance(npmToolsNeededSoonest[machineIndex].begin(), max_element(npmToolsNeededSoonest[machineIndex].begin(), npmToolsNeededSoonest[machineIndex].end()));
                npmCurrentMagazines[machineIndex][indexChangedTool] = j;
                npmToolsNeededSoonest[machineIndex][indexChangedTool] = toolsDistance(machineIndex, i, j);
                switchCount++;
            }
        }
    }

    return switchCount;
}

/* Flowtime Evaluation Function */
int flowtimeEvaluation(int machineIndex)
{
    return 1;
    int flowTime = 0;

    for (unsigned int i = 0 ; i < npmJobSequency[machineIndex][0].size() ; i++) {
        flowTime += npmJobTime[machineIndex][npmJobSequency[machineIndex][0][i]];
    }
    flowTime += toolSwitchesEvaluation(machineIndex) * npmSwitchCost[machineIndex];
    
    return flowTime;
}

/* Makespan Evaluation Function */
int makespanEvaluation(int machineIndex)
{
	int makespan = 0;

    for (unsigned int i = 0 ; i < npmJobSequency[machineIndex][0].size() ; i++) {
        makespan += npmJobTime[machineIndex][npmJobSequency[machineIndex][0][i]];
    }
    
    return makespan;
}

void singleRun(string inputFileName, string fpOut, int run)
{
    double runningTime;
    readProblem(inputFileName);

    toolSwitchesEvaluation(0);

	high_resolution_clock::time_point t1 = high_resolution_clock::now();		

	high_resolution_clock::time_point t2 = high_resolution_clock::now(); 

  	duration<double> time_span = duration_cast<duration<double> >(t2 - t1);
	runningTime =  time_span.count();											

	printSolution(runningTime, run);			

	termination();																
}

/* Reads the problem from a file specified by fileName */
void readProblem(string fileName)
{
	ifstream fpIn(fileName);
    int aux;

    fpIn >> machines >> jobs >> tools;

    initialization();

    for(int i = 0 ; i < machines ; i++) {
        fpIn >> aux;
        npmMagazineCapacity.push_back(aux);
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
        fpIn >> aux;
        npmSwitchCost.push_back(aux);
    }

    for(int i = 0 ; i < machines ; i++) {
        for (int j = 0 ; j < jobs ; j++) {
            fpIn >> aux;
            npmJobTime[i].push_back(aux);
        }
    }
    for(int i = 1 ; i < tools + 1 ; i++) {
        for(int j = 0 ; j < jobs ; j++) {
            fpIn >> aux;
            toolsAndJobs[i].push_back(aux);
            for (int l = 0 ; l < machines ; l++) /* Temp */
                npmJobSequency[l][i].push_back(aux);
        }
    }
}

void initialization()
{
    toolsAndJobs.resize(tools + 1); /* Primeira linha = índice {0,..., j-1} de cada tarefa */
    toolsAndJobs[0].push_back(INT_MAX); /* [0][0] */
    for(int i = 0 ; i < jobs ; i++) { /* Adicionando os índices das tarefas à primeira linha */
        toolsAndJobs[0].push_back(i);
    }

    npmJobTime.resize(machines);

    npmJobSequency.resize(machines, vector<vector<int>>(tools + 1));
    for(int i = 0 ; i < machines ; i++) { /* Atribuindo todos os j a todas as máquinas P/ TESTE */
        for(int j = 0 ; j < jobs ; j ++) {
            npmJobSequency[i][0].push_back(j);
        }
    }
    
    for(int i = 0 ; i < machines ; i++) {
        npmCurrentToolSwitches.push_back(INT_MAX);
        npmCurrentMakespan.push_back(INT_MAX);
        npmCurrentFlowTime.push_back(INT_MAX);
    }
}

void termination()
{
    for (auto& v : npmJobSequency) {
        for (auto& x : v) {
            x.clear();
        }
        v.clear();
    }
    for (auto& v : npmJobTime) {
        v.clear();
    }
    for (auto& v : toolsAndJobs) {
        v.clear();
    }
    for (auto & v : npmCurrentMagazines) {
        v.clear();
    }
    for (auto & v : npmToolsNeededSoonest) {
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
    fillJob = 0;
    fillTool = 0; 
    switchCount = 0;
    jobCount = 0;
}

void printSolution(double runningTime, int run) {
    int totalToolSwitches = 0, totalFlowtime = 0, totalMakespan = 0;

    for (int i = 0 ; i < machines ; i++) {
        cout << "- MACHINE [" << i << "]" << "\n\n";

        if (PRINT_MATRIX) 
            printMatrix(npmJobSequency[i]);

        cout << "JOBS : ";
        for(unsigned int j = 0 ; j < npmJobSequency[i][0].size(); j++) {
            cout << npmJobSequency[i][0][j] << " ";
        }
        cout << endl;

        totalToolSwitches += npmCurrentToolSwitches[i] = toolSwitchesEvaluation(i);
        totalMakespan += npmCurrentMakespan[i] = makespanEvaluation(i);
        npmCurrentFlowTime[i] = flowtimeEvaluation(i);
        if (npmCurrentFlowTime[i] > totalFlowtime) 
            totalFlowtime = npmCurrentFlowTime[i];

        cout << "TOOL SWITCHES : " << npmCurrentToolSwitches[i] << endl;
        cout << "FLOWTIME : " << npmCurrentFlowTime[i] << endl;
        cout << "MAKESPAN : " << npmCurrentMakespan[i] << "\n\n";
    }

    cout << "--------------------------" << "\n\n";
    cout << "--- TOTAL TOOL SWITCHES : " << totalToolSwitches << endl;
    cout << "--- TOTAL FLOWTIME : " << totalFlowtime << endl;
    cout << "--- TOTAL MAKESPAN : " << totalMakespan << endl;
    cout << "--- RUNNING TIME : " << runningTime << endl;
}

void printMatrix(vector<vector<int>> matrix) { 
    for (unsigned int i = 1 ; i < matrix.size() ; i++) {
        for (unsigned int j = 0 ; j < matrix[0].size() ; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

template <typename T> 
void printVector(vector<T> v) {
    for (auto i : v)
        cout << i << " ";
    cout << endl;
}

#endif