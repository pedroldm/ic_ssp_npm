#include "solver.hpp"

int main(int argc, char* argv[])
{
    if (argc < 4) throw invalid_argument("ERRO : Arquivos de I/O e nº de runs não informados"); 

    int runs = atoi(argv[3]);

	string inputFileName, outputFileName = argv[2];
	ifstream fpIndex(argv[1]);
	
	while(fpIndex>>inputFileName)
	{	
		for(int i = 1 ; i <= runs ; i++)
		{
			cout << endl << "RUN : " << i << " - " << inputFileName << "\n\n";	
			singleRun(inputFileName, outputFileName, i);
		}
	}
}
