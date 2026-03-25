#pragma once
#ifndef ARS_TIMORIS__GAME__INPUT_DATA_H
#define ARS_TIMORIS__GAME__INPUT_DATA_H
#include <SDL3/SDL.h>

struct InputData {
public:
    int numKeys;

    SDL_MouseButtonFlags oldMouse;
    SDL_FPoint oldMousePos;
    bool* oldKeys;
    SDL_Keymod oldMods;

    SDL_MouseButtonFlags mouse;
    SDL_FPoint mousePos;
    const bool* keys;
    SDL_Keymod mods;
};
#endif