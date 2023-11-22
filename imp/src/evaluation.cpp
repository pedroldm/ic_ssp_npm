#include "evaluation.hpp"

vector<set<tuple<int,int>>> criticJobEvaluation() {
    vector<set<tuple<int,int>>> cJ(machineCount);
    for(int machineIndex = 0 ; machineIndex < machineCount ; machineIndex++) {
        int jobsAssignedCount = (int)npmJobAssignement[machineIndex].size(), empty; /* .size() -> O(1)*/
        npmCurrentToolSwitches[machineIndex] = 0;
        if(!npmJobAssignement[machineIndex].size())
            continue;

        fillToolsDistances(machineIndex, jobsAssignedCount); /* O(tj) */
        int startSwitch = fillStartMagazine(machineIndex, jobsAssignedCount); /* O(j) */

        for(int i = startSwitch ; i < jobsAssignedCount ; i++) {
            magazines[1].insert(jobSets[npmJobAssignement[machineIndex][i]].begin(), jobSets[npmJobAssignement[machineIndex][i]].end()); /* O(t) */
            empty = npmMagazineCapacity[machineIndex] - magazines[1].size();
            for(auto t : magazines[0]) { /* O(t) */
                if(!toolsRequirements[t][npmJobAssignement[machineIndex][i]])
                    dist.insert(make_tuple(toolsDistancesGPCA[t][i], t)); /* O(logt) */
            }
            npmCurrentToolSwitches[machineIndex] += dist.size() - empty;
            cJ[machineIndex].insert(make_tuple(((dist.size() - empty) * npmSwitchCost[machineIndex]) + npmJobTime[machineIndex][npmJobAssignement[machineIndex][i]], i));

            if(empty) {
                for(auto t : dist) {
                    magazines[1].insert(get<1>(t)); /* O(logt) */
                    if(!--empty)
                        break;
                }
            }
            magazines[0] = magazines[1]; /* O(t) */
            magazines[1].clear(); /* O(t) */
            dist.clear(); /* O(t) */
        }
        magazines[0].clear(); /* O(t) */
    }

    return cJ;
}

int makespanEvaluation() {
    GPCA();

    for(int i : mI) {
        npmCurrentMakespan[i] = npmCurrentToolSwitches[i] * npmSwitchCost[i];
        for(int j = 0 ; j < (int)npmJobAssignement[i].size() ; j++) {
            npmCurrentMakespan[i] += npmJobTime[i][npmJobAssignement[i][j]];
        }
    }

    return *max_element(npmCurrentMakespan.begin(), npmCurrentMakespan.end());
}

int flowtimeEvaluation() {
    if(mI.empty()) {
        mI = vector<int>(machineCount);
        iota(mI.begin(), mI.end(), 0);
    }
    for(int machineIndex : mI) {
        npmCurrentFlowTime[machineIndex] = 0;
        npmCurrentToolSwitches[machineIndex] = 0;
        flowtimeSum = 0;
        int jobsAssignedCount = (int)npmJobAssignement[machineIndex].size(), empty; /* .size() -> O(1)*/
        if(!npmJobAssignement[machineIndex].size())
            continue;

        fillToolsDistances(machineIndex, jobsAssignedCount); /* O(tj) */
        int startSwitch = fillStartMagazine(machineIndex, jobsAssignedCount); /* O(j) */
        
        flowtimeAux = (startSwitch > jobsAssignedCount) ? jobsAssignedCount : startSwitch;
        for(int i = 0 ; i < flowtimeAux ; i++) {
            flowtimeSum += npmJobTime[machineIndex][npmJobAssignement[machineIndex][i]];
            npmCurrentFlowTime[machineIndex] += flowtimeSum;
        }

        for(int i = startSwitch ; i < jobsAssignedCount ; i++) {
            magazines[1].insert(jobSets[npmJobAssignement[machineIndex][i]].begin(), jobSets[npmJobAssignement[machineIndex][i]].end()); /* O(t) */
            empty = npmMagazineCapacity[machineIndex] - magazines[1].size();
            for(auto t : magazines[0]) { /* O(t) */
                if(!toolsRequirements[t][npmJobAssignement[machineIndex][i]])
                    dist.insert(make_tuple(toolsDistancesGPCA[t][i], t)); /* O(logt) */
            }
            npmCurrentToolSwitches[machineIndex] += dist.size() - empty;

            flowtimeSum += ((dist.size() - empty) * npmSwitchCost[machineIndex]) + npmJobTime[machineIndex][npmJobAssignement[machineIndex][i]];
            npmCurrentFlowTime[machineIndex] += flowtimeSum;

            if(empty) {
                for(auto t : dist) {
                    magazines[1].insert(get<1>(t)); /* O(logt) */
                    if(!--empty)
                        break;
                }
            }
            magazines[0] = magazines[1]; /* O(t) */
            magazines[1].clear(); /* O(t) */
            dist.clear(); /* O(t) */
        }
        magazines[0].clear(); /* O(t) */
    }

    return accumulate(npmCurrentFlowTime.begin(),npmCurrentFlowTime.end(), 0);
}

