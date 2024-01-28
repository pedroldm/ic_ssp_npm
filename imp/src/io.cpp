#include "io.hpp"
Instance singleRun(string inputFileName, ofstream& outputFile, int run, int objective) {
    double runningTime;
    readProblem(inputFileName);
    checkMachinesEligibility();

	t1 = high_resolution_clock::now();

    switch(objective) {
        case 1 : 
            ILSFull(GPCA, npmCurrentToolSwitches);
            break;
        case 2 :
            ILSCrit(makespanEvaluation, npmCurrentMakespan);
            break;
        case 3 :
            ILSFull(flowtimeEvaluation, npmCurrentFlowTime);
            break;
    }

  	duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() - t1);
	runningTime = time_span.count();											

    npmJobAssignement = bestSolution;
	printSolution(inputFileName, runningTime, objective, run, cout);		
	printSolution(inputFileName, runningTime, objective, run, outputFile);		

    Instance ins(bestSolution, GPCA(), makespanEvaluation(), flowtimeEvaluation(), improvements, runningTime, iterations, timeTracking);
    return ins;
}

void readProblem(string fileName) {
    int aux;
	ifstream fpIn(fileName);
    if(!fpIn)
        throw invalid_argument("ERROR : Instance " + fileName + " doesn't exist");

    fpIn >> machineCount >> jobCount >> toolCount;

    for(int i = 0 ; i < machineCount ; i++) {
        fpIn >> aux;
        npmMagazineCapacity.push_back(aux);
    }

    initialization();

    for(int i = 0 ; i < machineCount ; i++) {
        fpIn >> aux;
        npmSwitchCost.push_back(aux);
    }

    for(int i = 0 ; i < machineCount ; i++) {
        for(int j = 0 ; j < jobCount ; j++) {
            fpIn >> aux;
            npmJobTime[i].push_back(aux);
        }
    }
    
    for(int i = 0 ; i < toolCount ; i++) {
        for(int j = 0 ; j < jobCount ; j++) {
            fpIn >> aux;
            toolsRequirements[i].push_back(aux);
            if(aux)
                jobSets[j].insert(i);
        }
    }
}

void registerTimeTracking() {
    time_span = duration_cast<duration<double>>(high_resolution_clock::now() - t1);
        if(time_span.count() >= 300 && timeTracking[1] == INT_MAX) {
            timeTracking[1] = best;
        }
        else if(time_span.count() >= 900 && timeTracking[2] == INT_MAX) {
            timeTracking[2] = best;
        }
        else if(time_span.count() >= 1800 && timeTracking[3] == INT_MAX) {
            timeTracking[3] = best;
        }
        else if(time_span.count() >= 2700 && timeTracking[4] == INT_MAX) {
            timeTracking[4] = best;
        }
        else if(time_span.count() >= 3600 && timeTracking[5] == INT_MAX) {
            timeTracking[5] = best;
        }
}

void initialization() {
    toolsRequirements.resize(toolCount);
    npmJobTime.resize(machineCount);
    npmJobAssignement.resize(machineCount);
    randomTools.resize(ceil(oneBlockPercentage * toolCount));
    iota(randomTools.begin(), randomTools.end(), 0);
    similarityMatrix.resize(jobCount);
    for(int i = 0 ; i < jobCount ; i++) {
        similarityMatrix[i].resize(jobCount);
    }
    timeTracking = {INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX};

    npmCurrentMagazines.resize(machineCount);
    npmToolsNeedDistance.resize(machineCount);
    for(int i = 0 ; i < machineCount ; i++) {
        npmCurrentMagazines[i].resize(npmMagazineCapacity[i]);
        npmToolsNeedDistance[i].resize(npmMagazineCapacity[i]);
    }
    
    for(int i = 0 ; i < machineCount ; i++) {
        npmCurrentToolSwitches.push_back(INT_MAX);
        npmCurrentMakespan.push_back(INT_MAX);
        npmCurrentFlowTime.push_back(INT_MAX);
    }

    jobEligibility.resize(machineCount);
    for(int i = 0 ; i < machineCount ; i++)
        jobEligibility[i].resize(jobCount);

    jobSets.resize(jobCount);
    toolsDistancesGPCA.resize(toolCount);
    for(int i = 0 ; i < toolCount ; i++)
        toolsDistancesGPCA[i].resize(jobCount);
    magazines.resize(2);

    best = INT_MAX;
    iterations = 0;
}

