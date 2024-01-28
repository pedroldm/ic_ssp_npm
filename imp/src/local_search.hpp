#ifndef LOCAL_SEARCH_HPP
#define LOCAL_SEARCH_HPP

#include "global_vars.hpp"

bool VNDCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector);
bool VNDFull(function<int(void)> evaluationFunction, vector<int> &evaluationVector);
bool VNDCritSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector);
bool VNDFullSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector);

/* Local search methods */
/* 0 */ bool jobInsertionLocalSearchCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest);
/* 0 */ bool jobInsertionLocalSearchFull(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest);
/* 1 */ bool twoOptLocalSearch(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest, bool onlyCriticalMachine);
/* 2 */ bool jobExchangeLocalSearchCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest);
/* 2 */ bool jobExchangeLocalSearchFull(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest);
/* 3 */ bool swapLocalSearch(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest);
bool oneBlockLocalSearch(function<int(void)> &evaluationFunction, vector<int>& evaluationVector, int currentBest);
bool oneBlockLocalSearchCrit(function<int(void)> &evaluationFunction, vector<int>& evaluationVector, int currentBest);
vector<tuple<int,int>> findOneBlocks(int machineIndex, int tool);

bool swapLocalSearchSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest);
bool twoOptLocalSearchSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest, bool onlyCriticalMachine);
bool jobExchangeLocalSearchFullSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest);
bool jobExchangeLocalSearchCritSim(function<int(void)> evaluationFunction, vector<int> &evaluationVector, int currentBest);

#endif