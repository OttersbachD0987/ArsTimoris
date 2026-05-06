#pragma once
#ifndef PARTICLE_ASSET_H
#define PARTICLE_ASSET_H
#include <ArsTimoris/Assets/Asset.h>
#include <ArsTimoris/Visual/Particle/ParticleData.h>

#include <vector>
#include <memory>

struct TimeData;

namespace ArsTimoris {
    namespace Assets {
        class Assets;
        class TextureAsset;

        /// @brief 
        class ParticleAsset: public Asset {
        public:
            /// @brief 
            /// @param a_id 
            /// @param a_name 
            ParticleAsset(std::string a_id, std::string a_name);

            /// @brief 
            std::shared_ptr<TextureAsset> texture;
            std::vector<Visual::Particle::ParticleData> instances;
            SDL_FPoint center;
            uint8_t tintR;
            uint8_t tintG;
            uint8_t tintB;
            uint8_t tintA;
            SDL_BlendMode blendmode;

            void SpawnParticle(Visual::Particle::ParticleData a_particle);
            void Render(SDL_Renderer* a_renderer, TimeData* a_timeData);
            /// @brief 
            /// @param a_path 
            void Load(Assets* a_assets, std::string a_path);
            /// @brief 
            void Unload(void) override;
        };
    }
}
#endif