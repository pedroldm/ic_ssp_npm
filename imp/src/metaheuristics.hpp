#ifndef METAHEURISTICS_HPP
#define METAHEURISTICS_HPP

#include "global_vars.hpp"

/* Metaheuristics */
void ILSCrit(function<int(void)> evaluationFunction, vector<int> &evaluationVector);
void ILSFull(function<int(void)> evaluationFunction, vector<int> &evaluationVector);

void jobInsertionDisturb();
void jobExchangeDisturb();
void twoOptDisturb();
void swapDisturb();
bool criticJobDisturb();

void updateBestSolution(function<int(void)> evaluationFunction);

#endif