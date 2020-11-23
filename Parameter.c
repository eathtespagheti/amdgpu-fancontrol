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

#include "Parameter.h"

/**
 * Set value for a certain string
 */
static void setStringValue(char **string, char value[]) {
    if (*string != NULL) {
        free(*string);
    }

    if (!strcmp(value, "")) {
        *string = NULL;
    } else {
        *string = calloc(strlen(value) + 1, sizeof(*value));
        strcpy(*string, value);
    }
}

/**
 * Setter for value
 */
void setParameterValue(Parameter p, char value[]) {
    setStringValue(&p->Value, value);
}

/**
 * Setter for description
 */
void setParameterShortname(Parameter p, char shortname[]) {
    setStringValue(&p->Shortname, shortname);
}

/**
 * Create a new Parameter
 */
Parameter newParameter(char name[], char shortname[], char value[], char description[]) {
    Parameter p = malloc(sizeof(*p));

    p->Name = calloc(strlen(name) + 1, sizeof(*name));
    strcpy(p->Name, name);
    p->Description = calloc(strlen(description) + 1, sizeof(*description));
    strcpy(p->Description, description);

    p->Value     = NULL;
    p->Shortname = NULL;

    setParameterValue(p, value);
    setParameterShortname(p, shortname);

    return p;
}

/**
 * Free parameter from memory
 */
void freeParameter(Parameter p) {
    if (p != NULL) {
        if (p->Name != NULL) {
            free(p->Name);
        }
        if (p->Shortname != NULL) {
            free(p->Shortname);
        }
        if (p->Value != NULL) {
            free(p->Value);
        }
        if (p->Description != NULL) {
            free(p->Description);
        }
    }
    free(p);
}

/**
 * Convert to int and return Parameter value
 */
int getIntValue(Parameter p) {
    return p->Description == NULL ? 0 : atoi(p->Value);
}
