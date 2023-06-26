#include "eligibility.hpp"

int checkJobEligibility(int machineIndex, int job) {
    int sum = 0;
    for(int i = 0 ; i < toolCount ; i++)
        sum += toolsRequirements[i][job];
    return (npmMagazineCapacity[machineIndex] >= sum) ? 1 : 0;
}

void checkMachinesEligibility() {
    for(int i = 0 ; i < machineCount ; i++) {
        for(int j = 0 ; j < jobCount ; j++) {
            jobEligibility[i][j] = checkJobEligibility(i, j);
        }
    }
}