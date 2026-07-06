#pragma once

#include "GameSession.h"
#include "Singleton.h"

class GameManager : public Singleton<GameManager> {
public:
    GameSession* GetSession();

    void StartGame(GameMode mode, uint8_t presetIndex, uint8_t playerSetIndex);
    void StopGame();

    void RecordHit(uint8_t playerIndex);
    void RecordMiss(uint8_t playerIndex);
    void AdvanceTurn();
    void TickTimer();

private:
    friend class Singleton<GameManager>;
    GameManager();
    GameSession _session;
};