int GPCA() {
    if(mI.empty()) {
        mI = vector<int>(machineCount);
        iota(mI.begin(), mI.end(), 0);
    }
    for(int machineIndex : mI) {
        int jobsAssignedCount = (int)npmJobAssignement[machineIndex].size(), empty; /* .size() -> O(1)*/
        npmCurrentToolSwitches[machineIndex] = 0;
        if(!npmJobAssignement[machineIndex].size())
            continue;

        fillToolsDistances(machineIndex, jobsAssignedCount); /* O(tj) */
        int startSwitch = fillStartMagazine(machineIndex, jobsAssignedCount); /* O(j) */

        for(int i = startSwitch ; i < jobsAssignedCount ; i++) {
            magazines[1].insert(jobSets[npmJobAssignement[machineIndex][i]].begin(), jobSets[npmJobAssignement[machineIndex][i]].end()); /* O(t) */
            empty = npmMagazineCapacity[machineIndex] - magazines[1].size();
            for(auto t : magazines[0]) { /* O(t) */
                if(!toolsRequirements[t][npmJobAssignement[machineIndex][i]])
                    dist.insert(make_tuple(toolsDistancesGPCA[t][i], t)); /* O(logt) */
            }
            npmCurrentToolSwitches[machineIndex] += dist.size() - empty;

            if(empty) {
                for(auto t : dist) {
                    magazines[1].insert(get<1>(t)); /* O(logt) */
                    if(!--empty)
                        break;
                }
            }
            magazines[0] = magazines[1]; /* O(t) */
            magazines[1].clear(); /* O(t) */
            dist.clear(); /* O(t) */
        }
        magazines[0].clear(); /* O(t) */
    }
    return accumulate(npmCurrentToolSwitches.begin(),npmCurrentToolSwitches.end(), 0);
}

int fillStartMagazine(int machineIndex, int jobsAssignedCount) {
    for(int i = 0 ; i < jobsAssignedCount ; i++) {
        for(auto s : jobSets[npmJobAssignement[machineIndex][i]]) {
            if(npmMagazineCapacity[machineIndex] - magazines[0].size())
                magazines[0].insert(s);
            else    
                return i;
        }
    }
    return INT_MAX;
}

void fillToolsDistances(int machineIndex, int jobsAssignedCount) {
    for(int i = 0 ; i < toolCount ; i++) {
        if(toolsRequirements[i][npmJobAssignement[machineIndex][jobsAssignedCount - 1]])
            toolsDistancesGPCA[i][jobsAssignedCount - 1] = jobsAssignedCount - 1;
        else
            toolsDistancesGPCA[i][jobsAssignedCount - 1] = INT_MAX;
    }

    for(int i = jobsAssignedCount - 2 ; i >= 0 ; i--) {
        for(int j = 0 ; j < toolCount ; j++) {
            if(toolsRequirements[j][npmJobAssignement[machineIndex][i]])
                toolsDistancesGPCA[j][i] = i;
            else 
                toolsDistancesGPCA[j][i] = toolsDistancesGPCA[j][i + 1];
        }
    }
}

int replacedTool (int i) {
    for(auto t : magazines[0])
        dist.insert(make_tuple(toolsDistancesGPCA[t][i], t));

    return get<1>(*dist.rbegin());
}

int KTNS() {
    for(int machineIndex = 0 ; machineIndex < machineCount ; machineIndex++) {
        npmCurrentToolSwitches[machineIndex] = 0;
        int jobsAssignedCount = (int)npmJobAssignement[machineIndex].size();

        fillToolsDistances(machineIndex, jobsAssignedCount);
        int startSwitch = fillStartMagazine(machineIndex, jobsAssignedCount);

        for(int i = startSwitch ; i < jobsAssignedCount ; i++) {
            for(int j = 0 ; j < toolCount ; j++) {
                if(toolsRequirements[j][npmJobAssignement[machineIndex][i]] && find(magazines[0].begin(), magazines[0].end(), j) == magazines[0].end()) {
                    magazines[0].insert(j);
                    magazines[0].erase(replacedTool(i));
                    npmCurrentToolSwitches[machineIndex]++;
                    dist.clear();
                }
            }
        }
        magazines[0].clear();
    }
    return accumulate(npmCurrentToolSwitches.begin(),npmCurrentToolSwitches.end(),0);
}