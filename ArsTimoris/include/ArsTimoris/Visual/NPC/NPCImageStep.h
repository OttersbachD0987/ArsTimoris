#pragma once
#ifndef ARS_TIMORIS__VISUAL__NPC__NPC_IMAGE_STEP_H
#define ARS_TIMORIS__VISUAL__NPC__NPC_IMAGE_STEP_H
#include <ArsTimoris/Visual/NPC/NPCRenderStep.h>
#include <memory>

namespace ArsTimoris {
    namespace Assets {
        class TextureAsset;
    }

    namespace Visual::NPC {
        struct NPCImageStep : public NPCRenderStep {
        public:
            std::shared_ptr<Assets::TextureAsset> image;
            uint8_t alpha;
            SDL_BlendMode blendmode;

            NPCImageStep(std::shared_ptr<Assets::TextureAsset> a_image, uint8_t a_alpha, SDL_BlendMode a_blendmode);
            void Render(SDL_Renderer* a_renderer, SDL_FRect a_rect, float a_time);
            ~NPCImageStep();
        };
    }
}
#endif