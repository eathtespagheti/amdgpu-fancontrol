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

#include "ParametersManagement.h"
bool VERBOSE              = false;
struct timespec WAIT_TIME = {
    1, 0};

/**
 * Print help message
 */
void printHelp() {
    printf("Usage: amdgpu-fancontrol %sparameter value %sp value\n\n", PARAMETER_PREFIX, SHORT_PARAMETER_PREFIX);
    puts("Parameters available:");
    puts("");

    Parameter p;
    for (size_t i = 0; i < PARAMETERS_NUMBER; i++) {
        ;
        p = PARAMETERS[i];
        printf("%s%s", PARAMETER_PREFIX, p->Name);
        if (p->Shortname != NULL) {
            printf(" %s%s", SHORT_PARAMETER_PREFIX, p->Shortname);
        }
        printf(":");
        if (p->Value != NULL) {
            printf("\tDEFAULT VALUE: %s", p->Value);
        }
        printf("\n%s\n", p->Description);
        puts("");
    }

    exit(0);
}

/**
 * Take parameters values from parameters array and set global variables
 */
static void calculateParametersValues() {
    CONTROL_FREQUENCY = getParameterFloatValue(PARAMETERS[controlFrequency]);
    MAX_FAN_TEMP      = getParameterIntValue(PARAMETERS[maxFanTemp]);
    STEPS_DELTA       = getParameterIntValue(PARAMETERS[stepsDelta]);
    VERBOSE           = getParameterBoolValue(PARAMETERS[verbose]);
    ZERO_RMP_DELTA    = getParameterIntValue(PARAMETERS[zeroRpmDelta]);
    ZERO_RMP_ZONE     = getParameterIntValue(PARAMETERS[zeroRpmTemp]);

    WAIT_TIME.tv_sec  = 1 / CONTROL_FREQUENCY;
    WAIT_TIME.tv_nsec = ((1.0 / CONTROL_FREQUENCY) - WAIT_TIME.tv_sec) * 1000000000;

    if (VERBOSE) {
        printf("Max fan temp: %dC°\nSteps delta: %d\nZero RMP temp: %dC°\nZero RPM delta: %dC°\nControl frequency it's %.2fHz\n", MAX_FAN_TEMP, STEPS_DELTA, ZERO_RMP_ZONE, ZERO_RMP_DELTA, CONTROL_FREQUENCY);
    }
}

/**
 * Initialize parameters values
 */
void initParameters() {
    PARAMETERS[controlFrequency] = newParameter("control-frequency", "f", "1", "Set how many times per second temperature value should be checked");
    PARAMETERS[help]             = newParameter("help", "h", "", "Print help message");
    PARAMETERS[maxFanTemp]       = newParameter("max-fan-temp", "m", "90", "Set temperature target for 100% fan speed");
    PARAMETERS[stepsDelta]       = newParameter("steps-delta", "s", "3", "Amount of minium temperature delta between different fan speed steps");
    PARAMETERS[verbose]          = newParameter("verbose", "v", "", "Enable verbose mode");
    PARAMETERS[zeroRpmDelta]     = newParameter("zero-rpm-delta", "zd", "3", "Delta value for the zero-rpm-temp, if temperature exceed zero-rpm-temp but it's lower than zero-rpm-temp + zero-rpm-delta, fans keep spinning at constant speed until temperature reaches zero-rpm-temp - zero-rpm-delta");
    PARAMETERS[zeroRpmTemp]      = newParameter("zero-rpm-temp", "zt", "33", "Set the temperature limit for the zero rpm mode, if this temperature it's exceeded fan start spinning");
}

/**
 * Parse commandline args
 */
void parseArgs(int argc, char const *argv[]) {
    bool toCheck[argc - 1];
    for (size_t i = 0; i < argc - 1; i++) {
        toCheck[i] = true;
    }
    for (size_t i = 1; i < argc; i++) {
        if (toCheck[i - 1]) {
            if (compareParameter(PARAMETERS[controlFrequency], argv[i])) {
                if (i + 1 < argc) {
                    toCheck[i] = false;
                    setParameterValue(PARAMETERS[controlFrequency], argv[i + 1]);
                }
            } else if (compareParameter(PARAMETERS[help], argv[i])) {
                printHelp();
            } else if (compareParameter(PARAMETERS[maxFanTemp], argv[i])) {
                if (i + 1 < argc) {
                    toCheck[i] = false;
                    setParameterValue(PARAMETERS[maxFanTemp], argv[i + 1]);
                }
            } else if (compareParameter(PARAMETERS[stepsDelta], argv[i])) {
                if (i + 1 < argc) {
                    toCheck[i] = false;
                    setParameterValue(PARAMETERS[stepsDelta], argv[i + 1]);
                }
            } else if (compareParameter(PARAMETERS[verbose], argv[i])) {
                setParameterValue(PARAMETERS[verbose], "1");
            } else if (compareParameter(PARAMETERS[zeroRpmDelta], argv[i])) {
                if (i + 1 < argc) {
                    toCheck[i] = false;
                    setParameterValue(PARAMETERS[zeroRpmDelta], argv[i + 1]);
                }
            } else if (compareParameter(PARAMETERS[zeroRpmTemp], argv[i])) {
                if (i + 1 < argc) {
                    toCheck[i] = false;
                    setParameterValue(PARAMETERS[zeroRpmTemp], argv[i + 1]);
                }
            } else {
                printf("Parameter not recognized \"%s\"\n", argv[i]);
                printHelp();
            }
        }
        toCheck[i - 1] = false;
    }
    calculateParametersValues();
}