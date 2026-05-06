#pragma once
#ifndef NPC_RENDERER_ASSET_H
#define NPC_RENDERER_ASSET_H
#include <ArsTimoris/Assets/Asset.h>
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>
#include <ArsTimoris/Assets/TextureAsset.h>

namespace ArsTimoris {
    struct RenderPassData {
    public:
        SDL_FRect src;
        SDL_FRect rect;
        uint8_t alpha;
    };

    struct NPCRenderStep {
    public:
        std::vector<std::function<void(RenderPassData*, float)>> modifiers = std::vector<std::function<void(RenderPassData*, float)>>();

        virtual void Render(SDL_Renderer* a_renderer, SDL_FRect a_rect, float a_time) = 0;
        virtual ~NPCRenderStep() {};
    };

    struct NPCImageStep : public NPCRenderStep {
    public:
        std::shared_ptr<Assets::TextureAsset> image;
        uint8_t alpha;
        SDL_BlendMode blendmode;

        NPCImageStep(std::shared_ptr<Assets::TextureAsset> a_image, uint8_t a_alpha, SDL_BlendMode a_blendmode);
        void Render(SDL_Renderer* a_renderer, SDL_FRect a_rect, float a_time);
        ~NPCImageStep();
    };

    namespace Assets {
        class Assets;

        /// @brief 
        class NPCRendererAsset: public Asset {
        public:
            /// @brief 
            /// @param a_id 
            /// @param a_name 
            NPCRendererAsset(std::string a_id, std::string a_name);

            /// @brief 
            std::vector<std::unique_ptr<NPCRenderStep>> renderSteps;

            int32_t width;
            int32_t height;

            void Render(SDL_Renderer* a_renderer, SDL_FRect a_rect, float a_time);
            /// @brief 
            /// @param a_path 
            void Load(Assets* a_assets, std::string a_path);
            /// @brief 
            void Unload(void) override;
        };
    }
}
#endif