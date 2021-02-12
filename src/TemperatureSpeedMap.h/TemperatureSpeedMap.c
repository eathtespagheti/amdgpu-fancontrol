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

#include "TemperatureSpeedMap.h"

uint8_t TEMPERATURE_SPEED_MAP[TEMPERATURE_STEPS];

/**
 * Build temperature speed map
 */
void buildTemperatureSpeedMap(uint8_t (*fanSpeedFunc)(uint8_t temp)) {
    if (VERBOSE) {
        logCardName();
        printf("Generating Temperature/Speed Map\n");
    }

    for (uint8_t i = 0; i < TEMPERATURE_STEPS; i++) {
        TEMPERATURE_SPEED_MAP[i] = fanSpeedFunc((i / STEPS_DELTA) * STEPS_DELTA);
    }
    // Manual step for zero rpm mode
    for (uint8_t i = ZERO_RMP_ZONE; i < ZERO_RMP_ZONE + ZERO_RMP_DELTA; i++) {
        TEMPERATURE_SPEED_MAP[i] = fanSpeedFunc(i);
    }

    if (VERBOSE) {
        uint8_t prevSpeed = -1;
        for (uint8_t i = 0; i < TEMPERATURE_STEPS; i++) {
            if (TEMPERATURE_SPEED_MAP[i] != prevSpeed) {
                prevSpeed = TEMPERATURE_SPEED_MAP[i];
                logCardName();
                printf("Fan speed for %dC° it's %d%\n", i, TEMPERATURE_SPEED_MAP[i]);
            }
        }
    }
}

/**
 * Return fan speed value in percentage based on the card temperature
*/
uint8_t fanSpeedValue(uint8_t temp) {
    if (temp >= TEMPERATURE_STEPS) {
        temp = TEMPERATURE_STEPS - 1;
    }
    return TEMPERATURE_SPEED_MAP[temp];
}

/**
 * Calculate and return fan speed value in percentage based on the card temperature
*/
uint8_t fanSpeedFunction(uint8_t temp) {
    if (temp <= ZERO_RMP_ZONE) {
        return 0;
    }
    uint8_t fanSpeed = (100.0 / (MAX_FAN_TEMP)) * temp;
    return fanSpeed > 100 ? 100 : fanSpeed;
}