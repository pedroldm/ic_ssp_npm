#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <climits>
#include <set>
#include <memory>

using namespace std;

void parseArguments(vector<string> arguments);
void readInstance();
void checkSolution();
bool areDistinct();
bool allJobs();
int fillStartMagazine();
void fillToolsDistances();
void GPCA();
template<typename T>
ostream& operator<<(ostream& out, const vector<vector<T>>& matrix);
template <typename T>
ostream& operator<<(ostream& os, const vector<T>& vector);
template<typename T>
ostream& operator<<(ostream& out, const set<T>& m);

ifstream instancia, solucao;
int nroJobs, nroFerramentas, capMagazine, resultado, aux, in, out, costSum = 0, nroTrocas;
vector<int>instalacaoInicial;
vector<vector<int>>matrizDeTrocas, matrizBinaria, toolsDistancesGPCA, inseridasGPCA, removidasGPCA, inseridasSolucao, removidasSolucao;
vector<int>vetorSolucao;
vector<set<int>> magazines;
set<tuple<int, int>> dist;

int main (int argc, char* argv[]) {
    vector<string> arguments(argv + 1, argv + argc);

    parseArguments(arguments);
    readInstance();
    checkSolution();
    
    return 0;
}

void checkSolution() {
    for(int i = 0 ; i < nroJobs ; i++) {
        solucao >> aux;
        vetorSolucao.push_back(aux);
    }
    if(!areDistinct())
        throw invalid_argument("ERRO : A solução possui tarefas repetidas!");
    if(!allJobs())
        throw invalid_argument("ERRO : A solução não contempla todos os jobs!");
    GPCA();
    for(int i = 0 ; i < nroJobs ; i++) {
        solucao >> aux;
        for(int j = 0 ; j < aux ; j++) {
            solucao >> out;
            solucao >> in;
            if(find(inseridasGPCA[i].begin(), inseridasGPCA[i].end(), in) == inseridasGPCA[i].end() || find(removidasGPCA[i].begin(), removidasGPCA[i].begin(), out) == removidasGPCA[i].end())
                throw invalid_argument("ERRO : Troca de ferramentas inválida!");
            costSum += matrizDeTrocas[out][in];
            inseridasGPCA[i].erase(remove(inseridasGPCA[i].begin(), inseridasGPCA[i].end(), in), inseridasGPCA[i].end());
            removidasGPCA[i].erase(remove(removidasGPCA[i].begin(), removidasGPCA[i].end(), out), removidasGPCA[i].end());
        }
    }
    for(int i = 0 ; i < (int)inseridasGPCA.size() ; i++) {
        if(!inseridasGPCA[i].empty() || !removidasGPCA[i].empty())
            throw invalid_argument("ERRO : Há trocas pendentes para serem contabilizadas!");    
    }
    solucao >> aux;
    if (aux != costSum)
        throw invalid_argument("ERRO : A soma do custo não confere");
}

void readInstance() {
    instancia >> nroJobs >> nroFerramentas >> capMagazine;
    instancia >> aux; /* min ferramentas */
    instancia >> aux; /* max ferramentas */
    matrizDeTrocas.resize(nroFerramentas);
    matrizBinaria.resize(nroFerramentas);
    inseridasGPCA.resize(nroJobs);
    removidasGPCA.resize(nroJobs);
    for(int i = 0 ; i < nroFerramentas ; i++) {
        matrizBinaria[i].resize(nroJobs);
        fill(matrizBinaria[i].begin(), matrizBinaria[i].end(), 0);
    }
    for(int i = 0 ; i < nroFerramentas ; i++) {
        instancia >> aux;
        instalacaoInicial.push_back(aux);
    }
    for(int i = 0 ; i < nroFerramentas ; i++) {
        for(int j = 0 ; j < nroFerramentas ; j++) {
            instancia >> aux;
            matrizDeTrocas[i].push_back(aux);
        }
    }
    for(int i = 0 ; i < nroFerramentas ; i++) {
        for(int j = 0 ; j < nroJobs ; j++) {
            instancia >> aux;
            if(aux > 0)
                matrizBinaria[aux-1][j] = 1;
        }
    }
    toolsDistancesGPCA.resize(nroFerramentas);
    for(int i = 0 ; i < nroFerramentas ; i++)
        toolsDistancesGPCA[i].resize(nroJobs);
    magazines.resize(2);
}

