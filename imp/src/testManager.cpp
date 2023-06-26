#include "global_vars.hpp"
#include "io.hpp"

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