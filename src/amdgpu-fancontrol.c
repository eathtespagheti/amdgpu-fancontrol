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

#include "TemperatureSpeedMap.h/TemperatureSpeedMap.h"
#include <signal.h>
#include <unistd.h>

/**
 * echo value inside the pwmControl file
*/
void writeToPWM(uint8_t cardID, uint8_t hwmonID, uint8_t value, const char pwmControl[]) {
    char commandPreset[]  = "echo \"%d\" > %s/%s%d/%s/%s%d/%s";
    uint8_t commandLenght = strlen(commandPreset) - 2 * 8 + 3 + strlen(DEVICES_PATH) + 1 + strlen(CARD_PREFIX) + 2 + 1 + strlen(HWMON_PATH) + 1 + strlen(HWMON_PREFIX) + 2 + 1 + strlen(pwmControl);
    char command[commandLenght + 1];
    snprintf(command, commandLenght, commandPreset, value, DEVICES_PATH, CARD_PREFIX, cardID, HWMON_PATH, HWMON_PREFIX, hwmonID, pwmControl);
    system(command);
}

/**
 * echo value inside the pwm_enable
*/
void writeToPwmEnable(uint8_t cardID, uint8_t hwmonID, uint8_t value) {
    writeToPWM(cardID, hwmonID, value, PWM_ENABLE);
}

/**
 * Restore automatic fan speed on the last
 * manual control enabled card
*/
void restoreAutoFan() {
    if (ENABLED_CARD != -1 && CURRENT_HWMON != -1) {
        if (VERBOSE) {
            puts("Resetting auto fan speed");
        }
        writeToPwmEnable(ENABLED_CARD, CURRENT_HWMON, 2);
        ENABLED_CARD  = -1;
        CURRENT_HWMON = -1;
    }
}

/**
 * Safely quit from program
 */
void quit() {
    if (READ_TEMP_COMMAND != NULL) {
        free(READ_TEMP_COMMAND);
    }
    if (TEMP_SENSOR_LOCATION != NULL) {
        free(TEMP_SENSOR_LOCATION);
    }
    restoreAutoFan();
    for (size_t i = 0; i < PARAMETERS_NUMBER; i++) {
        freeParameter(PARAMETERS[i]);
    }
    exit(0);
}

/**
 * Enable manual fan control for cardId on hwmonID
 * Launching this will disable manual fan speed on every card where this
 * function has been called previously
*/
void enableManualFan(uint8_t cardID, uint8_t hwmonID) {
    restoreAutoFan();
    writeToPwmEnable(cardID, hwmonID, 1);
    ENABLED_CARD  = cardID;
    CURRENT_HWMON = hwmonID;
    buildTemperatureSpeedMap(&fanSpeedFunction);
}

/**
 * Manage signal that require stopping amdgpu-fancontrol
*/
void signalHandler(int signal) {
    if (signal == SIGCHLD) {
        return;
    }
    // Works only for childs
    quit();
}

/**
 * Set fan speed in percantage for the card
 * Works only after calling enableManualFan on said card
*/
void setFanSpeed(uint8_t value) {
    writeToPWM(ENABLED_CARD, CURRENT_HWMON, value * 2.55, PWM);
}

/**
 * Find a AMD GPU and return it's ID
*/
int8_t findCardID() {
    /**
     * Command composed by command preset minus args +
     * DEVICES_PATH +
     * CARD_PREFIX * 2 +
     * EOL
    */
    char commandPreset[]     = "ls -1 %s | grep %s[[:digit:]]*$ | sed {'s/%s//'}";
    uint8_t maxCommandLenght = strlen(commandPreset) - 6 + strlen(DEVICES_PATH) + strlen(CARD_PREFIX) * 2 + 1;
    char command[maxCommandLenght];
    snprintf(command, maxCommandLenght, commandPreset, DEVICES_PATH, CARD_PREFIX, CARD_PREFIX);

    // List all cards in the system without the CARD_PREFIX
    FILE *cmd = popen(command, "r");
    // Capped at max 100 cards
    char cardID[3] = {0x0};
    uint8_t IDs[100];
    int numberOfCards = 0;
    while (fgets(cardID, sizeof(cardID), cmd) != NULL) {
        IDs[numberOfCards++] = atoi(cardID);
    }
    // Close process output
    pclose(cmd);
    if (VERBOSE) {
        printf("%d card(s) found\n", numberOfCards);
    }
    if (numberOfCards == 0) {
        if (VERBOSE) {
            fprintf(stderr, "No cards found!\n");
        }
        exit(1);
    }

    strcpy(commandPreset, "cat /sys/class/drm/%s%d/device/vendor");
    char cardVendor[strlen(CARD_PREFIX) + 2 + 1];
    maxCommandLenght = strlen(commandPreset) - 4 + strlen(CARD_PREFIX) + 2 + 1;

    // For every card
    for (size_t i = 0; i < numberOfCards; i++) {
        if (!fork()) { // Create a child for every card
            // Read vendor ID
            snprintf(command, maxCommandLenght, commandPreset, CARD_PREFIX, IDs[i]);
            cmd = popen(command, "r");
            fgets(cardVendor, sizeof(cardVendor), cmd);
            pclose(cmd);
            if (strcmp(cardVendor, AMD_VENDOR) != 0) {
                if (VERBOSE) {
                    fprintf(stderr, "[%s%d] Not an AMD card!\n", CARD_PREFIX, i);
                }
                exit(2);
            }
            return IDs[i]; // Return valid AMD card ID
        }
    }

    // Only Parent
    signal(SIGCHLD, signalHandler);
    while (true) {
        pause();
    }
    exit(0);
}

