#ifndef SOLVER_HPP
#define SOLVER_HPP
#define DEBUG 1
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
#include <chrono>
#include <unordered_set>
#include <climits>
#include <memory>

using namespace std;
using namespace std::chrono;

#define PRINT_MATRIX true

//Configuration of the datasets n, m, c, Tmin, Tmaz
int nMachines, nJobs, nTools, magazineCapacity;
vector<int> magazineCapacities;
vector<int> machineNJobs;
vector<int> swapCost;
vector<vector<int>> jobsTime;
vector<vector<int>> jobsAssignementsBinary;
vector<set<int>> machinesMagazine;

int makeSpam, flowTime, swaps;
int makeSpamSolution, flowTimeSolution, swapsSolution;
vector<vector<int>> jobsAssignementsSolution;

int totalTime, bestTime, nSwaps, indexJob, value;
vector<int> bestSolution; /* array with the best jobs assignements*/
vector<int> currentSolution; /* array with the best jobs assignements*/
set<int> magazine;
vector<int> jobs;
vector<set<pair<int, int>>> jobsAssignementsMatrix;

int runSolution(vector<int> solution, int machine);
int evaluation();
int GPCA(vector<int> solution, int machine);

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& arr);
template<typename T>
ostream& operator<<(ostream& out, const vector<vector<T>>& matrix);
ostream& operator<<(ostream& output, const set<int>& set);
ostream& operator<<(ostream& output, const vector<vector<int>>& matrix);
ostream& operator<<(ostream& output, const vector<set<int>>& matrix);
ostream& operator<<(ostream& output, const vector<set<pair<int, int>>>& matrix);

//Node class for use in graph search
class Node{
public:	
	Node()
	{
		degree = 0;
		visited = false;
		id = 0;
	}
	
	void setDegree(int i)
	{
		degree = i;
	}
	
	void setVisited(bool b)
	{
		visited = b;
	}
		
	void setId(int i)
	{
		id = i;
	}

	int getDegree()
	{
		return degree;
	}
	
	bool wasVisited()
	{
		return visited;
	}
		
	int getId()
	{
		return id;
	}

private:
	int degree;	 //stores the degree of the node
	int id;		 //stores the index of the node
	bool visited; //indicates whether the node has been reached by the BFS
};


/*
Initializes the structures, vectors and matrices used
*/
void initialization()
{
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	
	
	totalTime = 0;
	bestTime = INT_MAX;
	nSwaps = 0;
	indexJob = 0;
	magazineCapacities.resize(nMachines);
	swapCost.resize(nMachines);
	bestSolution.resize(nJobs);
	currentSolution.resize(nJobs);
	jobsTime.resize(nMachines);
	jobsAssignementsSolution.resize(nMachines);
	machinesMagazine.resize(nMachines);
	machineNJobs.resize(nMachines);


	jobsAssignementsBinary.resize(nTools);

	for(int i = 0 ; i < nTools ; i++) {
		jobsAssignementsBinary[i].resize(nJobs);

    }
	for(int i = 0 ; i < nMachines ; i++) {
		jobsTime[i].resize(nJobs);

    }	
}

/*
Reads the problem from a file specified by fileName
*/
void readProblem(string fileName)
{
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	ifstream fpIn(fileName);
	int get;

	if(!fpIn)
        throw invalid_argument("ERROR : Instance " + fileName + "doesn't exist");


	fpIn >> nMachines >> nJobs >> nTools;

	cout << nMachines << " " << nJobs << " " << nTools << endl;

	initialization();//initializes all structures, vectors and matrices

	for(int i = 0 ; i < nMachines ; i++) {
        fpIn >> get;
        magazineCapacities[i] = get;
    }

	cout << "Capacidade do magazine de cada maquina: " << magazineCapacities;

	for(int i = 0 ; i < nMachines ; i++) {
        fpIn >> get;
        swapCost[i] = get;
    }

	cout << "Custo de troca de ferramenta em cada máquina: " << swapCost; 

	for(int i = 0 ; i < nMachines ; i++) {
        for(int j = 0 ; j < nJobs ; j++) {
            fpIn >> get;
            jobsTime[i][j] = get;
        }
    }

	cout << "Tempo de processamento de cada tarefa em cada máquina:\n" << jobsTime; 

	for(int i = 0 ; i < nTools ; i++) {
        for(int j = 0 ; j < nJobs ; j++) {
            fpIn >> get;
            jobsAssignementsBinary[i][j] = get;
        }
    }

	cout << "Matriz binária: \n" << jobsAssignementsBinary;
}

