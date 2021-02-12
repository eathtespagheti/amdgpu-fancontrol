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

#ifndef CARDSMANAGER_H
#define CARDSMANAGER_H

// * DNT
#include "../ForksManager.h/ForksManager.h"
#include "../TemperatureSpeedMap.h/TemperatureSpeedMap.h"
#include <unistd.h>
// * EDNT

uint8_t getNewTemperature(uint8_t lastTemperature);
uint8_t getTempValue();
int8_t findHwmon(uint8_t cardID);
int8_t findCardID();
void setFanSpeed(uint8_t value);
void enableManualFan(uint8_t cardID, uint8_t hwmonID);
void restoreAutoFan();
void writeToPwmEnable(uint8_t cardID, uint8_t hwmonID, uint8_t value);
void writeToPWM(uint8_t cardID, uint8_t hwmonID, uint8_t value, const char pwmControl[]);

#endif // ! CARDSMANAGER_H
