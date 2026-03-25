#include <ArsTimoris/Game/Map/ConnectionInstance.h>
#include <ArsTimoris/Game/GameState.h>

bool ConnectionInstance::CanPass(GameState& a_gameState) {
    bool currentResult = true;
    bool arrivalResult = true;

    if (currentTest.condition != nullptr) {
        currentResult = currentTest.condition(a_gameState, a_gameState.rooms[destination]);
    }

    if (arrivalTest.condition != nullptr) {
        arrivalResult = arrivalTest.condition(a_gameState, a_gameState.rooms[destination]);
    }

    if (currentTest.conditionDisplay != nullptr) {
        currentTest.conditionDisplay(a_gameState, a_gameState.rooms[destination], currentResult);
    }
    
    if (arrivalTest.conditionDisplay != nullptr) {
        arrivalTest.conditionDisplay(a_gameState, a_gameState.rooms[destination], arrivalResult);
    }

    return currentResult && arrivalResult;
}

void ConnectionInstance::Passes(GameState& a_gameState) {
    if (currentTest.action != nullptr) {
        currentTest.action(a_gameState, a_gameState.rooms[destination]);
    }

    if (arrivalTest.action != nullptr) {
        arrivalTest.action(a_gameState, a_gameState.rooms[destination]);
    }
}