void readSolution(string fileName)
{
	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	ifstream fpIn(fileName);
	int get;
	int nAssignements;
	int aux = 0;

	if(!fpIn)
        throw invalid_argument("ERROR : Instance " + fileName + "doesn't exist");

	cout << "Lendo solução:" << endl;

	for(int i = 0 ; i < nMachines; i++) {

		fpIn >> nAssignements;

		machineNJobs.push_back(nAssignements);

		cout << "Máquina -> " << i << " : numero de tarefas: " << nAssignements << endl;
		for(int j = 0; j < nAssignements; j++) {
			fpIn >> get;
			jobsAssignementsSolution[i].push_back(get);
		}
		cout << jobsAssignementsSolution[i];
	}

	fpIn >> swapsSolution >> makeSpamSolution >> flowTimeSolution;

	cout << "Número de trocas: " << swapsSolution << endl;
	cout << "Make span: " << makeSpamSolution << endl;
	cout << "Flow time: " << flowTimeSolution << endl;


	for(int i = 0; i < nMachines; i ++){
		aux = runSolution(jobsAssignementsSolution[i], i);
		if(aux > makeSpam)
			makeSpam = aux;
	}

	if(makeSpamSolution < makeSpam){
		printf("Make span inferior ao esperado: %d < %d !\n", makeSpamSolution, makeSpam);
	}
	else if(makeSpamSolution > makeSpam){
		printf("Make span superior ao esperado: %d > %d !\n", makeSpamSolution, makeSpam);
	}
	else{
		printf("Make span igual ao esperado: %d == %d !\n", makeSpamSolution, makeSpam);
	}

	if(flowTimeSolution < flowTime){
		printf("Flow time inferior ao esperado: %d < %d !\n", flowTimeSolution, flowTime);
	}
	else if(flowTimeSolution > flowTime){
		printf("Flow time superior ao esperado: %d > %d !\n", flowTimeSolution, flowTime);
	}
	else{
		printf("Flow time igual ao esperado: %d == %d !\n", flowTimeSolution, flowTime);
	}

	if(swapsSolution < swaps){
		printf("Número de trocas inferior ao esperado: %d < %d !\n", swapsSolution, swaps);
	}
	else if(swapsSolution > swaps){
		printf("Número de trocas superior ao esperado: %d > %d !\n", swapsSolution, swaps);
	}
	else{
		printf("Número de trocas igual ao esperado: %d == %d !\n", swapsSolution, swaps);
	}


}