// Search hwmon id for the specified card
int8_t findHwmon(uint8_t cardID) {
    char commandPreset[]  = "ls -1 /sys/class/drm/%s%d/%s/ | sed {'s/%s//'}";
    uint8_t commandLenght = 60 - 2 * 4 + strlen(CARD_PREFIX) + strlen(HWMON_PATH) + strlen(HWMON_PREFIX) + 2;
    char command[commandLenght + 1];
    snprintf(command, commandLenght, commandPreset, CARD_PREFIX, cardID, HWMON_PATH, HWMON_PREFIX);
    // Execute command
    FILE *cmd = popen(command, "r");
    char hwmonID[2];
    if (fgets(hwmonID, sizeof(hwmonID), cmd) == NULL) {
        if (VERBOSE) {
            fprintf(stderr, "[%s%d] No hwmon available!\n", CARD_PREFIX, cardID);
        }
        exit(3);
    }
    pclose(cmd);
    return atoi(hwmonID);
}

/**
 * Call signal on various interrupting signals
*/
void childSignalsManagement() {
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
}

/**
 * Return card temperature, works only after wiriting read command in READ_TEMP_COMMAND
*/
uint8_t getTempValue() {
    FILE *tempReadingOutput;
    char temperature[3];
    tempReadingOutput = popen(READ_TEMP_COMMAND, "r");
    fgets(temperature, sizeof(temperature), tempReadingOutput);
    pclose(tempReadingOutput);
    return atoi(temperature);
}

/**
 * Wait for temperature to change and return it's value
 */
uint8_t getNewTemperature(uint8_t lastTemperature) {
    uint8_t temp = 0;
    temp         = getTempValue();
    while (temp == lastTemperature) {
        nanosleep(&WAIT_TIME, &WAIT_TIME);
        temp = getTempValue();
    }
    return temp;
}

int main(int argc, char const *argv[]) {
    initParameters();
    // Read args
    parseArgs(argc, argv);

    // Find card
    int8_t cardID        = findCardID();
    uint8_t stringLenght = strlen(DEVICES_PATH) + 1 + strlen(CARD_PREFIX) + 2;
    char cardPath[stringLenght + 1];
    // Write card path
    snprintf(cardPath, stringLenght, "%s/%s%d\n", DEVICES_PATH, CARD_PREFIX, cardID);

    // Enable signals management for childs
    childSignalsManagement();

    // Find hwmon
    int8_t hwmonID = findHwmon(cardID);
    stringLenght   = strlen(cardPath) + 1 + strlen(HWMON_PATH) + 1 + strlen(HWMON_PREFIX) + 2;
    char hwmonControllerPath[stringLenght + 1];
    // Write hwmon path
    snprintf(hwmonControllerPath, stringLenght, "%s/%s/%s%d", cardPath, HWMON_PATH, HWMON_PREFIX, hwmonID);

    // Temperature sensor location
    stringLenght         = strlen(hwmonControllerPath) + 1 + strlen(TEMP_SENSOR) + 1;
    TEMP_SENSOR_LOCATION = calloc(stringLenght + 1, sizeof(*TEMP_SENSOR_LOCATION));
    snprintf(TEMP_SENSOR_LOCATION, stringLenght, "%s/%s", hwmonControllerPath, TEMP_SENSOR);
    // Temperature reading loop
    char commandPreset[] = "cat %s";
    stringLenght         = strlen(commandPreset) + 1 - 2 + strlen(TEMP_SENSOR_LOCATION);
    READ_TEMP_COMMAND    = calloc(stringLenght + 1, sizeof(*READ_TEMP_COMMAND));
    snprintf(READ_TEMP_COMMAND, stringLenght, commandPreset, TEMP_SENSOR_LOCATION);
    uint8_t tempValue, fanspeed;

    // Start monitoring card temperature
    enableManualFan(cardID, hwmonID);
    tempValue = getTempValue();
    fanspeed  = fanSpeedValue(tempValue);
    setFanSpeed(fanspeed);
    // Log values
    if (VERBOSE) {
        logCardName();
        printf("Temperature it's: %dC° - Fan speed it's %d%\n", tempValue, fanspeed);
    }
    while (true) {
        tempValue = getNewTemperature(tempValue);
        fanspeed  = fanSpeedValue(tempValue);
        setFanSpeed(fanspeed);
        // Log values
        if (VERBOSE) {
            logCardName();
            printf("Temperature it's: %dC° - Fan speed it's %d%\n", tempValue, fanspeed);
        }

        // Zero RPM management
        if (tempValue > ZERO_RMP_ZONE && tempValue < ZERO_RMP_ZONE + ZERO_RMP_DELTA) { // In case of ramping up from ZERO_RMP_MODE
            if (VERBOSE) {
                logCardName();
                printf("Exit from Zero RPM Mode\n");
            }
            uint8_t targetTemp      = ZERO_RMP_ZONE - ZERO_RMP_DELTA;
            uint8_t tempTemperature = getNewTemperature(tempTemperature);
            while (tempTemperature <= tempValue && tempTemperature > targetTemp) {
                if (VERBOSE) {
                    logCardName();
                    printf("Temperature it's: %dC° - Waiting for temps to reach %dC°\n", tempTemperature, targetTemp);
                }
                tempTemperature = getNewTemperature(tempTemperature);
            }
        }
    }

    quit();
}