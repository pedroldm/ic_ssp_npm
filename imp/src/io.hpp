#ifndef IO_HPP
#define IO_HPP

#include "global_vars.hpp"
#include "eligibility.hpp"
#include "evaluation.hpp"
#include "metaheuristics.hpp"

/* I/O */
Instance singleRun(string inputFileName, ofstream& outputFile, int run, int objective);
void readProblem(string fileName);
void parseArguments(vector<string> arguments);
bool fileExists(const std::string& filename);
template <typename S>
void printSolution(string inputFileName, double runningTime, int objective, int run, S &s);
void printSummary(string input);
template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v);
template<typename T>
ostream& operator<< (ostream& out, const vector<vector<T>>& matrix);
template<typename T>
ostream& operator<<(ostream& out, const set<T>& m);
template <typename T>
ostream& operator<<(ostream& os, const vector<tuple<T, T>>& vector);

/* Initializes and terminates all data structures */
void initialization();
void termination();
void registerTimeTracking();

#endif