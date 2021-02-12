/**
 * Copyright (C) 2021 Fabio Sussarellu
 * 
 * This file is part of amdgpu-fancontrol.
 * 
 * amdgpu-fancontrol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * amdgpu-fancontrol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with amdgpu-fancontrol.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PARAMETERSMANAGEMENT_H
#define PARAMETERSMANAGEMENT_H

// * DNT
#include "../Parameter.h/Parameter.h"
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <wait.h>

#define PARAMETERS_NUMBER 7
enum PARAMETERS_NAMES {
    controlFrequency,
    help,
    maxFanTemp,
    stepsDelta,
    verbose,
    zeroRpmDelta,
    zeroRpmTemp
};
// * EDNT

void parseArgs(int argc, char const *argv[]);
void initParameters();
void printHelp();
extern double CONTROL_FREQUENCY;
extern uint8_t ZERO_RMP_ZONE, MAX_FAN_TEMP, STEPS_DELTA, ZERO_RMP_DELTA;
extern struct timespec WAIT_TIME;
extern bool VERBOSE;
extern Parameter PARAMETERS[PARAMETERS_NUMBER];

#endif // ! PARAMETERSMANAGEMENT_H
