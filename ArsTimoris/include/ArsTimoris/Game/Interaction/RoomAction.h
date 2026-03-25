#pragma once
#ifndef ARS_TIMORIS__GAME__INTERACTION__ROOM_ACTION_H
#define ARS_TIMORIS__GAME__INTERACTION__ROOM_ACTION_H
#include <functional>
#include <string>

struct GameState;

struct RoomAction {
public:
    std::string name        = "";
    std::string description = "";
    std::function<void(GameState&)> roomDescription = nullptr;
    std::function<bool(GameState&)> condition       = nullptr;
    std::function<void(GameState&)> usage           = nullptr;
};
#endif