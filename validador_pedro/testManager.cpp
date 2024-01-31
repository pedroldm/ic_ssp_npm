#include <cstdio>
using namespace std;
#include "solver.hpp"

#define RUNS 1						//how many runs for each instance?
void parseArguments(vector<string> arguments);
string inputFileName = "instance/";			//stores the name of the input file


int main(int argc, char* argv[])
{
	int solutionValue;					//stores the result 
	int i = 0, j;
	double runningTime;					//stores the running time


	//ifstream fpIndex("index.txt");
	ofstream fpOut("RESULTS_SUMMARY.txt");
	
	
		
	for(j=0; j<RUNS; j++)
	{
		printf("Run %d\n", j+1);

		multiRun(&solutionValue, &runningTime, inputFileName, j+1);
		printf("PROBLEM %d: %s \n%d\n", i, inputFileName.c_str(), solutionValue);
		fpOut<<inputFileName<<" "<<j+1<<" "<<solutionValue<<" "<<runningTime<<endl;
	}
}
