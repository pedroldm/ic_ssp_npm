#include "solver.hpp"

string inputFileName, ans;
ifstream fpIndex;
ofstream outputFile;
int objectives[] = {1 /*TS*/, 2/*Makespan*/, 3/*Flowtime*/};

int main(int argc, char* argv[])
{
    vector<string> arguments(argv + 1, argv + argc);

    parseArguments(arguments);

	while(fpIndex>>inputFileName) {
		for(int i = 1 ; i <= runs ; i++) {
			singleRun(inputFileName, outputFile, i, objective);
        }
	}
}

void parseArguments(vector<string> arguments) {
    for(int i = 0 ; i < (int)arguments.size() ; i++) {
        if (arguments[i]=="--objective")
            objective = stoi(arguments[i + 1]);
        else if (arguments[i]=="--runs")
            runs = stoi(arguments[i + 1]);
        else if (arguments[i]=="--iterations")
            maxIterations = stoi(arguments[i + 1]);
        else if (arguments[i]=="--localSearch1")
            localSearch1 = stoi(arguments[i + 1]);
        else if (arguments[i]=="--localSearch2")
            localSearch2 = stoi(arguments[i + 1]);
        else if (arguments[i]=="--localSearch3")
            localSearch3 = stoi(arguments[i + 1]);
        else if (arguments[i]=="--localSearch4")
            localSearch4 = stoi(arguments[i + 1]);
        else if (arguments[i]=="vnsDisturb")
            vnsDisturb = stoi(arguments[i + 1]);
        else if (arguments[i]=="--input") {
            inputFileName = arguments[i + 1];
            fpIndex.open(arguments[i + 1]);
            if(!fpIndex)
                throw invalid_argument("ERROR : Input file doesn't exist");
        }
        else if (arguments[i]=="--output") {
            if(fileExists(arguments[i + 1])) {
                cout << "Output file \"" << arguments[i + 1] << "\" already exists. Overwrite? ";
                cin >> ans;
                if(ans == "n" || ans == "no") 
                    exit(0);
            }
            outputFile.open(arguments[i + 1]);
        }
    }
}