void termination() {
    for(auto& v : npmJobAssignement) {
        v.clear();
    }
    for(auto& v : toolsRequirements) {
        v.clear();
    }
    for(auto& v : npmJobTime) {
        v.clear();
    }
    for(auto & v : npmCurrentMagazines) {
        v.clear();
    }
    for(auto & v : npmToolsNeedDistance) {
        v.clear();
    }
    for(auto & v : jobSets) {
        v.clear();
    }
    for(auto & v : bestSolution) {
        v.clear();
    }
    for(auto & v : jobEligibility) {
        v.clear();
    }
    for(auto & v : similarityMatrix) {
        v.clear();
    }

    similarityMatrix.clear();
    npmJobAssignement.clear();
    toolsRequirements.clear();
    npmJobTime.clear();
    npmCurrentMagazines.clear();
    npmToolsNeedDistance.clear();
    improvements.clear();

    jobEligibility.clear();
    npmMagazineCapacity.clear();
    npmSwitchCost.clear();
    npmCurrentToolSwitches.clear();
    npmCurrentMakespan.clear();
    npmCurrentFlowTime.clear();
    randomTools.clear();
    bestSolution.clear();

    machineCount = 0;
    toolCount = 0;
    jobCount = 0;
}

template <typename S>
void printSolution(string inputFileName, double runningTime, int objective, int run, S &s) {
    int totalToolSwitches = GPCA(), makespan = makespanEvaluation(), flowtime = flowtimeEvaluation();

    s << "RUN : " << run << " - " << inputFileName << "\n\n";
    for(int i = 0 ; i < machineCount ; i++) {
        s << "- MACHINE [" << i << "]" << "\n\n";

        if(PRINT_MATRIX) {
            for(int j = 0 ; j < toolCount ; j++) {
                for(int k : bestSolution[i]) {
                    s << toolsRequirements[j][k] << " ";
                }
                s << endl;
            }
            s << endl;
        }

        s << "JOBS " << bestSolution[i];
        s << "TOOL SWITCHES : " << npmCurrentToolSwitches[i] << endl;
        s << "MAKESPAN : " << npmCurrentMakespan[i] << "\n\n";
    }

    s << "# -------------------------- #" << endl << endl;
    s << "--- TOTAL TOOL SWITCHES : " << totalToolSwitches << endl;
    s << "--- HIGHEST MAKESPAN : " << makespan << endl;
    s << "--- TOTAL FLOWTIME : " << flowtime << endl;
    s << "--- COMPLETED ITERATIONS : " << --iterations << endl;
    s << "--- RUNNING TIME : " << runningTime << endl;
    s << "--- IMPROVEMENT HISTORY : " << improvements;
    s << "--- TIME TRACKING : " << timeTracking << endl;
}

