#include "GameManager.h"
#include "Settings.h"
#include "GoalfinderApp.h"

GameManager::GameManager() : Singleton<GameManager>() {
    _session.isRunning = false;
}

GameSession* GameManager::GetSession() {
    return &_session;
}

void GameManager::StartGame(GameMode mode, uint8_t presetIndex, uint8_t playerSetIndex) {
    Settings* settings = Settings::GetInstance();

    GamePreset (*presets)[Settings::PRESETS_PER_MODE] = settings->GetGamePresets();
    GamePreset& preset = presets[static_cast<int>(mode)][presetIndex];

    _session.mode = mode;
    _session.activePresetIndex = presetIndex;
    _session.activePlayerSetIndex = playerSetIndex;

    PlayerSet* playerSets = settings->GetPlayerSets();
    PlayerSet& set = playerSets[playerSetIndex];
    _session.playerCount = 0;
    for (int i = 0; i < Settings::PLAYERS_PER_SET; i++) {
        if (set.players[i][0] != '\0') {
            _session.playerCount++;
        }
    }

    _session.currentPlayerIndex = 0;
    _session.currentRound = 1;
    _session.maxRounds = preset.rounds;
    _session.timePerTurn = preset.timePerTurn;
    _session.timer = preset.timePerTurn;

    for (int i = 0; i < 16; i++) {
        _session.entries[i].hit = 0;
        _session.entries[i].miss = 0;
    }

    _session.isRunning = true;

    GoalFinderApp::GetInstance()->SetIsDetecting(true);
    GoalFinderApp::GetInstance()->SetIsSoundEnabled(true);
}

void GameManager::StopGame() {
    _session.isRunning = false;
    GoalFinderApp::GetInstance()->SetIsDetecting(false);
    GoalFinderApp::GetInstance()->SetIsSoundEnabled(false);
}

void GameManager::RecordHit(uint8_t playerIndex) {
    if (!_session.isRunning) return;
    _session.entries[playerIndex].hit++;
    _session.timer = _session.timePerTurn;
    AdvanceTurn();
}

void GameManager::RecordMiss(uint8_t playerIndex) {
    if (!_session.isRunning) return;
    _session.entries[playerIndex].miss++;
    _session.timer = _session.timePerTurn;
    AdvanceTurn();
}

void GameManager::AdvanceTurn() {
    _session.currentPlayerIndex++;
    if (_session.currentPlayerIndex >= _session.playerCount) {
        _session.currentPlayerIndex = 0;
        _session.currentRound++;
        if (_session.mode == GameMode::BOARD_HITS && _session.currentRound > _session.maxRounds) {
            StopGame();
            return;
        }
    }
    _session.timer = _session.timePerTurn;
}

void GameManager::TickTimer() {
    if (!_session.isRunning || _session.timer <= 0) return;
    _session.timer--;
    if (_session.timer <= 0) {
        RecordMiss(_session.currentPlayerIndex);
    }
}
