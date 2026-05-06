#pragma once
#ifndef ARS_TIMORIS__VISUAL__NPC__NPC_RENDER_STEP_H
#define ARS_TIMORIS__VISUAL__NPC__NPC_RENDER_STEP_H
#include <functional>
#include <vector>
#include <SDL3/SDL.h>

struct TimeData;

namespace ArsTimoris::Visual {
    struct RenderPassData;

    namespace NPC {
        struct NPCRenderStep {
        public:
            std::vector<std::function<void(RenderPassData*, TimeData*)>> modifiers = std::vector<std::function<void(RenderPassData*, TimeData*)>>();

            virtual void Render(SDL_Renderer* a_renderer, SDL_FRect a_rect, TimeData* a_timeData) = 0;
            virtual ~NPCRenderStep() {};
        };
    }
}
#endif