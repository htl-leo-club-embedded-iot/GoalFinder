#pragma once

#include <stdint.h>

struct PlayerEntry {
    uint8_t hit = 0;
    uint8_t miss = 0;
};

struct GameState {
    PlayerEntry entries[16];
};
