#include <ArsTimoris/Assets/Assets.h>
#include <iostream>
#include <fstream>
#include <print>
#include <ArsTimoris/Util/Input.hpp>

namespace ArsTimoris {
    NPCImageStep::NPCImageStep(std::shared_ptr<Assets::TextureAsset> a_image, uint8_t a_alpha, SDL_BlendMode a_blendmode) {
        this->image = a_image;
        this->alpha = a_alpha;
        this->blendmode = a_blendmode;
    }

    void NPCImageStep::Render(SDL_Renderer* a_renderer, SDL_FRect a_rect, float a_time) {
        uint8_t oldAlpha;
        SDL_BlendMode oldBlendmode;
        SDL_GetTextureAlphaMod(image->texture, &oldAlpha);
        SDL_GetTextureBlendMode(image->texture, &oldBlendmode);
        RenderPassData renderPassData = {{0, 0, image->w, image->h}, a_rect, alpha};
        for (std::function<void(RenderPassData*, float)> func : modifiers) {
            func(&renderPassData, a_time);
        }
        SDL_SetTextureAlphaMod(image->texture, renderPassData.alpha);
        SDL_SetTextureBlendMode(image->texture, blendmode);
        SDL_RenderTexture(a_renderer, image->texture, &renderPassData.src, &renderPassData.rect);
        SDL_SetTextureAlphaMod(image->texture, oldAlpha);
        SDL_SetTextureBlendMode(image->texture, oldBlendmode);
    }

    NPCImageStep::~NPCImageStep() {
        image.reset();
    }

    namespace Assets {
        enum class ModeOne {
            STEP_START,
            STEP_PARSING,
            STEP_TYPE,
            STEP_DATA,
            STEP_DONE,
            STEP_MODS,
            META_WIDTH,
            META_HEIGHT,
        };

        NPCRendererAsset::NPCRendererAsset(std::string a_id, std::string a_name) {
            this->id = a_id;
            this->name = a_name;
        }
        
        void NPCRendererAsset::Render(SDL_Renderer* a_renderer, SDL_FRect a_rect, float a_time) {
            for (std::unique_ptr<NPCRenderStep>& renderStep : renderSteps) {
                renderStep->Render(a_renderer, a_rect, a_time);
            }
        }

