#include "solver.hpp"

int main(int argc, char* argv[])
{
    vector<string> arguments(argv + 1, argv + argc);
    int sum = 0;
    int avg = 0;

    srand(time(NULL));
    parseArguments(arguments);

    if(fpIndex.is_open()) {
        while(fpIndex>>inputFileName) {
            for(int i = 1 ; i <= runs ; i++) {
                avg++;
                sum += singleRun(inputFileName, outputFile, i, objective);
                termination();
            }
        }
    }
    else if(fileExists(instance))
        singleRun(instance, outputFile, 0, objective);
    else
        throw invalid_argument("ERROR : Input file not well informed");
    cout << endl << endl << "avg : " << (double)sum / (double)avg << endl;
    if(outputFile.is_open())
        outputFile << endl << endl << "avg : " << (double)sum / (double)avg << endl;
}

void parseArguments(vector<string> arguments) {
    for(int i = 0 ; i < (int)arguments.size() ; i++) {
        if (arguments[i]=="--objective")
            objective = stoi(arguments[i + 1]);
        else if (arguments[i]=="--runs")
            runs = stoi(arguments[i + 1]);
        else if (arguments[i]=="--iterations")
            maxIterations = stoi(arguments[i + 1]);
        else if (arguments[i]=="--instance")
            instance = (arguments[i + 1]);
        else if (arguments[i]=="--input") {
            inputFileName = arguments[i + 1];
            fpIndex.open(arguments[i + 1]);
            if(!fpIndex.is_open())
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
