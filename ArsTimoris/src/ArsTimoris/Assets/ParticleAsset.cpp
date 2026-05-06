#include <ArsTimoris/Assets/Assets.h>
#include <ArsTimoris/Game/TimeData.h>

#include <iostream>
#include <fstream>
#include <print>

namespace ArsTimoris {
    namespace Assets {
        enum class ModeOne {
            BASE_START,
            META_SEARCH,
            META_TEXTURE,
            META_TINT,
            META_ALPHA,
            META_BLEND,
            META_CENTER,
        };

        ParticleAsset::ParticleAsset(std::string a_id, std::string a_name) {
            this->id = a_id;
            this->name = a_name;
            this->texture = nullptr;
            this->instances = std::vector<Visual::Particle::ParticleData>();
            this->center = SDL_FPoint{0.5, 0.5};
            this->tintR = 255;
            this->tintG = 255;
            this->tintB = 255;
            this->tintA = 255;
            this->blendmode = SDL_BLENDMODE_BLEND;
        }
        
        void ParticleAsset::SpawnParticle(Visual::Particle::ParticleData a_particle) {
            instances.push_back(a_particle);
        }

        void ParticleAsset::Render(SDL_Renderer* a_renderer, TimeData* a_timeData) {
            if (instances.size() <= 0) {
                return;
            }

            SDL_FRect destination = SDL_FRect{0, 0, 0, 0};
            SDL_FPoint drawCenter = center;
            uint8_t oldR, oldG, oldB, oldA;
            SDL_BlendMode oldBlendmode;
            SDL_GetTextureColorMod(texture->texture, &oldR, &oldG, &oldB);
            SDL_GetTextureAlphaMod(texture->texture, &oldA);
            SDL_GetTextureBlendMode(texture->texture, &oldBlendmode);
            SDL_SetTextureColorMod(texture->texture, tintR, tintG, tintB);
            SDL_SetTextureAlphaMod(texture->texture, tintA);
            SDL_SetTextureBlendMode(texture->texture, blendmode);
            for (int32_t i = (int32_t)instances.size() - 1; i > 0; --i) {
                Visual::Particle::ParticleData& particle = instances[i];
                if ((particle.timeLeft -= a_timeData->deltaTime_s) < 0) {
                    particle = std::move(instances.back());
                    instances.pop_back();
                    continue;
                }
                destination = SDL_FRect{
                    (particle.position.x += particle.linearVelocity.x * a_timeData->deltaTime_s) - (drawCenter.x = center.x * particle.scale), 
                    (particle.position.y += particle.linearVelocity.y * a_timeData->deltaTime_s) - (drawCenter.y = center.y * particle.scale), 
                    texture->w * particle.scale, 
                    texture->h * particle.scale
                };
                SDL_RenderTextureRotated(a_renderer, texture->texture, NULL, &destination, (particle.angle += particle.angularVelocity * a_timeData->deltaTime_s), &drawCenter, SDL_FLIP_NONE);
            }
            SDL_SetTextureColorMod(texture->texture, oldR, oldG, oldB);
            SDL_SetTextureAlphaMod(texture->texture, oldA);
            SDL_SetTextureBlendMode(texture->texture, oldBlendmode);
        }

        /*
        meta:
            texture: WhitePixel;
            tint: 77, 77, 77;
            alpha: 255;
            blend: blend;
            center: 0.5, 0.5;
        ;
        */

        void ParticleAsset::Load(Assets* a_assets, std::string a_path) {
            std::vector<std::string> registers = std::vector<std::string>();
            std::string accumulator = "";
            std::ifstream file = std::ifstream(a_path);
            ModeOne currentModeOne = ModeOne::BASE_START;
            char c = ' ';
            while ((c = file.get()) != -1) {
                switch (currentModeOne) {
                    case ModeOne::BASE_START: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ':':
                                if (accumulator == "meta") {
                                    accumulator = "";
                                    currentModeOne = ModeOne::META_SEARCH;
                                }
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::META_SEARCH: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ':':
                                if (accumulator == "texture") {
                                    currentModeOne = ModeOne::META_TEXTURE;
                                } else if (accumulator == "tint") {
                                    currentModeOne = ModeOne::META_TINT;
                                } else if (accumulator == "alpha") {
                                    currentModeOne = ModeOne::META_ALPHA;
                                } else if (accumulator == "blend") {
                                    currentModeOne = ModeOne::META_BLEND;
                                } else if (accumulator == "center") {
                                    currentModeOne = ModeOne::META_CENTER;
                                } else {
                                    std::println("[PARTICLE_ASSET]: Particle {} Unrecognized Meta Tag '{}'.", name, accumulator);
                                }
                                accumulator = "";
                                break;
                            case ';':
                                currentModeOne = ModeOne::BASE_START;
                                accumulator = "";
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::META_TEXTURE: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ';':
                                texture = a_assets->textures.at(accumulator);
                                accumulator = "";
                                currentModeOne = ModeOne::META_SEARCH;
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::META_TINT: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ',':
                                registers.push_back(accumulator);
                                accumulator = "";
                                break;
                            case ';':
                                tintB = std::stoi(accumulator);
                                accumulator = "";
                                tintG = std::stoi(registers.back());
                                registers.pop_back();
                                tintR = std::stoi(registers.back());
                                registers.pop_back();
                                currentModeOne = ModeOne::META_SEARCH;
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::META_ALPHA: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ';':
                                tintA = std::stoi(accumulator);
                                accumulator = "";
                                currentModeOne = ModeOne::META_SEARCH;
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::META_BLEND: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ';':
                                if (accumulator == "add") {
                                    blendmode = SDL_BLENDMODE_ADD;
                                } else if (accumulator == "blend") {
                                    blendmode = SDL_BLENDMODE_BLEND;
                                } else if (accumulator == "mod") {
                                    blendmode = SDL_BLENDMODE_MOD;
                                } else if (accumulator == "mul") {
                                    blendmode = SDL_BLENDMODE_MUL;
                                } else if (accumulator == "none") {
                                    blendmode = SDL_BLENDMODE_NONE;
                                } else {
                                    std::println("[PARTICLE_ASSET]: Particle {} Unrecognized Blend Mode '{}'.", name, accumulator);
                                }
                                accumulator = "";
                                currentModeOne = ModeOne::META_SEARCH;
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::META_CENTER: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ',':
                                registers.push_back(accumulator);
                                accumulator = "";
                                break;
                            case ';':
                                center.y = std::stof(accumulator);
                                accumulator = "";
                                center.x = std::stof(registers.back());
                                registers.pop_back();
                                currentModeOne = ModeOne::META_SEARCH;
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                }
            }

            if (texture == nullptr) {
                std::println("[PARTICLE_ASSET]: Particle {} is missing a texture.", name);
            } else {
                center.x *= texture->w;
                center.y *= texture->h;
            }
        }

        void ParticleAsset::Unload(void) {
            texture.reset();
        }
    }
}