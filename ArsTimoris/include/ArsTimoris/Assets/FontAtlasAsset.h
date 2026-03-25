#pragma once
#ifndef FONT_ATLAS_ASSET_H
#define FONT_ATLAS_ASSET_H
#include <memory>
#include <ArsTimoris/Assets/Asset.h>
#include <cstdint>

namespace ArsTimoris::UI:Text {
    struct FontAtlas;
}

namespace ArsTimoris::Assets {
    /// @brief a
    class FontAtlasAsset: public Asset {
    public:
        /// @brief 
        /// @param  
        /// @param  
        /// @param  
        FontAtlasAsset(std::string, std::string);

        /// @brief 
        std::shared_ptr<ArsTimoris::UI:Text::FontAtlas> fontAtlas;

        /// @brief 
        /// @param  
        void Load(std::string);
        /// @brief 
        void Unload(void) override;
    };
}
#endif