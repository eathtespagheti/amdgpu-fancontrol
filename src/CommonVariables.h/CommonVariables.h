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

#ifndef COMMONVARIABLES_H
#define COMMONVARIABLES_H

// * DNT
#include <inttypes.h>
#include <stdlib.h>
// * EDNT

extern char *TEMP_SENSOR_LOCATION;
extern char *READ_TEMP_COMMAND;
extern int8_t CURRENT_HWMON;
extern int8_t ENABLED_CARD;
extern const char CONFIG_FILE_PATH[];
extern const char PWM[];
extern const char PWM_ENABLE[];
extern const char TEMP_SENSOR[];
extern const char CARD_PREFIX[];
extern const char HWMON_PATH[];
extern const char HWMON_PREFIX[];
extern const char DEVICES_PATH[];
extern const char AMD_VENDOR[];

#endif // ! COMMONVARIABLES_H