void printSummary(string input) {
    cout << "Input: " << input << endl;
    cout << "Objective: " << objective << endl;
    cout << "# -------------------------------- #" << endl;
    cout << "Average Bests TS: " << (int)round(summary.getBestsMean(1)) << " (" << fixed << setprecision(2) << summary.getBestsMean(1) << ")" << endl;
    cout << "Mean TS: " << (int)round(summary.getGeneralMean(1)) << " (" << fixed << setprecision(2) << summary.getGeneralMean(1) << ")" << endl;
    cout << "Average Bests Makespan: " << (int)round(summary.getBestsMean(2)) << " (" << fixed << setprecision(2) << summary.getBestsMean(2) << ")" << endl;
    cout << "Mean Makespan: " << (int)round(summary.getGeneralMean(2)) << " (" << fixed << setprecision(2) << summary.getGeneralMean(2) << ")" << endl;
    cout << "Average Bests Flowtime: " << (int)round(summary.getBestsMean(3)) << " (" << fixed << setprecision(2) << summary.getBestsMean(3) << ")" << endl;
    cout << "Mean Flowtime: " << (int)round(summary.getGeneralMean(3)) << " (" << fixed << setprecision(2) << summary.getGeneralMean(3) << ")" << endl;
    cout << "# -------------------------------- #" << endl;
    cout << "Mean Standard Deviation TS: " << fixed << setprecision(2) << summary.getMeanStandardDeviation(1) << endl;
    cout << "Mean Standard Deviation Makespan: " << fixed << setprecision(2) << summary.getMeanStandardDeviation(2) << endl;
    cout << "Mean Standard Deviation Flowtime: " << fixed << setprecision(2) << summary.getMeanStandardDeviation(3) << endl;
    cout << "# -------------------------------- #" << endl;
    cout << "Mean Execution Time : " << fixed << setprecision(2) << summary.getMeanExecutionTime() << endl;
    cout << "Trajectory : " << summary.getTrajectoryData();
    cout << "LS Improvements : " << summary.localSearchImprovements;
    if(outputFile.is_open()) {
        outputFile << "--------- SUMMARY ---------" << endl << endl;
        outputFile << "Input: " << input << endl;
        outputFile << "Objective: " << objective << endl;
        outputFile << "Average TS: " << (int)round(summary.getBestsMean(1)) << " (" << fixed << setprecision(2) << summary.getBestsMean(1) << ")" << endl;
        outputFile << "Average Makespan: " << (int)round(summary.getBestsMean(2)) << " (" << fixed << setprecision(2) << summary.getBestsMean(2) << ")" << endl;
        outputFile << "Average Flowtime: " << (int)round(summary.getBestsMean(3)) << " (" << fixed << setprecision(2) << summary.getBestsMean(3) << ")" << endl;
        outputFile << "Mean Standard Deviation TS: " << fixed << setprecision(2) << summary.getMeanStandardDeviation(1) << endl;
        outputFile << "Mean Standard Deviation Makespan: " << fixed << setprecision(2) << summary.getMeanStandardDeviation(2) << endl;
        outputFile << "Mean Standard Deviation Flowtime: " << fixed << setprecision(2) << summary.getMeanStandardDeviation(3) << endl;
        outputFile << "Trajectory : " << summary.getTrajectoryData();
        outputFile << "LS Improvements : " << summary.localSearchImprovements;
    }
}

void parseArguments(vector<string> arguments) {
    for(int i = 0 ; i < (int)arguments.size() ; i++) {
        if (arguments[i]=="--objective") {
            objective = stoi(arguments[i + 1]);
            if (objective < 1 || objective > 3)
                throw invalid_argument("ERROR : Objective not well informed. Must be 1 (TS), 2 (makespan) or 3 (flowtime)");
        }
        else if (arguments[i]=="--runs")
            runs = stoi(arguments[i + 1]);
        else if (arguments[i]=="--iterations")
            maxIterations = stoi(arguments[i + 1]);
        else if (arguments[i]=="--instance")
            instance = (arguments[i + 1]);
        else if (arguments[i]=="--similarity_percentage")
            similarityPercentage = stof(arguments[i + 1]);
        else if (arguments[i]=="--critic_job_percentage")
            criticJobPercentage = stof(arguments[i + 1]);
        else if (arguments[i]=="--disturb_size")
            disturbSize = stof(arguments[i + 1]);
        else if (arguments[i]=="--one_block_percentage")
            oneBlockPercentage = stof(arguments[i + 1]);
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

template <typename T>
ostream& operator<<(ostream& os, const vector<tuple<T, T>>& vector) {
    os << "[" << vector.size() << "] - [";
    for(int i = 0; i < (int)vector.size(); ++i) {
        os << "(" << get<0>(vector[i]) << ", " << get<1>(vector[i]) << ")";
        if(i != (int)vector.size() - 1)
            os << " ; ";
    }
    os << "]\n";
    return os;
}

template<typename T>
ostream& operator<<(ostream& out, const vector<vector<T>>& matrix) {
    for(int i = 0 ; i < (int)matrix.size() ; i++) {
        for(int j = 0 ; j < (int)matrix[i].size() ; j++) {
            out << matrix[i][j] << " ";
        }
        out << endl;
    }
    return out;
}

template<typename T>
ostream& operator<<(ostream& out, const set<T>& m) {
    out << "[ ";
    for (auto& c : m) {
        out << c << ' ';
    }
    out << "]" << endl;
    return out;
}

bool fileExists(const std::string& filename) {
    struct stat buf;
    return (stat(filename.c_str(), &buf) != -1);
}