#include "local_search.hpp"

bool VNDFull(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    int k = 1;
    while (k != 5) {
        time_span = duration_cast<duration<double>>(high_resolution_clock::now() - t1);
        if(time_span.count() >= maxTime)
            return false;
        currentBest = evaluationFunction();
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
                if(twoOptLocalSearch(evaluationFunction, evaluationVector, currentBest, false)) {
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

bool VNDFullSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    int k = 1;
    while (k != 5) {
        time_span = duration_cast<duration<double>>(high_resolution_clock::now() - t1);
        if(time_span.count() >= maxTime)
            return false;
        currentBest = evaluationFunction();
        switch(k) {
            case 1 :
                if(jobExchangeLocalSearchFullSim(evaluationFunction, evaluationVector, currentBest)) {
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
                if(swapLocalSearchSim(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[2]++;
                    k = 1;
                }
                else
                    k++;
                break;
            case 4 :
                if(twoOptLocalSearchSim(evaluationFunction, evaluationVector, currentBest, false)) {
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
    while (k < 5) {
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
                if(twoOptLocalSearch(evaluationFunction, evaluationVector, currentBest, true)) {
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

bool VNDCritSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector) {
    int k = 1;
    while (k < 5) {
        time_span = duration_cast<duration<double>>(high_resolution_clock::now() - t1);
        if(time_span.count() >= maxTime)
            return false;
        currentBest = evaluationFunction();
        switch(k) {
            case 1 :
                if(jobExchangeLocalSearchCritSim(evaluationFunction, evaluationVector, currentBest)) {
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
                if(swapLocalSearchSim(evaluationFunction, evaluationVector, currentBest)) {
                    summary.localSearchImprovements[2]++;
                    k = 1;
                }
                else
                    k++;
                break;
            case 4 :
                if(twoOptLocalSearchSim(evaluationFunction, evaluationVector, currentBest, true)) {
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

bool jobExchangeLocalSearchCritSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest) {
    int criticalMachine = distance(evaluationVector.begin(),max_element(evaluationVector.begin(), evaluationVector.end()));

    for(int i = 0 ; i < (int)npmJobAssignement[criticalMachine].size() ; i++) {
        for(int l = 0 ; l < machineCount; l++) {
            mI = {criticalMachine, l};
            for (int j = 0 ; j < (int)npmJobAssignement[l].size() ; j++) {
                if (similarityMatrix[i][j] && jobEligibility[criticalMachine][npmJobAssignement[l][j]] && jobEligibility[l][npmJobAssignement[criticalMachine][i]]) {
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

bool jobExchangeLocalSearchFullSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest) {
    for (int l = 0 ; l < machineCount - 1; l++) {
        for (int k = l + 1 ; k < machineCount ; k++) {
            mI = {l, k};
            for(int i = 0 ; i < (int)npmJobAssignement[l].size() ; i++) {
                for (int j = 0 ; j < (int)npmJobAssignement[k].size() ; j++) {
                    if (similarityMatrix[i][j] && jobEligibility[l][npmJobAssignement[k][j]] && jobEligibility[k][npmJobAssignement[l][i]]) {
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

bool twoOptLocalSearch(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest, bool onlyCriticalMachine) {
    if (onlyCriticalMachine) {
        int i = distance(npmCurrentMakespan.begin(),max_element(npmCurrentMakespan.begin(), npmCurrentMakespan.end()));
        mI = {i};
        for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
            for (int k = j + 3 ; k < (int)npmJobAssignement[i].size() ; k++) {
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
    else {
        for(int i = 0 ; i < machineCount ; i++) {
            mI = {i};
            for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
                for (int k = j + 3 ; k < (int)npmJobAssignement[i].size() ; k++) {
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
    }

    mI.clear();
    return false;
}

bool twoOptLocalSearchSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest, bool onlyCriticalMachine) {
    if (onlyCriticalMachine) {
        int i = distance(npmCurrentMakespan.begin(),max_element(npmCurrentMakespan.begin(), npmCurrentMakespan.end()));
        mI = {i};
        for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
            for (int k = j + 3 ; k < (int)npmJobAssignement[i].size() ; k++) {
                if(!similarityMatrix[j][k])
                    continue;
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
    else {
        for(int i = 0 ; i < machineCount ; i++) {
            mI = {i};
            for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
                for (int k = j + 3 ; k < (int)npmJobAssignement[i].size() ; k++) {
                    if(!similarityMatrix[j][k])
                        continue;
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

bool swapLocalSearchSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest) {
    for(int i = 0 ; i < machineCount ; i++) {
        for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
            for (int k = j + 1 ; k < (int)npmJobAssignement[i].size() ; k++) {
                if(!similarityMatrix[j][k])
                    continue;
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
    mt19937 rng(random_device{}());
    shuffle(randomTools.begin(), randomTools.end(), rng);

    for(int i = 0 ; i < machineCount ; i++) {
        mI = {i};
        beforeSwap1 = evaluationVector[i];
        vector<int>oldAssignement = npmJobAssignement[i];
        for(int j : randomTools) {
            oneBlocks = findOneBlocks(i, j);
            for(int rm = 0 ; rm < oneBlocks.size() ; rm++){
                for(int ins = 0 ; ins < oneBlocks.size() ; ins++){
                    if(rm == ins)
                        continue;
                    int oneBlockSize = get<1>(oneBlocks[rm]) - get<0>(oneBlocks[rm]) + 1; 
                    if(rm > ins){
                        vector<int> blockToMove(npmJobAssignement[i].begin() + get<0>(oneBlocks[rm]), npmJobAssignement[i].begin() + get<1>(oneBlocks[rm]) + 1);
                        npmJobAssignement[i].erase(npmJobAssignement[i].begin() + get<0>(oneBlocks[rm]), npmJobAssignement[i].begin() + get<1>(oneBlocks[rm]) + 1);
                        npmJobAssignement[i].insert(npmJobAssignement[i].begin() + get<0>(oneBlocks[ins]), blockToMove.begin(), blockToMove.end());
                        if(evaluationFunction() < currentBest) {
                            mI.clear();
                            return true;
                        }
                        for(int s_in = get<0>(oneBlocks[ins]) ; s_in < get<1>(oneBlocks[ins]) + 1 ; s_in++){
                            npmJobAssignement[i].erase(npmJobAssignement[i].begin() + s_in, npmJobAssignement[i].begin() + s_in + oneBlockSize);
                            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + s_in + 1, blockToMove.begin(), blockToMove.end());
                            if(evaluationFunction() < currentBest) {
                                mI.clear();
                                return true;
                            }
                        }
                        npmJobAssignement[i] = oldAssignement;
                    }
                    else {
                        npmJobAssignement[i].insert(npmJobAssignement[i].begin() + get<0>(oneBlocks[ins]), npmJobAssignement[i].begin() + get<0>(oneBlocks[rm]), npmJobAssignement[i].begin() + get<1>(oneBlocks[rm]) + 1);
                        npmJobAssignement[i].erase(npmJobAssignement[i].begin() + get<0>(oneBlocks[rm]), npmJobAssignement[i].begin() + get<1>(oneBlocks[rm]) + 1);
                        if(evaluationFunction() < currentBest) {
                            mI.clear();
                            return true;
                        }
                        for(int s_in = get<0>(oneBlocks[ins]) ; s_in < get<1>(oneBlocks[ins]) + 1 ; s_in++){
                            npmJobAssignement[i].insert(npmJobAssignement[i].begin() + s_in + 1, npmJobAssignement[i].begin() + s_in - oneBlockSize, npmJobAssignement[i].begin() + s_in);
                            npmJobAssignement[i].erase(npmJobAssignement[i].begin() + s_in - oneBlockSize, npmJobAssignement[i].begin() + s_in);
                            if(evaluationFunction() < currentBest) {
                                mI.clear();
                                return true;
                            }
                        }
                        npmJobAssignement[i] = oldAssignement;
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
        vector<int>oldAssignement = npmJobAssignement[criticalMachine];
        for(int j : randomTools) {
            oneBlocks = findOneBlocks(criticalMachine, j);
            for(int rm = 0 ; rm < oneBlocks.size() ; rm++){
                for(int ins = 0 ; ins < oneBlocks.size() ; ins++){
                    if(rm == ins)
                        continue;
                    int oneBlockSize = get<1>(oneBlocks[rm]) - get<0>(oneBlocks[rm]) + 1; 
                    if(rm > ins){
                        vector<int> blockToMove(npmJobAssignement[criticalMachine].begin() + get<0>(oneBlocks[rm]), npmJobAssignement[criticalMachine].begin() + get<1>(oneBlocks[rm]) + 1);
                        npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + get<0>(oneBlocks[rm]), npmJobAssignement[criticalMachine].begin() + get<1>(oneBlocks[rm]) + 1);
                        npmJobAssignement[criticalMachine].insert(npmJobAssignement[criticalMachine].begin() + get<0>(oneBlocks[ins]), blockToMove.begin(), blockToMove.end());
                        if(evaluationFunction() < currentBest) {
                            mI.clear();
                            return true;
                        }
                        for(int s_in = get<0>(oneBlocks[ins]) ; s_in < get<1>(oneBlocks[ins]) + 1 ; s_in++){
                            npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + s_in, npmJobAssignement[criticalMachine].begin() + s_in + oneBlockSize);
                            npmJobAssignement[criticalMachine].insert(npmJobAssignement[criticalMachine].begin() + s_in + 1, blockToMove.begin(), blockToMove.end());
                            if(evaluationFunction() < currentBest) {
                                mI.clear();
                                return true;
                            }
                        }
                        npmJobAssignement[criticalMachine] = oldAssignement;
                    }
                    else {
                        npmJobAssignement[criticalMachine].insert(npmJobAssignement[criticalMachine].begin() + get<0>(oneBlocks[ins]), npmJobAssignement[criticalMachine].begin() + get<0>(oneBlocks[rm]), npmJobAssignement[criticalMachine].begin() + get<1>(oneBlocks[rm]) + 1);
                        npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + get<0>(oneBlocks[rm]), npmJobAssignement[criticalMachine].begin() + get<1>(oneBlocks[rm]) + 1);
                        if(evaluationFunction() < currentBest) {
                            mI.clear();
                            return true;
                        }
                        for(int s_in = get<0>(oneBlocks[ins]) ; s_in < get<1>(oneBlocks[ins]) + 1 ; s_in++){
                            npmJobAssignement[criticalMachine].insert(npmJobAssignement[criticalMachine].begin() + s_in + 1, npmJobAssignement[criticalMachine].begin() + s_in - oneBlockSize, npmJobAssignement[criticalMachine].begin() + s_in);
                            npmJobAssignement[criticalMachine].erase(npmJobAssignement[criticalMachine].begin() + s_in - oneBlockSize, npmJobAssignement[criticalMachine].begin() + s_in);
                            if(evaluationFunction() < currentBest) {
                                mI.clear();
                                return true;
                            }
                        }
                        npmJobAssignement[criticalMachine] = oldAssignement;
                    }
                }
            }
        }
        evaluationVector[criticalMachine] = beforeSwap1;

    mI.clear();
    return false;
}