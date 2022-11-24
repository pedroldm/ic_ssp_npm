#ifndef SOLVER_HPP
#define SOLVER_HPP

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

using namespace std;
using namespace std::chrono;

#define PRINT_MATRIX true

/* I/O */
void validateArguments (int argc, char* argv[]);
void readProblem(string fileName);
template <typename S>
int printSolution(string inputFileName, double runningTime, int objective, int run, S &s);
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
int toolSwitchesEvaluation();
int flowtimeEvaluation();
int makespanEvaluation();

/* Local search methods */
bool jobInsertionLocalSearch(function<int(void)> evaluationFunction, vector<int> evaluationVector);
bool twoOptLocalSearch(function<int(void)> evaluationFunction);
bool jobExchangeLocalSearch(function<int(void)> evaluationFunction, vector<int> evaluationVector);
bool swapLocalSearch(function<int(void)> evaluationFunction);

int singleRun(string inputFileName, ofstream& outputFile, int run);

int machineCount, toolCount, jobCount, best; /* quantity of machines, tools, jobs and current best solution value */
vector<vector<int>> npmJobAssignement, bestSolution; /* current jobs and tools assigned to each machine and best solution assignements */
vector<vector<int>> toolsRequirements; /* jobs and tools requirements */
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
    
    fill(npmCurrentMagazines[machineIndex].begin(), npmCurrentMagazines[machineIndex].end(), -1);

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

int toolSwitchesEvaluation() { 
    for (int machineIndex = 0 ; machineIndex < machineCount ; machineIndex++) {
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

    return accumulate(npmCurrentToolSwitches.begin(),npmCurrentToolSwitches.end(),0);
}

int flowtimeEvaluation() {
    for (int machineIndex = 0 ; machineIndex < machineCount ; machineIndex++) {
        npmCurrentFlowTime[machineIndex] = 0;
        for(int i = 0 ; i < (int)npmJobAssignement[machineIndex].size() ; i++)
            npmCurrentFlowTime[machineIndex] += npmJobTime[machineIndex][npmJobAssignement[machineIndex][i]];
    }

    return accumulate(npmCurrentFlowTime.begin(),npmCurrentFlowTime.end(),0);
}

int makespanEvaluation() {
    int highestMakespan = 0;

    toolSwitchesEvaluation();
    flowtimeEvaluation();
    for (int machineIndex = 0 ; machineIndex < machineCount ; machineIndex++) {
        npmCurrentMakespan[machineIndex] = npmCurrentFlowTime[machineIndex] + npmCurrentToolSwitches[machineIndex] * npmSwitchCost[machineIndex];
        if (npmCurrentMakespan[machineIndex] > highestMakespan)
            highestMakespan = npmCurrentMakespan[machineIndex];
    }

    return highestMakespan;
}

void makeInitialRandomSolution() {
    vector<int> r(jobCount);
    iota(r.begin(), r.end(), 0);

    mt19937 rng(random_device{}());
    shuffle(r.begin(), r.end(), rng);

    for (int i = 0 ; i < jobCount ; i++)
        npmJobAssignement[i % machineCount].push_back(r[i]);
}

bool jobInsertionLocalSearch(function<int(void)> evaluationFunction, vector<int> evaluationVector) {
    int currentBest = evaluationFunction();
    int criticalMachine = distance(evaluationVector.begin(),max_element(evaluationVector.begin(), evaluationVector.end()));

    for (int i = 0 ; i < (int)npmJobAssignement[criticalMachine].size() ; i++) {
        int jobIndex = npmJobAssignement[criticalMachine][i];
        npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + i);

        for(int j = 0 ; j < machineCount ; j++) {
            if (j == criticalMachine)
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

bool jobExchangeLocalSearch(function<int(void)> evaluationFunction, vector<int> evaluationVector) {
    int currentBest = evaluationFunction();
    int criticalMachine = distance(evaluationVector.begin(),max_element(evaluationVector.begin(), evaluationVector.end()));
    int briefestMachine = distance(evaluationVector.begin(),min_element(evaluationVector.begin(), evaluationVector.end()));

    for(int i = 0 ; i < (int)npmJobAssignement[criticalMachine].size() ; i++) {
        for (int j = 0 ; j < (int)npmJobAssignement[briefestMachine].size() ; j++) {
            swap(npmJobAssignement[criticalMachine][i], npmJobAssignement[briefestMachine][j]);
            if(evaluationFunction() < currentBest)
                    return true;
            else
                swap(npmJobAssignement[criticalMachine][i], npmJobAssignement[briefestMachine][j]);
        }
    }

    return false;
}

bool twoOptLocalSearch(function<int(void)> evaluationFunction) {
    int currentBest = evaluationFunction();

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

bool swapLocalSearch(function<int(void)> evaluationFunction) {
    int currentBest = evaluationFunction();

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
    int j;
    vector<tuple<int,int>> oneBlocks;

    for(int i = 0 ; i < (int)npmJobAssignement[machineIndex].size() ; i++) {
        if(toolsRequirements[tool][i]) {
            j = i;
            while(toolsRequirements[tool][i]) { i++; }
            oneBlocks.push_back(tuple<int, int>(j, i - 1));
        }
    }
    
    return oneBlocks;
}

void multiStartRandom(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    for (int i = 0 ; i < 100 ; i++) {
        makeInitialRandomSolution();
        while(jobInsertionLocalSearch(evaluationFunction, evaluationVector)  || 
                jobExchangeLocalSearch(evaluationFunction, evaluationVector) || 
                twoOptLocalSearch(evaluationFunction) ||
                swapLocalSearch(evaluationFunction)) {}
        if (evaluationFunction() < best) {
            best = evaluationFunction();
            bestSolution = npmJobAssignement;
        }
        for(auto& v : npmJobAssignement) {
            v.clear();
        }
    }
}

int singleRun(string inputFileName, ofstream& outputFile, int run, int objective) {
    double runningTime;
    int result;
    readProblem(inputFileName);

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

    switch(objective) {
        case 1:
            multiStartRandom(toolSwitchesEvaluation, npmCurrentToolSwitches);
            break;
        case 2:
            multiStartRandom(makespanEvaluation, npmCurrentMakespan);
            break;
        case 3:
            multiStartRandom(flowtimeEvaluation, npmCurrentFlowTime);
            break;
    }

	high_resolution_clock::time_point t2 = high_resolution_clock::now(); 

  	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	runningTime =  time_span.count();											

	result = printSolution(inputFileName, runningTime, objective, run, cout);		
	result = printSolution(inputFileName, runningTime, objective, run, outputFile);		

	termination();

    return result;												
}

/* Reads the problem from a file specified by fileName */
void readProblem(string fileName) {
	ifstream fpIn(fileName);
    if(!fpIn)
        throw invalid_argument("ERROR : Instance " + fileName + "doesn't exist");
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
int printSolution(string inputFileName, double runningTime, int objective, int run, S &s) {
    npmJobAssignement = bestSolution;
    int totalToolSwitches = toolSwitchesEvaluation(), totalFlowtime = flowtimeEvaluation(), totalMakespan = makespanEvaluation();

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

    switch(objective) {
        case 1: return totalToolSwitches;
        case 2: return totalMakespan;
        case 3: return totalFlowtime;
        default: return INT_MAX;
    }
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
        for(int j = 0 ; j < (int)matrix[i].size() ; j++) {
            out << matrix[i][j] << " ";
        }
        out << endl;
    }
    out << endl;
    return out;
}

bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

#endif