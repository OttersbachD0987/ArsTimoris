#pragma once
#ifndef ARS_TIMORIS__GAME__INTERACTION__ACTION_H
#define ARS_TIMORIS__GAME__INTERACTION__ACTION_H
#include <functional>
#include <string>

struct GameState;

#include <ArsTimoris/Game/Entity/EntityData.h>

/// @struct Action
/// @brief Holds the data for an action.
struct Action {
public:
    std::string name;                                                              //< The name of the action.
    std::function<bool(GameState&, EntityData*, EntityData*)> condition = nullptr; //< The condition of the action, if nullptr, then it has no condition.
    std::function<void(GameState&, EntityData*, EntityData*)> action = nullptr;    //< The action to execute, don't leave this null.

    bool operator==(const Action& other) {
        return this->name == other.name;
    }
};
#endif