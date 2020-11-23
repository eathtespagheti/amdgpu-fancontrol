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

#include "parameter.h"

/**
 * Create a new parameter
 */
parameter newParameter(char name[], char value[]) {
    parameter p;
    p.name  = calloc(strlen(name), sizeof(*name));
    p.value = calloc(strlen(value), sizeof(*value));
    return p;
}

/**
 * Delete dinamically allocated fields in parameter
 */
void freeParameter(parameter p) {
    if (p.name != NULL) {
        free(p.name);
    }
    if (p.value != NULL) {
        free(p.value);
    }
}

/**
 * Convert to int and return parameter value
 */
int getIntValue(parameter p) {
    return atoi(p.value);
}