        void NPCRendererAsset::Load(Assets* a_assets, std::string a_path) {
            std::vector<std::string> registers = std::vector<std::string>();
            std::string accumulator = "";
            std::string renderType = "";
            std::string modifierType = "";
            std::ifstream file = std::ifstream(a_path);
            std::unique_ptr<NPCRenderStep> renderStep = nullptr;
            ModeOne currentModeOne = ModeOne::STEP_START;
            //std::println("Reading: {}", a_path);
            //std::cout << "Press any key to continue..." << std::flush;
            //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            //std::cout << "\x1B[F\x1B[0K\r" << std::flush;
            char c = ' ';
            while ((c = file.get()) != -1) {
                //std::println("Parsing: [{}] [{}] [{}]", (int32_t)currentModeOne, c, (int64_t)file.tellg());
                switch (currentModeOne) {
                    case ModeOne::STEP_START: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ':':
                                if (accumulator == "steps") {
                                    accumulator = "";
                                    currentModeOne = ModeOne::STEP_PARSING;
                                } else if (accumulator == "meta") {
                                    accumulator = "";
                                    currentModeOne = ModeOne::META_WIDTH;
                                }
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::STEP_PARSING: {
                        switch (c) {
                            case '-':
                                currentModeOne = ModeOne::STEP_TYPE;
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                    case ModeOne::STEP_TYPE: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case '(':
                                renderType = accumulator;
                                accumulator = "";
                                currentModeOne = ModeOne::STEP_DATA;
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::STEP_DATA: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ')':
                                accumulator = "";
                                if (renderType == "Image") {
                                    SDL_BlendMode readBlendmode = 0;
                                    if (registers.back() == "blend") {
                                        readBlendmode = SDL_BLENDMODE_BLEND;
                                    } else if (registers.back() == "add") {
                                        readBlendmode = SDL_BLENDMODE_ADD;
                                    }
                                    registers.pop_back();
                                    uint8_t readAlpha = std::stoi(registers.back());
                                    registers.pop_back();
                                    std::string readTexture = registers.back();
                                    registers.pop_back();
                                    renderStep = std::make_unique<NPCImageStep>(a_assets->textures.at(readTexture), readAlpha, readBlendmode);
                                }
                                renderType = "";
                                currentModeOne = ModeOne::STEP_DONE;
                                break;
                            case ';':
                                registers.push_back(accumulator);
                                accumulator = "";
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::STEP_DONE: {
                        switch (c) {
                            case '[':
                                accumulator = "";
                                currentModeOne = ModeOne::STEP_MODS;
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                    case ModeOne::STEP_MODS: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case '(':
                                modifierType = accumulator;
                                accumulator = "";
                                break;
                            case ']':
                                accumulator = "";
                                renderSteps.push_back(std::move(renderStep));
                                currentModeOne = ModeOne::STEP_PARSING;
                                break;
                            case ')':
                            case ',':
                                registers.push_back(accumulator);
                                accumulator = "";
                                break;
                            case ';':
                                if (modifierType == "sway_sin_x") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](RenderPassData* a_renderPassData, float a_time) {
                                        a_renderPassData->rect.x += readAmplitude * sinf(a_time * readFrequency * 3.14f);
                                    });
                                } else if (modifierType == "sway_sin_y") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](RenderPassData* a_renderPassData, float a_time) {
                                        a_renderPassData->rect.y += readAmplitude * sinf(a_time * readFrequency * 3.14f);
                                    });
                                } else if (modifierType == "sway_cos_x") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](RenderPassData* a_renderPassData, float a_time) {
                                        a_renderPassData->rect.x += readAmplitude * cosf(a_time * readFrequency * 3.14f);
                                    });
                                } else if (modifierType == "sway_cos_y") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](RenderPassData* a_renderPassData, float a_time) {
                                        a_renderPassData->rect.y += readAmplitude * cosf(a_time * readFrequency * 3.14f);
                                    });
                                } else if (modifierType == "offset_x") {
                                    float readOffset = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readOffset](RenderPassData* a_renderPassData, float a_time) {
                                        a_renderPassData->rect.x += readOffset;
                                    });
                                } else if (modifierType == "offset_y") {
                                    float readOffset = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readOffset](RenderPassData* a_renderPassData, float a_time) {
                                        a_renderPassData->rect.y += readOffset;
                                    });
                                } else if (modifierType == "sway_sin_a") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](RenderPassData* a_renderPassData, float a_time) {
                                        a_renderPassData->alpha += (int8_t)roundf(readAmplitude * sinf(a_time * readFrequency * 3.14f));
                                    });
                                } else if (modifierType == "sway_cos_a") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](RenderPassData* a_renderPassData, float a_time) {
                                        a_renderPassData->alpha += (int8_t)roundf(readAmplitude * cosf(a_time * readFrequency * 3.14f));
                                    });
                                } else if (modifierType == "animate_x") {
                                    float readSeconds = std::stof(registers.back());
                                    registers.pop_back();
                                    int32_t readFrames = std::stoi(registers.back());
                                    registers.pop_back();
                                    int32_t readFrameSize = std::stoi(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readSeconds, readFrames, readFrameSize](RenderPassData* a_renderPassData, float a_time) {
                                        a_renderPassData->src.x += readFrameSize * ((int32_t)floorf(a_time / readSeconds) % readFrames);
                                        a_renderPassData->src.w = (float)readFrameSize;
                                    });
                                } else if (modifierType == "animate_y") {
                                    float readSeconds = std::stof(registers.back());
                                    registers.pop_back();
                                    int32_t readFrames = std::stoi(registers.back());
                                    registers.pop_back();
                                    int32_t readFrameSize = std::stoi(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readSeconds, readFrames, readFrameSize](RenderPassData* a_renderPassData, float a_time) {
                                        std::println("Prae: ({}, {}, {}, {})", a_renderPassData->src.x, a_renderPassData->src.y, a_renderPassData->src.w, a_renderPassData->src.h);
                                        a_renderPassData->src.y += readFrameSize * ((int32_t)floorf(a_time / readSeconds) % readFrames);
                                        a_renderPassData->src.h = (float)readFrameSize;
                                        std::println("Post: ({}, {}, {}, {})", a_renderPassData->src.x, a_renderPassData->src.y, a_renderPassData->src.w, a_renderPassData->src.h);
                                    });
                                }
                                accumulator = "";
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::META_WIDTH: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ',':
                                this->width = std::stoi(accumulator);
                                std::println("Width: {}", width);
                                accumulator = "";
                                currentModeOne = ModeOne::META_HEIGHT;
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                    case ModeOne::META_HEIGHT: {
                        switch (c) {
                            case '\t':
                            case '\n':
                            case ' ':
                                break;
                            case ';':
                                this->height = std::stoi(accumulator);
                                std::println("Height: {}", height);
                                accumulator = "";
                                currentModeOne = ModeOne::STEP_START;
                                break;
                            default:
                                accumulator += c;
                                break;
                        }
                        break;
                    }
                }
            }
        }

        void NPCRendererAsset::Unload(void) {
        }
    }
}