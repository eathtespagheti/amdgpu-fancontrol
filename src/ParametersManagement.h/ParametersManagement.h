#ifndef PARAMETERSMANAGEMENT_H
#define PARAMETERSMANAGEMENT_H

// * DNT
#include "../Parameter.h/Parameter.h"
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <wait.h>

#define PARAMETERS_NUMBER 7
Parameter PARAMETERS[PARAMETERS_NUMBER];
enum PARAMETERS_NAMES {
    controlFrequency,
    help,
    maxFanTemp,
    stepsDelta,
    verbose,
    zeroRpmDelta,
    zeroRpmTemp
};
uint8_t ZERO_RMP_ZONE, MAX_FAN_TEMP, STEPS_DELTA, ZERO_RMP_DELTA;
double CONTROL_FREQUENCY;
extern bool VERBOSE;
extern struct timespec WAIT_TIME;
// * EDNT

void parseArgs(int argc, char const *argv[]);
void initParameters();
void printHelp();
extern struct timespec WAIT_TIME;
extern bool VERBOSE;

#endif // ! PARAMETERSMANAGEMENT_H
