#pragma once
#ifndef ARS_TIMORIS__VISUAL__RENDER_PASS_DATA_H
#define ARS_TIMORIS__VISUAL__RENDER_PASS_DATA_H
#include <cstdint>
#include <SDL3/SDL.h>

namespace ArsTimoris::Visual {
    struct RenderPassData {
    public:
        SDL_FRect src;
        SDL_FRect rect;
        uint8_t alpha;
    };
}
#endif