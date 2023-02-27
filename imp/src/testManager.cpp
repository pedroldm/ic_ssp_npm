#include "solver.hpp"

int main(int argc, char* argv[])
{
    vector<string> arguments(argv + 1, argv + argc);

    srand(time(NULL));
    parseArguments(arguments);

    if(fpIndex.is_open()) {
        while(fpIndex>>inputFileName) {
            for(int i = 1 ; i <= runs ; i++) {
                singleRun(inputFileName, outputFile, i, objective);
            }
        }
    }
    else if(fileExists(instance))
        cout << singleRun(instance, outputFile, 0, objective);
    else
        throw invalid_argument("ERROR : Input file not well informed");
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
        else if (arguments[i]=="--enableLS1")
            enableLS1 = stoi(arguments[i + 1]);
        else if (arguments[i]=="--enableLS2")
            enableLS2 = stoi(arguments[i + 1]);
        else if (arguments[i]=="--enableLS3")
            enableLS3 = stoi(arguments[i + 1]);
        else if (arguments[i]=="--enableLS4")
            enableLS4 = stoi(arguments[i + 1]);
        else if (arguments[i]=="--lsOrder") {
            localSearch1 = (int)arguments[i + 1].at(0) - 48;
            localSearch2 = (int)arguments[i + 1].at(1) - 48;
            localSearch3 = (int)arguments[i + 1].at(2) - 48;
            localSearch4 = (int)arguments[i + 1].at(3) - 48;
            vnsDisturb = (int)arguments[i + 1].at(4) - 48;
        }
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
