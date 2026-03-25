#pragma once
#ifndef ARS_TIMORIS__GAME__MAP__CONNECTION_TEST_H
#define ARS_TIMORIS__GAME__MAP__CONNECTION_TEST_H
#include <functional>

struct GameState;
struct RoomInstance;

struct ConnectionTest {
public:
    std::function<bool(GameState&, RoomInstance&)>       condition = nullptr;
    std::function<void(GameState&, RoomInstance&, bool)> conditionDisplay = nullptr;
    std::function<void(GameState&, RoomInstance&)>       action = nullptr;
};
#endif