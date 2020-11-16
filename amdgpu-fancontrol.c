/**
 * Copyright (C) 2020 Fabio Sussarellu
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
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

const char AMD_VENDOR[]   = "0x1002";
const char DEVICES_PATH[] = "/sys/class/drm";
const char HWMON_PREFIX[] = "hwmon";
const char HWMON_PATH[]   = "device/hwmon";
const char CARD_PREFIX[]  = "card";
const char TEMP_SENSOR[]  = "temp1_input";
const char PWM_ENABLE[]   = "pwm1_enable";
const char PWM[]          = "pwm1";
uint8_t ZERO_RMP_ZONE     = 40;
uint8_t TEMP_VALUE_100    = 64;
uint8_t CONTROL_FREQUENCY = 1;
int8_t ENABLED_CARD       = -1;
int8_t CURRENT_HWMON      = -1;
struct timespec WAIT_TIME = {
    1, 0};

/**
 * Return fan speed value in percentage based on the card temperature
*/
uint8_t fanSpeeValue(uint8_t temp) {
    if (temp <= ZERO_RMP_ZONE) {
        return 0;
    }
    uint8_t fanSpeed = (100 / (TEMP_VALUE_100)) * temp;
    return fanSpeed > 100 ? 100 : fanSpeed;
}

/**
 * echo value inside the pwmControl file
*/
void writeToPWM(uint8_t cardID, uint8_t hwmonID, uint8_t value, char pwmControl[]) {
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
}

/**
 * Manage signal that require stopping or killing amdgpu-fancontrol
*/
void signalHandler(int signal) {
    printf("Received signal %d\n", signal);
    restoreAutoFan();
    kill(0, signal);
    exit(0);
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
    char commandPreset[]     = "ls -1 %s | grep %s[0-9]*$ | sed {'s/%s//'}";
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
            // Check if it's not AMD
            if (strcmp(cardVendor, AMD_VENDOR) != 0) {
                exit(0);
            }
            return IDs[i]; // Return valid AMD card ID
        } else {
            while (wait > 0)
                ;
        }
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
        return -1; // In case of missing hwmon
    }
    pclose(cmd);
    return atoi(hwmonID);
}

int main(int argc, char const *argv[]) {
    // Manage signals
    signal(SIGABRT, signalHandler);
    signal(SIGSTOP, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandler);
    
    // Parse args
    if (argc > 1) {
        TEMP_VALUE_100 = atoi(argv[1]);
        if (argc > 2) {
            ZERO_RMP_ZONE = atoi(argv[2]);
            if (argc > 3) {
                CONTROL_FREQUENCY = atoi(argv[3]);
            }
        }
    }
    WAIT_TIME.tv_sec  = 1 / CONTROL_FREQUENCY;
    WAIT_TIME.tv_nsec = ((1.0 / CONTROL_FREQUENCY) - WAIT_TIME.tv_sec) * 1000000000;
    printf("Max fan temp: %dC°\nZero RMP temp: %dC°\nWait time it's %d seconds and %d nanoseconds ", TEMP_VALUE_100, ZERO_RMP_ZONE, WAIT_TIME.tv_sec, WAIT_TIME.tv_nsec);

    // Find card
    int8_t cardID        = findCardID();
    uint8_t stringLenght = strlen(DEVICES_PATH) + 1 + strlen(CARD_PREFIX) + 2;
    char cardPath[stringLenght + 1];
    snprintf(cardPath, stringLenght, "%s/%s%d\n", DEVICES_PATH, CARD_PREFIX, cardID);

    // Find hwmon
    int8_t hwmonID = findHwmon(cardID);
    stringLenght   = strlen(cardPath) + 1 + strlen(HWMON_PATH) + 1 + strlen(HWMON_PREFIX) + 2;
    char hwmonControllerPath[stringLenght + 1];
    snprintf(hwmonControllerPath, stringLenght, "%s/%s/%s%d", cardPath, HWMON_PATH, HWMON_PREFIX, hwmonID);

    // Temperature reading loop
    char commandPreset[] = "cat %s/%s";
    stringLenght         = strlen(commandPreset) - 2 * 2 + strlen(hwmonControllerPath) + 1 + strlen(TEMP_SENSOR);
    char command[stringLenght + 1];
    snprintf(command, stringLenght, commandPreset, hwmonControllerPath, TEMP_SENSOR);
    FILE *tempReadingOutput;
    char temperature[3];
    uint8_t tempValue, fanspeed, newfanspeed;

    // Start monitoring card temperature
    enableManualFan(cardID, hwmonID);
    fanspeed = 0;
    setFanSpeed(fanspeed);
    while (1) {
        tempReadingOutput = popen(command, "r");
        fgets(temperature, sizeof(temperature), tempReadingOutput);
        tempValue   = atoi(temperature);
        newfanspeed = fanSpeeValue(tempValue);
        if (newfanspeed != fanspeed) {
            setFanSpeed(newfanspeed);
            fanspeed = newfanspeed;
        }
        printf("Temperature it's: %dC° - Fan speed it's %d%\n", tempValue, fanspeed);
        pclose(tempReadingOutput);
        nanosleep(&WAIT_TIME, &WAIT_TIME);
    }
    restoreAutoFan();

    exit(0);
}
