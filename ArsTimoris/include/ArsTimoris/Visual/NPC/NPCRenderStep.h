#pragma once
#ifndef ARS_TIMORIS__VISUAL__NPC__NPC_RENDER_STEP_H
#define ARS_TIMORIS__VISUAL__NPC__NPC_RENDER_STEP_H
#include <functional>
#include <vector>
#include <SDL3/SDL.h>

namespace ArsTimoris::Visual {
    struct RenderPassData;

    namespace NPC {
        struct NPCRenderStep {
        public:
            std::vector<std::function<void(RenderPassData*, float)>> modifiers = std::vector<std::function<void(RenderPassData*, float)>>();

            virtual void Render(SDL_Renderer* a_renderer, SDL_FRect a_rect, float a_time) = 0;
            virtual ~NPCRenderStep() {};
        };
    }
}
#endif