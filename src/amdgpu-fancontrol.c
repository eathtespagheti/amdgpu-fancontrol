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

#include "ForksManager.h/ForksManager.h"

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