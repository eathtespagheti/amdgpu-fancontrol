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

#include "Parameter.h"
static int __PARAMETER_H__PARAMETER_PREFIX_LENGHT       = -1;
static int __PARAMETER_H__SHORT_PARAMETER_PREFIX_LENGHT = -1;

/**
 * Set value for a certain string
 */
static void setStringValue(char **string, const char value[]) {
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
void setParameterValue(Parameter p, const char value[]) {
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
int getParameterIntValue(Parameter p) {
    return p->Value == NULL ? 0 : atoi(p->Value);
}

/**
 * Convert to double and return Parameter value
 */
double getParameterFloatValue(Parameter p) {
    return p->Value == NULL ? 0 : atof(p->Value);
}

/**
 * Convert to bool and return Parameter value
 */
bool getParameterBoolValue(Parameter p) {
    if (p->Value == NULL) {
        return false;
    }

    if (strcmp(p->Value, "1") == 0) {
        return true;
    }

    if (strcmp(p->Value, "true") == 0) {
        return true;
    }

    return false;
}

// Return true if arg string matcher parameter name or shortname
bool compareParameter(Parameter p, const char *arg) {
    // Compare shortparameter name
    if (__PARAMETER_H__SHORT_PARAMETER_PREFIX_LENGHT == -1) {
        __PARAMETER_H__SHORT_PARAMETER_PREFIX_LENGHT = strlen(SHORT_PARAMETER_PREFIX);
    }
    if (strcmp(p->Shortname, &(arg[__PARAMETER_H__SHORT_PARAMETER_PREFIX_LENGHT])) == 0) {
        return true;
    }

    // Compare full parameter name
    if (__PARAMETER_H__PARAMETER_PREFIX_LENGHT == -1) {
        __PARAMETER_H__PARAMETER_PREFIX_LENGHT = strlen(PARAMETER_PREFIX);
    }
    if (strcmp(p->Name, &(arg[__PARAMETER_H__PARAMETER_PREFIX_LENGHT])) == 0) {
        return true;
    }

    return false;
}