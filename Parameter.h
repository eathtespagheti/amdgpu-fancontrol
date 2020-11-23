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

#ifndef PARAMETER_H
#define PARAMETER_H

// * DNT
#include <stdlib.h>
#include <string.h>
static const char PARAMETER_PREFIX[]       = "--";
static const char SHORT_PARAMETER_PREFIX[] = "-";

typedef struct parameter_h {
    char *Name;
    char *Shortname;
    char *Value;
    char *Description;
} * Parameter;

// * EDNT

int getIntValue(Parameter p);
void freeParameter(Parameter p);
Parameter newParameter(char name[], char shortname[], char value[], char description[]);
void setParameterShortname(Parameter p, char shortname[]);
void setParameterValue(Parameter p, char value[]);

#endif // ! PARAMETER_H
