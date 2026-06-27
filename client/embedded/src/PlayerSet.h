#pragma once

#include <stdint.h>
#include <string.h>

struct PlayerSet {
    char name[17];
    char players[16][17];

    PlayerSet() {
        name[0] = '\0';
        for (int i = 0; i < 16; i++) {
            players[i][0] = '\0';
        }
    }

    PlayerSet(const char* setName, const char* const* playerNames, int playerCount) {
        size_t setNameLen = strlen(setName);
        if (setNameLen > 16) {
            setNameLen = 16;
        }
        memcpy(name, setName, setNameLen);
        name[setNameLen] = '\0';

        for (int i = 0; i < 16; i++) {
            if (i < playerCount && playerNames[i] != nullptr) {
                size_t playerNameLen = strlen(playerNames[i]);
                if (playerNameLen > 16) {
                    playerNameLen = 16;
                }
                memcpy(players[i], playerNames[i], playerNameLen);
                players[i][playerNameLen] = '\0';
            } else {
                players[i][0] = '\0';
            }
        }
    }
};
