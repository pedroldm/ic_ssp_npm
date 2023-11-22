#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include "global_vars.hpp"

/* Evaluation functions */
int toolsDistances(int machineIndex, int currentJob, int currentTool, int jobAssignedCount);
int GPCA();
int KTNS();
int makespanEvaluation();
int replacedTool(int i);
void fillToolsDistances(int machineIndex, int jobCount);
int fillStartMagazine(int machineIndex, int jobsAssignedCount);
int flowtimeEvaluation();
vector<set<tuple<int,int>>> criticJobEvaluation();

#endif