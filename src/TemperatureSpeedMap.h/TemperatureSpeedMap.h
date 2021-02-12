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

#ifndef TEMPERATURESPEEDMAP_H
#define TEMPERATURESPEEDMAP_H

// * DNT
#include "../CommonVariables.h/CommonVariables.h"
#include "../Logging.h/Logging.h"
#include "../ParametersManagement.h/ParametersManagement.h"
#define TEMPERATURE_STEPS 100
// * EDNT

uint8_t fanSpeedFunction(uint8_t temp);
uint8_t fanSpeedValue(uint8_t temp);
void buildTemperatureSpeedMap(uint8_t (*fanSpeedFunc)(uint8_t temp));
extern uint8_t TEMPERATURE_SPEED_MAP[TEMPERATURE_STEPS];

#endif // ! TEMPERATURESPEEDMAP_H
