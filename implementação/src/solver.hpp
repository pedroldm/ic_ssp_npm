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
/* Initializes the structures, vectors and matrices used */
void initialization();
/* Terminates all data structures */
void termination();

void fillStartMagazine(vector<int>& magazine, int machineIndex, int *actualTool, int *actualJob);
void KTNS(vector<int>& magazine, vector<int>& toolsNeededSoonest, int machineIndex, int actualJob);
int toolsDistance (int machineIndex, int actualTool, int actualJob);
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
vector<vector<vector<int>>> npmJobSequency; /* current jobs and tools assigned to each machine */
vector<vector<int>> toolsAndJobs; /* all tools and jobs */
vector<vector<int>> npmJobTime; /* time cost of each job on each machine */
vector<int> npmMagazineCapacity; /* magazine capacity of each machine */
vector<int> npmSwitchCost; /* switch cost of each machine */
vector<int> npmCurrentToolSwitches; /* current switches count of each machine */
vector<int> npmCurrentMakespan; /* current makespan of each machine */
vector<int> npmCurrentFlowTime; /* current flowtime of each machine */

void fillStartMagazine(vector<int>& magazine, int machineIndex, int *actualTool, int *actualJob) {
    int aux = 0;
    
    for(long unsigned int i = 0 ; i <= npmJobSequency[machineIndex][0].size() ; i++) {
        *actualJob = i;
        for (int j = 1 ; j <= tools ; j++) {
            *actualTool = j;
            if(!npmJobSequency[machineIndex][j][i])
                continue;
            if(aux < npmMagazineCapacity[machineIndex]) {
                if (find(magazine.begin(), magazine.end(), j) == magazine.end()) {
                    magazine.push_back(j);
                    aux++;
                }
            }
            else {
                return;
            }
        }
    }
    return;
}

void KTNS(vector<int>& magazine, vector<int>& toolsNeededSoonest, int machineIndex, int actualJob) {
    for (auto i : magazine) {
        toolsNeededSoonest.push_back(toolsDistance(machineIndex, i, actualJob));
    }
}

int toolsDistance (int machineIndex, int actualTool, int actualJob) {
    int aux = 0;

    for (long unsigned int i = actualJob ; i <= npmJobSequency[machineIndex][0].size() ; i++) {
        if (npmJobSequency[machineIndex][actualTool][i])
            return aux;
        else
            aux++;
    }

    return INT_MAX;
}

int toolSwitchesEvaluation(int machineIndex)
{
    int actualJob = 0, actualTool = 0, switchCount = 0;
    vector<int> magazine, toolsNeededSoonest;

    fillStartMagazine(magazine, machineIndex, &actualTool, &actualJob);

    for (long unsigned int i = actualJob ; i <= npmJobSequency[machineIndex][0].size() ; i++) {
        toolsNeededSoonest.clear();
        for(int j = 1 ; j <= tools ; j++) {
            if(npmJobSequency[machineIndex][j][i] && find(magazine.begin(), magazine.end(), j) == magazine.end()) {
                if (!toolsNeededSoonest.size())
                    KTNS(magazine, toolsNeededSoonest, machineIndex, i);
                int indexChangedTool = distance(toolsNeededSoonest.begin(), max_element(toolsNeededSoonest.begin(), toolsNeededSoonest.end()));
                magazine[indexChangedTool] = j;
                toolsNeededSoonest[indexChangedTool] = toolsDistance(machineIndex, j, i);
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

    for (long unsigned int i = 0 ; i < npmJobSequency[machineIndex][0].size() ; i++) {
        flowTime += npmJobTime[machineIndex][npmJobSequency[machineIndex][0][i]];
    }
    flowTime += toolSwitchesEvaluation(machineIndex) * npmSwitchCost[machineIndex];
    
    return flowTime;
}

/* Makespan Evaluation Function */
int makespanEvaluation(int machineIndex)
{
	int makespan = 0;

    for (long unsigned int i = 0 ; i < npmJobSequency[machineIndex][0].size() ; i++) {
        makespan += npmJobTime[machineIndex][npmJobSequency[machineIndex][0][i]];
    }
    
    return makespan;
}

void singleRun(string inputFileName, string fpOut, int run)
{
    double runningTime;
    readProblem(inputFileName);

    toolSwitchesEvaluation(0);
    printSolution(0, 0);

	high_resolution_clock::time_point t1 = high_resolution_clock::now();		

	high_resolution_clock::time_point t2 = high_resolution_clock::now(); 

  	duration<double> time_span = duration_cast<duration<double> >(t2 - t1);
	runningTime =  time_span.count();											

	//printSolution(runningTime, run, PRINT_MATRIX);			

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

    npmJobSequency.clear();
    npmJobTime.clear();
    npmSwitchCost.clear();
    npmCurrentToolSwitches.clear();
    npmCurrentMakespan.clear();
    npmCurrentFlowTime.clear();
    toolsAndJobs.clear();

    machines = 0;
    tools = 0;
    jobs = 0;
}

void printSolution(double runningTime, int run) {
    int totalToolSwitches = 0, totalFlowtime = 0, totalMakespan = 0;

    for (int i = 0 ; i < machines ; i++) {
        cout << "- MACHINE [" << i << "]" << "\n\n";

        if (PRINT_MATRIX) 
            printMatrix(npmJobSequency[i]);

        cout << "JOBS : ";
        for(long unsigned int j = 0 ; j < npmJobSequency[i][0].size(); j++) {
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
}

void printMatrix(vector<vector<int>> matrix) { 
    for (long unsigned int i = 1 ; i < matrix.size() ; i++) {
        for (long unsigned int j = 0 ; j < matrix[0].size() ; j++) {
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