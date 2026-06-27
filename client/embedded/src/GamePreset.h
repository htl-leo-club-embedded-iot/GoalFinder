#pragma once

#include <stdint.h>
#include <string.h>

enum class GameMode : uint8_t {
    FREE_PLAY = 0,
    TIMED_SHOTS = 1,
    BOARD_HITS = 2
};

struct GamePreset {
    char name[17];
    uint8_t rounds;
    uint8_t timePerTurn;

    GamePreset() {
        name[0] = '\0';
        rounds = 0;
        timePerTurn = 0;
    }

    GamePreset(const char* n, uint8_t r, uint8_t t) {
        size_t len = strlen(n);
        if (len > 16) {
            len = 16;
        }
        memcpy(name, n, len);
        name[len] = '\0';
        rounds = r;
        timePerTurn = t;
    }
};
