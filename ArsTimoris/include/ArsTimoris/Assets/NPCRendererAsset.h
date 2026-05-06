#pragma once
#ifndef NPC_RENDERER_ASSET_H
#define NPC_RENDERER_ASSET_H
#include <ArsTimoris/Assets/Asset.h>
#include <vector>
#include <cstdint>
#include <memory>
#include <ArsTimoris/Visual/NPC/NPCRenderStep.h>

namespace ArsTimoris {
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
            std::vector<std::unique_ptr<Visual::NPC::NPCRenderStep>> renderSteps;

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