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
#include "ForksManager.h"

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
 * Call signal on various interrupting signals
*/
void childSignalsManagement() {
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
}
