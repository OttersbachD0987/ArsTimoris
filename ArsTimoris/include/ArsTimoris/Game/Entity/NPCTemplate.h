#pragma once
#ifndef ARS_TIMORIS__GAME__ENTITY__NPC_TEMPLATE_H
#define ARS_TIMORIS__GAME__ENTITY__NPC_TEMPLATE_H
#include <functional>
#include <cstdint>

struct GameState;
struct NPCData;

/// @struct NPCTemplate
/// @brief A template holding the outline for an NPC instance.
struct NPCTemplate {
public:
    int32_t xp = 0;
    int32_t gold = 0;
    std::function<void(GameState&, NPCData&)> ai = nullptr;
};
#endif