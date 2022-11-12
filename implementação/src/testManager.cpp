#include "solver.hpp"

int main(int argc, char* argv[])
{
    if (argc < 4) throw invalid_argument("ERRO : Arquivos de I/O e nº de runs não informados"); 

	string inputFileName = argv[2];
	ifstream fpIndex(argv[1]);
    ofstream outputFile(argv[2]);
	
	while(fpIndex>>inputFileName)
	{
		for(int i = 1 ; i <= atoi(argv[3]) ; i++)
			singleRun(inputFileName, outputFile, i);
	}
}
