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
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char AMD_VENDOR[]   = "0x1002";
const char DEVICES_PATH[] = "/sys/class/drm";
const char CARD_PREFIX[]  = "card";

/**
 * Find a AMD GPU and return it's ID
*/
int findCardID() {
    /**
     * Command composed by command preset minus args +
     * DEVICES_PATH +
     * CARD_PREFIX * 2 +
     * EOL
    */
    char commandPreset[]     = "ls -1 %s | grep %s[0-9]*$ | sed {'s/%s//'}";
    uint8_t maxCommandLenght = strlen(commandPreset) - 6 + strlen(DEVICES_PATH) + strlen(CARD_PREFIX) * 2 + 1;
    char command[maxCommandLenght];
    snprintf(command, maxCommandLenght, commandPreset, DEVICES_PATH, CARD_PREFIX, CARD_PREFIX);

    // List all cards in the system without the CARD_PREFIX
    FILE *cmd = popen(command, "r");
    // Capped at max 100 cards
    char cardID[3] = {0x0};
    uint8_t IDs[100];
    int numberOfCards = 0;
    while (fgets(cardID, sizeof(cardID), cmd) != NULL) {
        IDs[numberOfCards++] = atoi(cardID);
    }
    // Close process output
    pclose(cmd);

    strcpy(commandPreset, "cat /sys/class/drm/%s%d/device/vendor");
    char cardVendor[strlen(CARD_PREFIX) + 2 + 1];
    maxCommandLenght = strlen(commandPreset) - 4 + strlen(CARD_PREFIX) + 2 + 1;
    // For every card
    for (size_t i = 0; i < numberOfCards; i++) {
        if (!fork()) { // Create a child for every card
            // Read vendor ID
            snprintf(command, maxCommandLenght, commandPreset, CARD_PREFIX, IDs[i]);
            cmd = popen(command, "r");
            fgets(cardVendor, sizeof(cardVendor), cmd);
            pclose(cmd);
            // Check if it's not AMD
            if (strcmp(cardVendor, AMD_VENDOR) != 0) {
                exit(0);
            }
            return IDs[i]; // Return valid AMD card ID
        }
    }
    exit(0);
}

int main(int argc, char const *argv[]) {
    printf("%s/card%d\n", DEVICES_PATH, findCardID());

    return 0;
}
