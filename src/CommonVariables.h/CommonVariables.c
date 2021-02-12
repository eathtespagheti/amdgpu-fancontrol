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

#include "CommonVariables.h"

// Paths
const char AMD_VENDOR[]       = "0x1002";
const char DEVICES_PATH[]     = "/sys/class/drm";
const char HWMON_PREFIX[]     = "hwmon";
const char HWMON_PATH[]       = "device/hwmon";
const char CARD_PREFIX[]      = "card";
const char TEMP_SENSOR[]      = "temp1_input";
const char PWM_ENABLE[]       = "pwm1_enable";
const char PWM[]              = "pwm1";
const char CONFIG_FILE_PATH[] = "/etc/amd-fancontrol.conf";

// Variables
int8_t ENABLED_CARD        = -1;
int8_t CURRENT_HWMON       = -1;
char *READ_TEMP_COMMAND    = NULL;
char *TEMP_SENSOR_LOCATION = NULL;