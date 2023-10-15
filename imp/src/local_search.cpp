#include "local_search.hpp"

bool VNDFull(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    int k = 1;
    while (k != 6) {
        time_span = duration_cast<duration<double>>(high_resolution_clock::now() - t1);
        if(time_span.count() >= maxTime)
            return false;
        switch(k) {
            case 1 :
                if(jobExchangeLocalSearchFull(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[0]++;
                    k = 1;
                }
                else
                    k++;
                break;
            case 2 :
                if(jobInsertionLocalSearchFull(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[1]++;
                    k = 1;
                }
                else
                    k++;
                break;
            case 3 : 
                if(swapLocalSearch(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[2]++;
                    k = 1;
                }
                else
                    k++;
                break;
            case 4 :
                if(twoOptLocalSearch(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[3]++;
                    k = 1;
                }
                else
                    k++;
                break;
            case 5 :
                if(oneBlockLocalSearch(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[3]++;
                    k = 1;
                }
                else
                    k++;
                break;
        }
    }
    return true;
}

bool VNDCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    int k = 1;
    while (k != 6) {
        time_span = duration_cast<duration<double>>(high_resolution_clock::now() - t1);
        if(time_span.count() >= maxTime)
            return false;
        currentBest = evaluationFunction();
        switch(k) {
            case 1 :
                if(jobExchangeLocalSearchCrit(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[0]++;
                    k = 1;
                }
                else
                    k++;
                break;
            case 2 :
                if(jobInsertionLocalSearchCrit(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[1]++;
                    k = 1;
                }
                else
                    k++;
                break;
            case 3 : 
                if(swapLocalSearch(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[2]++;
                    k = 1;
                }
                else
                    k++;
                break;
            case 4 :
                if(twoOptLocalSearch(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[3]++;
                    k = 1;
                }
                else
                    k++;
                break;
            case 5 :
                if(oneBlockLocalSearchCrit(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[3]++;
                    k = 1;
                }
                else
                    k++;
                break;
        }
    }
    return true;
}

bool jobInsertionLocalSearchFull(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest) {
    for(int l = 0 ; l < machineCount ; l++) {
        for (int i = 0 ; i < (int)npmJobAssignement[l].size() ; i++) {
            int jobIndex = npmJobAssignement[l][i];
            npmJobAssignement[l].erase(npmJobAssignement[l].begin() + i);
            beforeSwap1 = evaluationVector[l];
            mI = {l};
            evaluationFunction();

            for(int j = 0 ; j < machineCount ; j++) {
                if (j == l || !jobEligibility[j][jobIndex])
                    continue;
                beforeSwap2 = evaluationVector[j];
                npmJobAssignement[j].insert(npmJobAssignement[j].begin(), jobIndex);
                mI = {j};
                if(evaluationFunction() < currentBest) {
                    mI.clear();
                    return true;
                }
                for(int k = 1 ; k < (int)npmJobAssignement[j].size() ; k++) {
                    swap(npmJobAssignement[j][k - 1], npmJobAssignement[j][k]);
                    if(evaluationFunction() < currentBest) {
                        mI.clear();
                        return true;
                    }
                }
                npmJobAssignement[j].pop_back();
                evaluationVector[j] = beforeSwap2;
            }
            
            npmJobAssignement[l].insert(npmJobAssignement[l].begin() + i, jobIndex);
            evaluationVector[l] = beforeSwap1;
        }
    }

    mI.clear();
    return false;
}

bool jobInsertionLocalSearchCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest) {
    int criticalMachine = distance(evaluationVector.begin(),max_element(evaluationVector.begin(), evaluationVector.end()));

    for (int i = 0 ; i < (int)npmJobAssignement[criticalMachine].size() ; i++) {
        int jobIndex = npmJobAssignement[criticalMachine][i];
        npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + i);
        beforeSwap1 = evaluationVector[criticalMachine];
        mI = {criticalMachine};
        evaluationFunction();

        for(int j = 0 ; j < machineCount ; j++) {
            if (j == criticalMachine || !jobEligibility[j][jobIndex])
                continue;
            beforeSwap2 = evaluationVector[j];
            npmJobAssignement[j].insert(npmJobAssignement[j].begin(), jobIndex);
            mI = {j};
            if(evaluationFunction() < currentBest) {
                mI.clear();
                return true;
            }
            for(int k = 1 ; k < (int)npmJobAssignement[j].size() ; k++) {
                swap(npmJobAssignement[j][k - 1], npmJobAssignement[j][k]);
                if(evaluationFunction() < currentBest) {
                    mI.clear();
                    return true;
                }
            }
            npmJobAssignement[j].pop_back();
            evaluationVector[j] = beforeSwap2;
        }
        
        npmJobAssignement[criticalMachine].insert(npmJobAssignement[criticalMachine].begin() + i, jobIndex);
        evaluationVector[criticalMachine] = beforeSwap1;
    }

    mI.clear();
    return false;
}

bool jobExchangeLocalSearchCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest) {
    int criticalMachine = distance(evaluationVector.begin(),max_element(evaluationVector.begin(), evaluationVector.end()));

    for(int i = 0 ; i < (int)npmJobAssignement[criticalMachine].size() ; i++) {
        for(int l = 0 ; l < machineCount; l++) {
            mI = {criticalMachine, l};
            for (int j = 0 ; j < (int)npmJobAssignement[l].size() ; j++) {
                if (jobEligibility[criticalMachine][npmJobAssignement[l][j]] && jobEligibility[l][npmJobAssignement[criticalMachine][i]]) {
                    beforeSwap1 = evaluationVector[criticalMachine];
                    beforeSwap2 = evaluationVector[l];
                    swap(npmJobAssignement[criticalMachine][i], npmJobAssignement[l][j]);
                    if(evaluationFunction() < currentBest) {
                        mI.clear();
                        return true;
                    }
                    else {
                        swap(npmJobAssignement[criticalMachine][i], npmJobAssignement[l][j]);
                        evaluationVector[criticalMachine] = beforeSwap1;
                        evaluationVector[l] = beforeSwap2;
                    }
                }
            }
        }
    }

    mI.clear();
    return false;
}

bool jobExchangeLocalSearchFull(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest) {
    for (int l = 0 ; l < machineCount - 1; l++) {
        for (int k = l + 1 ; k < machineCount ; k++) {
            mI = {l, k};
            for(int i = 0 ; i < (int)npmJobAssignement[l].size() ; i++) {
                for (int j = 0 ; j < (int)npmJobAssignement[k].size() ; j++) {
                    if (jobEligibility[l][npmJobAssignement[k][j]] && jobEligibility[k][npmJobAssignement[l][i]]) {
                        beforeSwap1 = evaluationVector[l];
                        beforeSwap2 = evaluationVector[k];
                        swap(npmJobAssignement[l][i], npmJobAssignement[k][j]);
                        if(evaluationFunction() < currentBest) {
                            mI.clear();
                            return true;
                        }
                        else {
                            swap(npmJobAssignement[l][i], npmJobAssignement[k][j]);
                            evaluationVector[l] = beforeSwap1;
                            evaluationVector[k] = beforeSwap2;
                        }
                    }
                }
            }
        }
    }

    mI.clear();
    return false;
}

bool twoOptLocalSearch(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest) {
    for(int i = 0 ; i < machineCount ; i++) {
        mI = {i};
        for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
            for (int k = j + 1 ; k < (int)npmJobAssignement[i].size() ; k++) {
                beforeSwap1 = evaluationVector[i];
                reverse(npmJobAssignement[i].begin() + j, npmJobAssignement[i].end() - k + 1);
                if(evaluationFunction() < currentBest) {
                    mI.clear();
                    return true;
                }
                else {
                    reverse(npmJobAssignement[i].begin() + j, npmJobAssignement[i].end() - k + 1);
                    evaluationVector[i] = beforeSwap1;
                }
            }
        }
    }

    mI.clear();
    return false;
}

bool swapLocalSearch(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest) {
    for(int i = 0 ; i < machineCount ; i++) {
        for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
            for (int k = j + 1 ; k < (int)npmJobAssignement[i].size() ; k++) {
                beforeSwap1 = evaluationVector[i];
                swap(npmJobAssignement[i][j], npmJobAssignement[i][k]);
                mI = {i};
                if(evaluationFunction() < currentBest) {
                    mI.clear();
                    return true;
                }
                else {
                    swap(npmJobAssignement[i][j], npmJobAssignement[i][k]);
                    evaluationVector[i] = beforeSwap1;
                }
            }
        }
    }

    mI.clear();
    return false;
}

vector<tuple<int,int>> findOneBlocks(int machineIndex, int tool) {
    vector<tuple<int,int>> oneBlocks;
    int blockStart;

    for(int i = 0 ; i < (int)npmJobAssignement[machineIndex].size() ; i++) {
        if(toolsRequirements[tool][npmJobAssignement[machineIndex][i]]) {
            blockStart = i;
            while(i < (int)npmJobAssignement[machineIndex].size() && toolsRequirements[tool][npmJobAssignement[machineIndex][i]]) { i++; }
            oneBlocks.push_back(make_tuple(blockStart, i - 1));
        }
    }

    return oneBlocks;
}

bool oneBlockLocalSearch(function<int(void)> &evaluationFunction, vector<int>& evaluationVector, int currentBest) {
    int job;

    mt19937 rng(random_device{}());
    shuffle(randomTools.begin(), randomTools.end(), rng);

    for(int i = 0 ; i < machineCount ; i++) {
        mI = {i};
        beforeSwap1 = evaluationVector[i];
        for(int j : randomTools) {
            oneBlocks = findOneBlocks(i, j);
            for(int remove = 0 ; remove < (int)oneBlocks.size() ; remove++) {
                for(int insert = 0 ; insert < (int)oneBlocks.size() ; insert++) {
                    if (remove == insert)
                        continue;
                    for(int m = get<0>(oneBlocks[remove]) ; m <= get<1>(oneBlocks[remove]) ; m++) {
                        job = npmJobAssignement[i][m];
                        if(insert > remove) {
                            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + get<0>(oneBlocks[insert]), job);
                            npmJobAssignement[i].erase(npmJobAssignement[i].begin() + m);
                            if(evaluationFunction() < currentBest) {
                                mI.clear();
                                return true;
                            }
                        
                            for(int n = get<0>(oneBlocks[insert]) - 1; n < get<1>(oneBlocks[insert]); n++) {
                                swap(npmJobAssignement[i][n], npmJobAssignement[i][n + 1]);
                                if(evaluationFunction() < currentBest) {
                                    mI.clear();
                                    return true;
                                }
                            }

                            npmJobAssignement[i].erase(npmJobAssignement[i].begin() + get<1>(oneBlocks[insert]));
                            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + m, job);
                        }
                        else {
                            npmJobAssignement[i].erase(npmJobAssignement[i].begin() + m);
                            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + get<0>(oneBlocks[insert]), job);
                            if(evaluationFunction() < currentBest) {
                                mI.clear();
                                return true;
                            }

                            for(int n = get<0>(oneBlocks[insert]); n <= get<1>(oneBlocks[insert]); n++) {
                                swap(npmJobAssignement[i][n], npmJobAssignement[i][n + 1]);
                                if(evaluationFunction() < currentBest) {
                                    mI.clear();
                                    return true;
                                }
                            }

                            npmJobAssignement[i].erase(npmJobAssignement[i].begin() + get<1>(oneBlocks[insert]) + 1);
                            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + m, job);
                        }
                    }
                }
            }
        }
        evaluationVector[i] = beforeSwap1;
    }

    mI.clear();
    return false;
}

