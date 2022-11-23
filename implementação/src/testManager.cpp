#include "solver.hpp"

int main(int argc, char* argv[])
{
    if (argc < 5) throw invalid_argument("ERROR : I/O files, runs and objective not well informed");

	string inputFileName;
	ifstream fpIndex(argv[1]);
    if(!fpIndex)
        throw invalid_argument("ERROR : Input file doesn't exist");
    if(fileExists(argv[2])) {
        cout << "Output file \"" << argv[2] << "\" already exists. Overwrite? ";
        string ans;
        cin >> ans;
        if(ans == "n" || ans == "no") 
            exit(0);
    }
    ofstream outputFile(argv[2]);

    int runs = atoi(argv[4]), objective = atoi(argv[3]);
    int objectives[] = {1, 2, 3};
    if(!runs || !objective || find(begin(objectives), end(objectives), objective) == end(objectives)) {
        throw invalid_argument("ERROR : Invalid objective or number of runs");
    }
	
	while(fpIndex>>inputFileName)
	{
		for(int i = 1 ; i <= runs ; i++) {
			singleRun(inputFileName, outputFile, i, objective);
        }
	}
}
