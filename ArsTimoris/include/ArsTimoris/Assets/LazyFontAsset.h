#pragma once
#ifndef LAZY_FONT_ASSET_H
#define LAZY_FONT_ASSET_H
#include <ArsTimoris/Assets/FontAsset.h>
#include <unordered_map>
#include <memory>

namespace ArsTimoris {
    namespace Assets {
        /// @brief 
        class LazyFontAsset: public Asset {
        public:
            /// @brief 
            /// @param  
            /// @param  
            LazyFontAsset(std::string a_id, std::string a_name);

            /// @brief 
            std::string path;
            /// @brief
            std::unordered_map<float, std::shared_ptr<FontAsset>> sizes;

            /// @brief 
            /// @param a_size 
            /// @return 
            std::shared_ptr<FontAsset> GetFontSize(float a_size);

            /// @brief 
            /// @param  
            void Load(std::string a_path);
            /// @brief 
            void Unload(void) override;
        };
    }
}
#endif