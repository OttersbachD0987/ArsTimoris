#pragma once
#ifndef ARS_TIMORIS__GAME__CLASSES__CLASS_LEVEL_H
#define ARS_TIMORIS__GAME__CLASSES__CLASS_LEVEL_H
#include <functional>
#include <vector>
#include <string>

struct GameState;
struct PlayerData;

struct ClassLevel {
public:
    std::function<std::string(GameState&, PlayerData&)> displayRequirements;
    std::function<bool(GameState&, PlayerData&)> applicable;
    std::vector<std::function<void(GameState&, PlayerData&, bool)>> levelUpEffects;
};
#endif