bool oneBlockLocalSearchCrit(function<int(void)> &evaluationFunction, vector<int>& evaluationVector, int currentBest) {
    int job, criticalMachine = distance(evaluationVector.begin(),max_element(evaluationVector.begin(), evaluationVector.end()));

    mt19937 rng(random_device{}());
    shuffle(randomTools.begin(), randomTools.end(), rng);

    mI = {criticalMachine};
    beforeSwap1 = evaluationVector[criticalMachine];
    for(int j : randomTools) {
        oneBlocks = findOneBlocks(criticalMachine, j);
        for(int remove = 0 ; remove < (int)oneBlocks.size() ; remove++) {
            for(int insert = 0 ; insert < (int)oneBlocks.size() ; insert++) {
                if (remove == insert)
                    continue;
                for(int m = get<0>(oneBlocks[remove]) ; m <= get<1>(oneBlocks[remove]) ; m++) {
                    job = npmJobAssignement[criticalMachine][m];
                    if(insert > remove) {
                        npmJobAssignement[criticalMachine].insert(npmJobAssignement[criticalMachine].begin() + get<0>(oneBlocks[insert]), job);
                        npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + m);
                        if(evaluationFunction() < currentBest) {
                            mI.clear();
                            return true;
                        }
                    
                        for(int n = get<0>(oneBlocks[insert]) - 1; n < get<1>(oneBlocks[insert]); n++) {
                            swap(npmJobAssignement[criticalMachine][n], npmJobAssignement[criticalMachine][n + 1]);
                            if(evaluationFunction() < currentBest) {
                                mI.clear();
                                return true;
                            }
                        }

                        npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + get<1>(oneBlocks[insert]));
                        npmJobAssignement[criticalMachine].insert(npmJobAssignement[criticalMachine].begin() + m, job);
                    }
                    else {
                        npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + m);
                        npmJobAssignement[criticalMachine].insert(npmJobAssignement[criticalMachine].begin() + get<0>(oneBlocks[insert]), job);
                        if(evaluationFunction() < currentBest) {
                            mI.clear();
                            return true;
                        }

                        for(int n = get<0>(oneBlocks[insert]); n <= get<1>(oneBlocks[insert]); n++) {
                            swap(npmJobAssignement[criticalMachine][n], npmJobAssignement[criticalMachine][n + 1]);
                            if(evaluationFunction() < currentBest) {
                                mI.clear();
                                return true;
                            }
                        }

                        npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + get<1>(oneBlocks[insert]) + 1);
                        npmJobAssignement[criticalMachine].insert(npmJobAssignement[criticalMachine].begin() + m, job);
                    }
                }
            }
        }
        evaluationVector[criticalMachine] = beforeSwap1;
    }

    mI.clear();
    return false;
}