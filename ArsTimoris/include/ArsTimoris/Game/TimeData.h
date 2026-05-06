#pragma once
#ifndef ARS_TIMORIS__GAME__TIME_DATA_H
#define ARS_TIMORIS__GAME__TIME_DATA_H
#include <cstdint>

struct TimeData {
public:
    uint64_t deltaTime_ms;
    uint64_t lastTime_ms;
    float totalTime_s;
    float deltaTime_s;
};
#endif