int runSolution(vector<int> solution, int machine){

	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	int cost = 0;
	int makeSpanLocal = 0;
	int flowTimeLocal = 0;
	int timeLine = 0;
	pair<int, int> tool;
    set<pair<int, int>>::iterator it;
	vector<vector<int>> auxJobsAssignementsMatrix;

	indexJob = 0;

	cout << "Maquina: " << machine << endl;
	cout << "Tarefas: " << solution << endl;

	nJobs = solution.size();
   
    jobsAssignementsMatrix.resize(nJobs);

    auxJobsAssignementsMatrix.resize(nTools);

    for(int i = 0; i < nTools; i++){
        auxJobsAssignementsMatrix[i].resize(nJobs);

    }

    for(int i = 0; i < nTools; i++){
        if(jobsAssignementsBinary[i][solution[nJobs - 1]] == 1){

            auxJobsAssignementsMatrix[i][nJobs - 1] = nJobs - 1;

        }
        else{
            auxJobsAssignementsMatrix[i][nJobs - 1] = INT_MAX;

        }
    }



    for(int i = 0; i < nTools; i++){
        for(int j = nJobs - 2; j >= 0;j--){
            if(jobsAssignementsBinary[i][solution[j]] == 1){
                
                auxJobsAssignementsMatrix[i][j] = j;
            }
            else{
                auxJobsAssignementsMatrix[i][j] = auxJobsAssignementsMatrix[i][j + 1];

            }
        }
    }

	cout << auxJobsAssignementsMatrix;


    for(int i = 0; i < nTools; i++){
        for(int j = nJobs - 1; j >= 0;j--){

            tool.first = auxJobsAssignementsMatrix[i][j];
            tool.second = i + 1;
            jobsAssignementsMatrix[j].insert(tool);

        }
    }


	if(DEBUG)
		cout << "Jobs assignements matrix: \n" << jobsAssignementsMatrix;


    for(auto& v : auxJobsAssignementsMatrix) {
        v.clear();
    }
    auxJobsAssignementsMatrix.clear();
    machinesMagazine[machine].clear();

    indexJob = 0;
    for(auto& v : jobsAssignementsMatrix[indexJob]){

        if(machinesMagazine[machine].size() < magazineCapacities[machine]){
            machinesMagazine[machine].insert(v.second);

        }
        else{
            break;
        }
    }

	cout << "Magazine inicial: " << machinesMagazine[machine];
	timeLine += jobsTime[machine][solution[indexJob]];
	flowTime += timeLine;
	makeSpanLocal += timeLine;

	cout << "Time atual: " << timeLine << endl;
	cout << "Make span: " << makeSpanLocal << endl;
	cout << "Flow time: " << flowTime << endl;

	for(indexJob = 1; indexJob < nJobs; indexJob++){

		cost = GPCA(solution, machine);

		cout << "Magazine: " << machinesMagazine[machine];
		timeLine += cost + jobsTime[machine][solution[indexJob]];
		flowTime += timeLine;
		makeSpanLocal += jobsTime[machine][solution[indexJob]] + cost;
		cout << "Time atual: " << timeLine << endl;
		cout << "Make span: " << makeSpanLocal << endl;
		cout << "Flow time: " << flowTime << endl;
		if(DEBUG){
			cout << "job -> " << solution[indexJob] << endl;
			cout << machinesMagazine[machine];
			cout << "cost: " << cost << endl;
			cout << "fim job" << endl;

		} 

	}
	
	machinesMagazine[machine].clear();

	for(int i = 0; i < nJobs; i++){
		jobsAssignementsMatrix[i].clear();

	}
	jobsAssignementsMatrix.clear();
	
	return makeSpanLocal;
}

int GPCA(vector<int> solution, int machine){

	if(DEBUG)
		cout <<__FUNCTION__ << endl;

	int cost = 0;
	set<int> updateMagazine;
	set<int> uninstall;
	set<int> install;
	set<int>::iterator it;

	for(auto& v : jobsAssignementsMatrix[indexJob]){
		
		if(v.first != indexJob)
			break;

		if(updateMagazine.size() == magazineCapacities[machine]){
			printf("ERRO : A maquina %d não comporta o job %d!", machine, solution[indexJob]);
			exit(1);   
		}
		updateMagazine.insert(v.second);
       
    }

	for(auto& v : jobsAssignementsMatrix[indexJob]){

		if(v.first == INT_MAX)
			break;

		it = find (machinesMagazine[machine].begin(), machinesMagazine[machine].end(), v.second);

		if (it != machinesMagazine[machine].end()){

			if(updateMagazine.size() < magazineCapacities[machine]){

					updateMagazine.insert(v.second);
			}
			else{
				break;
			}
		}
		
		
	}
	

	if(updateMagazine.size() < magazineCapacities[machine]){
		for(auto& v : machinesMagazine[machine]){
			updateMagazine.insert(v);

			if(updateMagazine.size() >= magazineCapacities[machine]){
				break;
			}
		}
	}

	set_difference(updateMagazine.begin(), updateMagazine.end(), machinesMagazine[machine].begin(), machinesMagazine[machine].end(),  inserter(install, install.begin()));
	set_difference(machinesMagazine[machine].begin(), machinesMagazine[machine].end(), updateMagazine.begin(), updateMagazine.end(), inserter(uninstall, uninstall.begin()));

	if(DEBUG){
		cout << "Machine: " << machine << endl;
		cout << "Current magazine: " << machinesMagazine[machine];
		cout << "Update magazine: " << updateMagazine;
		cout << "Unstalling tools: " << uninstall;
		cout << "Installing tools: " << install; 

	}

	if(uninstall.size() == 0 || install.size() == 0){
		if(DEBUG)
			cout << "No swap: " << cost << endl;

		swap(updateMagazine, machinesMagazine[machine]);

		updateMagazine.clear();

		return cost;
	}

	cost = swapCost[machine] * uninstall.size();
	swaps += uninstall.size();

	if(DEBUG)
		cout << "Swap cost: " << cost << endl;

	swap(updateMagazine, machinesMagazine[machine]);

	updateMagazine.clear();

	return cost;
}

