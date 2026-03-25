#pragma once
#ifndef FONT_ASSET_H
#define FONT_ASSET_H
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <ArsTimoris/Assets/Asset.h>
#include <cstdint>

namespace ArsTimoris {
    namespace Assets {
        /// @brief a
        /// @deprecated FontAsset is superceded by LazyFontAsset for access, use that instead of directly accessing FontAsset.
        class FontAsset: public Asset {
        public:
            /// @brief 
            /// @param  
            /// @param  
            /// @param  
            FontAsset(std::string, std::string, float);

            /// @brief 
            TTF_Font* font;
            /// @brief 
            float ptsize;

            /// @brief 
            /// @param  
            void Load(std::string);
            /// @brief 
            void Unload(void) override;
        };
    }
}
#endif