void GPCA() {
    fillToolsDistances();
    int startSwitch = fillStartMagazine();
    int empty;

    for(int i = startSwitch ; i < nroJobs ; i++) {
        for(int j = 0 ; j < nroFerramentas ; j++) {
            if(matrizBinaria[j][vetorSolucao[i]])
                magazines[1].insert(j);
        }

        for(auto t : magazines[0]) {
            if(!matrizBinaria[t][vetorSolucao[i]]) {
                dist.insert(make_tuple(toolsDistancesGPCA[t][i], t));
            }
        }

        empty = capMagazine - magazines[1].size();
        if(empty) {
            for(auto t : dist) {
                magazines[1].insert(get<1>(t));
                if(!--empty)
                    break;
            }
        }

        set_difference(magazines[0].begin(), magazines[0].end(), magazines[1].begin(), magazines[1].end(), inserter(removidasGPCA[i], removidasGPCA[i].begin()));
        set_difference(magazines[1].begin(), magazines[1].end(), magazines[0].begin(), magazines[0].end(), inserter(inseridasGPCA[i], inseridasGPCA[i].begin()));

        magazines[0] = magazines[1];
        magazines[1].clear();
        dist.clear();
    }
    magazines[0].clear();
}

int fillStartMagazine() {
    for(int i = 0 ; i < nroJobs ; i++) {
        for(int j = 0 ; j < nroFerramentas ; j++) {
            if(magazines[0].size() < capMagazine) {
                if(matrizBinaria[j][vetorSolucao[i]]) {
                    magazines[0].insert(j);
                    costSum += instalacaoInicial[j];
                }
            }
            else
                return i;
        }
    }
    return INT_MAX;
}

void fillToolsDistances() {
    for(int i = 0 ; i < nroFerramentas ; i++) {
        if(matrizBinaria[i][vetorSolucao[nroJobs - 1]])
            toolsDistancesGPCA[i][nroJobs - 1] = nroJobs - 1;
        else
            toolsDistancesGPCA[i][nroJobs - 1] = INT_MAX;
    }

    for(int i = nroJobs - 2 ; i >= 0 ; i--) {
        for(int j = 0 ; j < nroFerramentas ; j++) {
            if(matrizBinaria[j][vetorSolucao[i]])
                toolsDistancesGPCA[j][i] = i;
            else 
                toolsDistancesGPCA[j][i] = toolsDistancesGPCA[j][i + 1];
        }
    }
}

void parseArguments(vector<string> arguments) {
    for(int i = 0 ; i < (int)arguments.size() ; i++) {
        if (arguments[i]=="--instancia") {
            instancia.open(arguments[i + 1]);
            if(!instancia.is_open())
                throw invalid_argument("ERROR : Input file doesn't exist");
        }
        else if (arguments[i]=="--solucao") {
            solucao.open(arguments[i + 1]);
            if(!solucao.is_open())
                throw invalid_argument("ERROR : Input file doesn't exist");
        }
    }
}

bool allJobs() {
    for(int i = 0 ; i < nroJobs ; i++)
        if(find(vetorSolucao.begin(), vetorSolucao.end(), i) == vetorSolucao.end())
            return false;
    return true;
}

bool areDistinct() {
    unordered_set<int> s;
    for (int i = 0; i < vetorSolucao.size(); i++) {
        s.insert(vetorSolucao[i]);
    }
 
    return (s.size() == vetorSolucao.size());
}

template<typename T>
ostream& operator<<(ostream& out, const set<T>& m) {
    out << "[" << (int)m.size() << "] - ";
    out << "[ ";
    for (auto& c : m) {
        out << c << ' ';
    }
    out << "]" << endl;
    return out;
}

template<typename T>
ostream& operator<<(ostream& out, const vector<vector<T>>& matrix) {
    for(int i = 0 ; i < (int)matrix.size() ; i++) {
        out << "[" << (int)matrix[i].size() << "] - "; 
        for(int j = 0 ; j < (int)matrix[i].size() ; j++) {
            out << matrix[i][j] << " ";
        }
        out << endl;
    }
    out << endl;
    return out;
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& vector) {
    os << "[" << vector.size() << "] - [";
    for(int i = 0; i < (int)vector.size(); ++i) {
        os << vector[i];
        if(i != (int)vector.size() - 1)
            os << ", ";
    }
    os << "]\n";
    return os;
}