template <typename T>
ostream& operator<<(ostream& output, const vector<T>& vector) {
    //output << "[";

	for(int element : vector){
		output << element << " ";
	}
	output << endl;
    
    //output << "]\n";
    return output;
}

ostream& operator<<(ostream& output, const set<int>& set) {
    //output << "[";

	for(int element : set){
		output << element << " ";
	}
    output << endl;
    //output << "]\n";
    return output;
}

ostream& operator<<(ostream& output, const vector<vector<int>>& matrix) {

	for(vector<int> line : matrix){
		output << line;
	}
    
    return output;
}

ostream& operator<<(ostream& output, const vector<set<pair<int, int>>>& matrix) {

	for(set<pair<int, int>> line : matrix){
		output << "[";
		for(pair<int, int> elem : line)
			output << "( " << elem.first << ", " << elem.second << " ) ";

	    output << "]\n";

	}
    
    return output;
}

ostream& operator<<(ostream& output, const vector<set<int>>& matrix) {

	for(set<int> line : matrix){
		output << line;
	}
    
    return output;
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


/*
Builds the graph 
*/
void buildGraph()
{
}


/*
Evaluates the current solution
*/
int evaluation()
{
	cout <<__FUNCTION__ << endl;

	int value = 0;
	return value;						//returns the maximum number of open stacks
}

/*
 Pre-processing procedure
 */
void preProcessing()
{
}

/*
Terminates all data structures. 
*/
void termination()
{
	
	for(auto& v : jobsAssignementsBinary) {
        v.clear();
    }
	for(auto& v : jobsAssignementsMatrix) {
        v.clear();
    }

    magazine.clear();
    jobs.clear();
	bestSolution.clear();
	currentSolution.clear();

	jobsAssignementsBinary.clear();
	jobsAssignementsMatrix.clear();

	nJobs = 0;
	nTools = 0;
	magazineCapacity = 0;
	nSwaps = 0;
	totalTime = 0;
	//.clear in all data structures
	//=0 in all numeric variables
}

/*
Prints the solution information to the output file
*/
void printSolution(string inputFileName, int solutionValue, double time, int run)
{
	string outputFileName = "solutions/" + inputFileName;
	ofstream fpSolution(outputFileName);


	fpSolution << solutionValue << endl;

	
					//file that contains the information about the solution of a problem instance
	
	//print input file name/characteristics	
	//print algorithm parameters and settings
	//print solution value
	//print solution time
	//print preprocessing result
	//print solution structure
}

/*
Sets the lowerbound 
*/
void setLowerBound()
{
}

/*
Main procedure - Multi-Run version
Parameters:
solutionValue			stores the solution value obtained by the method
runningTime				stores the exeution time of the method
inputFileName			stores the name of the file containing the instance
run 					stores the run #
*/

void multiRun(int *solutionValue, double *runningTime, string inputFileName, int run)
{
	int FinalSolutionValue = INT_MAX;
	duration<double> time_span2;

														//reads the problem data

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	
	readProblem(inputFileName + "instancia.txt");
	readSolution(inputFileName + "solution.txt");
	 exit(0);																//builds the mosp graph


	high_resolution_clock::time_point t2 = high_resolution_clock::now(); 		//time taking
  	duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

	*solutionValue = totalTime;                             			//stores the solution value
	*runningTime =  time_span.count();											//stores the execution time

	printSolution(inputFileName, *solutionValue, *runningTime, run);	
				//prints the solution to the file

	termination();																//terminates all data structures
}

/*
Main procedure - single run
Parameters:
solutionValue			stores the solution value obtained by the method
runningTime				stores the exeution time of the method
inputFileName			stores the name of the file containing the instance
*/
void singleRun(int *solutionValue, double *runningTime, string inputFileName)
{
	int i, j;
	int FinalSolutionValue = INT_MAX;											//considering minimization problems		

	readProblem(inputFileName);													//reads the problem data
	
	high_resolution_clock::time_point t1 = high_resolution_clock::now();		//time taking
	
	setLowerBound();															//determines the lower bound
   	preProcessing();															//preprocessing procedure
   	buildGraph();																//builds the graph
    
    //solve the problem

	high_resolution_clock::time_point t2 = high_resolution_clock::now(); 		//time taking
  	duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

	*solutionValue = totalTime;                             			//stores the solution value
	*runningTime =  time_span.count();											//stores the execution time

	printSolution(inputFileName, *solutionValue, *runningTime, 1);					//prints the solution to the file

	termination();																//terminates all data structures
}
#endif
