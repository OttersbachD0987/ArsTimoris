#pragma once
#ifndef ARS_TIMORIS__GAME__INTERACTION__TIMED_EFFECT_H
#define ARS_TIMORIS__GAME__INTERACTION__TIMED_EFFECT_H
#include <cstdint>
#include <functional>

struct GameState;
struct EntityData;

struct TimedEffect {
public:
    int32_t timeLeft;
    std::function<void(GameState&, EntityData&)> onTick = nullptr;
    std::function<void(GameState&, EntityData&)> onRemove = nullptr;
};
#endif