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
#include "CardsManager.h"

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