#pragma once

#include <stdint.h>
#include "GamePreset.h"

struct PlayerEntry {
    uint8_t hit = 0;
    uint8_t miss = 0;
};

struct GameSession {
    GameMode mode;
    uint8_t activePresetIndex;
    uint8_t activePlayerSetIndex;
    bool isRunning;
    uint8_t currentPlayerIndex;
    uint8_t playerCount;
    int16_t timer;
    uint8_t currentRound;
    uint8_t maxRounds;
    uint16_t timePerTurn;
    PlayerEntry entries[16];
};
