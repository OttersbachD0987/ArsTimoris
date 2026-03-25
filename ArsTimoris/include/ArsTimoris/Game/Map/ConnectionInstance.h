#pragma once
#ifndef ARS_TIMORIS__GAME__MAP__CONNECTION_INSTANCE_H
#define ARS_TIMORIS__GAME__MAP__CONNECTION_INSTANCE_H
#include <ArsTimoris/Game/Map/ConnectionTest.h>

struct ConnectionInstance {
public:
    size_t destination;
    ConnectionTest currentTest;
    ConnectionTest arrivalTest;

    bool CanPass(GameState& a_gameState);

    void Passes(GameState& a_gameState);
};
#endif