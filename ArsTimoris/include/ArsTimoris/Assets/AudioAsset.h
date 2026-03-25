#pragma once
#ifndef AUDIO_ASSET_H
#define AUDIO_ASSET_H
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <ArsTimoris/Assets/Asset.h>
#include <cstdint>

namespace ArsTimoris {
    namespace Assets {
        /// @brief 
        class AudioAsset: public Asset {
        public:
            /// @brief 
            /// @param a_id 
            /// @param a_name 
            AudioAsset(std::string a_id, std::string a_name);

            /// @brief 
            SDL_AudioSpec wavSpec;
            /// @brief 
            uint8_t* wavStart;
            /// @brief 
            uint32_t wavLength;
            /// @brief 
            SDL_AudioStream *stream;

            /// @brief 
            /// @param a_audioDevice 
            /// @param a_path 
            void Load(SDL_AudioDeviceID a_audioDevice, std::string a_path);
            /// @brief 
            /// @param a_audioDevice 
            void Play(SDL_AudioDeviceID a_audioDevice);
            /// @brief 
            void Unload(void) override;
        };
    }
}
#endif