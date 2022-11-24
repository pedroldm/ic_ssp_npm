#include "solver.hpp"

string inputFileName;
ifstream fpIndex;
ofstream outputFile;
int runs, objective;
int objectives[] = {1 /*TS*/, 2/*Makespan*/, 3/*Flowtime*/};

int main(int argc, char* argv[])
{
    validateArguments(argc, argv);

	ifstream fpIndex(argv[1]);
    if(!fpIndex)
        throw invalid_argument("ERROR : Input file doesn't exist");
    ofstream outputFile(argv[2]);
	
	while(fpIndex>>inputFileName) {
		for(int i = 1 ; i <= runs ; i++) {
			singleRun(inputFileName, outputFile, i, objective);
        }
	}
}

void validateArguments (int argc, char* argv[]) {
    if (argc < 5) 
        throw invalid_argument("ERROR : I/O files, runs and objective not well informed");
    if(fileExists(argv[2])) {
        cout << "Output file \"" << argv[2] << "\" already exists. Overwrite? ";
        string ans;
        cin >> ans;
        if(ans == "n" || ans == "no") 
            exit(0);
    }
    runs = atoi(argv[4]);
    objective = atoi(argv[3]);
    if(!runs || !objective || find(begin(objectives), end(objectives), objective) == end(objectives)) {
        throw invalid_argument("ERROR : Invalid objective or number of runs");
    }
}