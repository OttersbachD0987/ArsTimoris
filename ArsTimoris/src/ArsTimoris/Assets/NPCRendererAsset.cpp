#include <ArsTimoris/Assets/Assets.h>
#include <ArsTimoris/Game/TimeData.h>
#include <ArsTimoris/Util/Input.hpp>
#include <ArsTimoris/Visual/NPC/NPCImageStep.h>
#include <ArsTimoris/Visual/RenderPassData.h>

#include <iostream>
#include <fstream>
#include <print>

namespace ArsTimoris {
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
        
        void NPCRendererAsset::Render(SDL_Renderer* a_renderer, SDL_FRect a_rect, TimeData* a_timeData) {
            for (std::unique_ptr<Visual::NPC::NPCRenderStep>& renderStep : renderSteps) {
                renderStep->Render(a_renderer, a_rect, a_timeData);
            }
        }

        void NPCRendererAsset::Load(Assets* a_assets, std::string a_path) {
            std::vector<std::string> registers = std::vector<std::string>();
            std::string accumulator = "";
            std::string renderType = "";
            std::string modifierType = "";
            std::ifstream file = std::ifstream(a_path);
            std::unique_ptr<Visual::NPC::NPCRenderStep> renderStep = nullptr;
            ModeOne currentModeOne = ModeOne::STEP_START;
            char c = ' ';
            while ((c = file.get()) != -1) {
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
                                    } else if (registers.back() == "mod") {
                                        readBlendmode = SDL_BLENDMODE_MOD;
                                    } else if (registers.back() == "mul") {
                                        readBlendmode = SDL_BLENDMODE_MUL;
                                    } else if (registers.back() == "none") {
                                        readBlendmode = SDL_BLENDMODE_NONE;
                                    }
                                    registers.pop_back();
                                    uint8_t readAlpha = std::stoi(registers.back());
                                    registers.pop_back();
                                    std::string readTexture = registers.back();
                                    registers.pop_back();
                                    renderStep = std::make_unique<Visual::NPC::NPCImageStep>(a_assets->textures.at(readTexture), readAlpha, readBlendmode);
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
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](Visual::RenderPassData* a_renderPassData, TimeData* a_timeData) {
                                        a_renderPassData->rect.x += readAmplitude * sinf(a_timeData->totalTime_s * readFrequency * 3.14f);
                                    });
                                } else if (modifierType == "sway_sin_y") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](Visual::RenderPassData* a_renderPassData, TimeData* a_timeData) {
                                        a_renderPassData->rect.y += readAmplitude * sinf(a_timeData->totalTime_s * readFrequency * 3.14f);
                                    });
                                } else if (modifierType == "sway_cos_x") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](Visual::RenderPassData* a_renderPassData, TimeData* a_timeData) {
                                        a_renderPassData->rect.x += readAmplitude * cosf(a_timeData->totalTime_s * readFrequency * 3.14f);
                                    });
                                } else if (modifierType == "sway_cos_y") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](Visual::RenderPassData* a_renderPassData, TimeData* a_timeData) {
                                        a_renderPassData->rect.y += readAmplitude * cosf(a_timeData->totalTime_s * readFrequency * 3.14f);
                                    });
                                } else if (modifierType == "offset_x") {
                                    float readOffset = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readOffset](Visual::RenderPassData* a_renderPassData, TimeData* a_timeData) {
                                        a_renderPassData->rect.x += readOffset;
                                    });
                                } else if (modifierType == "offset_y") {
                                    float readOffset = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readOffset](Visual::RenderPassData* a_renderPassData, TimeData* a_timeData) {
                                        a_renderPassData->rect.y += readOffset;
                                    });
                                } else if (modifierType == "sway_sin_a") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](Visual::RenderPassData* a_renderPassData, TimeData* a_timeData) {
                                        a_renderPassData->alpha += (int8_t)roundf(readAmplitude * sinf(a_timeData->totalTime_s * readFrequency * 3.14f));
                                    });
                                } else if (modifierType == "sway_cos_a") {
                                    float readAmplitude = std::stof(registers.back());
                                    registers.pop_back();
                                    float readFrequency = std::stof(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readAmplitude, readFrequency](Visual::RenderPassData* a_renderPassData, TimeData* a_timeData) {
                                        a_renderPassData->alpha += (int8_t)roundf(readAmplitude * cosf(a_timeData->totalTime_s * readFrequency * 3.14f));
                                    });
                                } else if (modifierType == "animate_x") {
                                    float readSeconds = std::stof(registers.back());
                                    registers.pop_back();
                                    int32_t readFrames = std::stoi(registers.back());
                                    registers.pop_back();
                                    int32_t readFrameSize = std::stoi(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readSeconds, readFrames, readFrameSize](Visual::RenderPassData* a_renderPassData, TimeData* a_timeData) {
                                        a_renderPassData->src.x += readFrameSize * ((int32_t)floorf(a_timeData->totalTime_s / readSeconds) % readFrames);
                                        a_renderPassData->src.w = (float)readFrameSize;
                                    });
                                } else if (modifierType == "animate_y") {
                                    float readSeconds = std::stof(registers.back());
                                    registers.pop_back();
                                    int32_t readFrames = std::stoi(registers.back());
                                    registers.pop_back();
                                    int32_t readFrameSize = std::stoi(registers.back());
                                    registers.pop_back();
                                    renderStep->modifiers.emplace_back([readSeconds, readFrames, readFrameSize](Visual::RenderPassData* a_renderPassData, TimeData* a_timeData) {
                                        //std::println("Prae: ({}, {}, {}, {})", a_renderPassData->src.x, a_renderPassData->src.y, a_renderPassData->src.w, a_renderPassData->src.h);
                                        a_renderPassData->src.y += readFrameSize * ((int32_t)floorf(a_timeData->totalTime_s / readSeconds) % readFrames);
                                        a_renderPassData->src.h = (float)readFrameSize;
                                        //std::println("Post: ({}, {}, {}, {})", a_renderPassData->src.x, a_renderPassData->src.y, a_renderPassData->src.w, a_renderPassData->src.h);
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
                                //std::println("Width: {}", width);
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
                                //std::println("Height: {}", height);
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