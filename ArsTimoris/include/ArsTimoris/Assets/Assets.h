#pragma once
#ifndef ASSETS_H
#define ASSETS_H
#include <string>
#include <unordered_map>
#include <memory>
#include <ArsTimoris/Assets/AudioAsset.h>
#include <ArsTimoris/Assets/FontAsset.h>
#include <ArsTimoris/Assets/TextureAsset.h>
#include <ArsTimoris/Assets/LazyFontAsset.h>

namespace ArsTimoris {
    namespace UI {
        class UIManager;
    }

    namespace Assets {
        /// @brief 
        class Assets {
        public:
            /// @brief 
            Assets(void);

            /// @brief 
            std::unordered_map<std::string, std::shared_ptr<LazyFontAsset>> lazyFonts;
            /// @brief 
            std::unordered_map<std::string, std::shared_ptr<FontAsset>> fonts;
            /// @brief 
            std::unordered_map<std::string, std::shared_ptr<AudioAsset>> sounds;
            /// @brief 
            std::unordered_map<std::string, std::shared_ptr<TextureAsset>> textures;
            /// @brief 
            ArsTimoris::UI::UIManager* uiManager;

            /// @brief 
            /// @param  
            /// @param  
            /// @param  
            void AddTexture(SDL_Renderer*, std::string, std::string);
            /// @brief 
            /// @param  
            /// @param  
            /// @param  
            void AddFont(std::string, std::string, float);
            /// @brief 
            /// @param a_path 
            /// @param a_name 
            void AddLazyFont(std::string a_path, std::string a_name);
            /// @brief 
            /// @param  
            /// @param  
            /// @param  
            void AddSound(SDL_AudioDeviceID, std::string, std::string);
            /// @brief 
            void Uninitialize(void);
        };
    }
}
#endif