// CSC 134
// M3LAB1
// Daley Ottersbach
// 3/6/2026

//#define __STDC_WANT_LIB_EXT1__ 1
//#include <cstring>
#include <ArsTimoris/Game/Map/RoomData.h>
#include <ArsTimoris/Game/GameState.h>
#include <ArsTimoris/DataComponents/DataContainer.h>
#include <ArsTimoris/Util/Input.hpp>
#include <ArsTimoris/Util/Random.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_audio.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <ArsTimoris/Assets/Assets.h>
#include <ArsTimoris/Assets/AudioAsset.h>
#include <ArsTimoris/Assets/FontAsset.h>
#include <ArsTimoris/Assets/TextureAsset.h>
#include <ArsTimoris/UI/UIManager.h>
#include <ArsTimoris/UI/UILazyTextComponent.h>
#include <ArsTimoris/UI/UIImageComponent.h>
#include <ArsTimoris/UI/UISliderComponent.h>
#include <ArsTimoris/UI/UIAtlasTextComponent.h>
#include <print>

void PrintCombatNPCData(GameState& a_gameState, const NPCData& a_npc) {
    std::cout << a_npc.name<< "\n";

    if (
        a_gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT))
    ) {
        std::cout << "- Armor [" << a_npc.armor << "]\n";
    }

    if (
        a_gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT)) ||
        a_gameState.player.GetSkillModifier("Brawler") >= 6
    ) {
        std::cout << "- HP [" << a_npc.curHP << "/" << a_npc.maxHP << "]\n";
    }

    if (
        a_gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT)) ||
        a_gameState.player.perks.test(static_cast<size_t>(Perks::ARCANE_EYES))
    ) {
        std::cout << "- Mana [" << a_npc.curMana << "/" << a_npc.maxMana << "]\n";
    }
}

int main(int argc, char** argv) {
    DebugLogging::OpenParserLog(std::filesystem::current_path() / "Logs" / "ParserLog.txt");
    std::filesystem::path dataPath = std::filesystem::current_path().append("Assets").append("Data");
    //DataComponents::DataContainer container = DataComponents::ParseDataFile(std::filesystem::current_path().append("test.data"));
    //std::cout << std::format("Pre loading Starts ({})", dataPath.string()) << std::endl;
    for (const StartData& start : LoadStartData(dataPath / "starts.data")) {
        GameState::Starts.push_back(start);
    }
    #pragma region Random Setup
    std::random_device random = std::random_device();
    GameState gameState = GameState(std::mt19937(random()), Interpreter());
    #pragma endregion

    //std::cout << std::format("Pre loading Encounters ({})", dataPath.string()) << std::endl;
    for (const Encounter& encounter : LoadEncountersData(dataPath / "encounters.data")) {
        GameData::ENCOUNTERS.push_back(encounter);
    }

    //std::cout << std::format("Pre loading Entity Templates ({})", dataPath.string()) << std::endl;
    for (const std::pair<std::string, EntityTemplate>& entityTemplate : LoadEntityData(dataPath / "entities.data")) {
        GameData::ENTITY_TEMPLATES.emplace(entityTemplate);
    }

    struct NPCDisplay {
    public:
        SDL_FRect area;
        std::shared_ptr<ArsTimoris::Assets::TextureAsset> texture;
        size_t index;

        NPCDisplay(SDL_FRect a_area, std::shared_ptr<ArsTimoris::Assets::TextureAsset> a_texture, size_t a_index) {
            this->area = a_area;
            this->texture = a_texture;
            this->index = a_index;
        }
    };
    std::vector<NPCDisplay> combatNPCs = std::vector<NPCDisplay>();
    
    //std::cout << "The answer is: " << (std::pow(29, 452)) << "." << std::endl;

    //for (const std::pair<size_t, std::uniform_int_distribution<uint32_t>&>& dice : DICE) {
    //    std::cout << "DIE " << dice.first << "\n====================" << std::endl;
    //    for (int32_t i = 0; i < 50; ++i) {
    //        std::cout << (i + 1) << ") " << dice.second(gameState.generator) << "/" << dice.first << std::endl;
    //    }
    //    std::cout << "====================" << std::endl;
    //}

    SDL_SetAppMetadata("Ars Timoris", "0.1.0.0", "com.darthsae.ArsTimoris");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, "Darthsae");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/OttersbachD0987/ArsTimoris");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

    if (!TTF_Init()) {
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

    if (!SDL_CreateWindowAndRenderer("Ars Timoris", 1200, 800, SDL_WINDOW_OPENGL, &gameState.window, &gameState.renderer)) {
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

    gameState.audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    
    std::cout << "Pre Assets" << std::endl;
    gameState.assets = ArsTimoris::Assets::Assets();
    std::cout << "Pre UI Manager" << std::endl;
    gameState.uiManager = ArsTimoris::UI::UIManager();
    gameState.assets.uiManager = &gameState.uiManager;

    std::cout << "Pre Images" << std::endl;
    for (std::filesystem::directory_entry const& dir_entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path().append("Assets"))) {
        if (dir_entry.path().has_extension()) {
            if (dir_entry.path().extension().string() == ".png") {
                gameState.assets.AddTexture(gameState.renderer, dir_entry.path().string(), dir_entry.path().stem().string());
            } else if (dir_entry.path().extension().string() == ".fpng") {
                gameState.assets.AddFontAtlas(gameState.renderer, dir_entry.path().string(), dir_entry.path().stem().string());
            } else if (dir_entry.path().extension().string() == ".ttf") {
                gameState.assets.AddLazyFont(dir_entry.path().string(), dir_entry.path().stem().string());

                // Deprecated
                /*
                gameState.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 12), 12);
                gameState.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 16), 16);
                gameState.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 18), 18);
                gameState.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 20), 20);
                gameState.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 24), 24);
                gameState.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 30), 30);
                gameState.assets.AddFont(dir_entry.path().string(), std::format("{}-{}", dir_entry.path().stem().string(), 36), 36);
                */
            } else if (dir_entry.path().extension().string() == ".wav") {
                gameState.assets.AddSound(gameState.audioDevice, dir_entry.path().string(), dir_entry.path().stem().string());
            }
        }
    }
    #pragma endregion

    #pragma region Setup Vars
    RoomInstance* room = nullptr;
    int32_t choice;
    #pragma endregion

    #pragma region UI
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Main Menu"), 
        std::forward_as_tuple(std::string_view("Main Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Settings Menu"), 
        std::forward_as_tuple(std::string_view("Settings Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Starts Menu"), 
        std::forward_as_tuple(std::string_view("Starts Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Room General Menu"), 
        std::forward_as_tuple(std::string_view("Room General Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Room Move Menu"), 
        std::forward_as_tuple(std::string_view("Room Move Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Room Actions Menu"), 
        std::forward_as_tuple(std::string_view("Room Actions Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Combat Menu"), 
        std::forward_as_tuple(std::string_view("Combat Menu"))
    );

    ArsTimoris::UI::UIElement* element;
    ArsTimoris::UI::UIElement* otherElement;

    ArsTimoris::UI::UILayer* mainMenu = &gameState.uiManager.uiLayers.at("Main Menu");
    mainMenu->enabled = true;
    ArsTimoris::UI::UILayer* settingsMenu = &gameState.uiManager.uiLayers.at("Settings Menu");
    settingsMenu->enabled = false;
    ArsTimoris::UI::UILayer* startsMenu = &gameState.uiManager.uiLayers.at("Starts Menu");
    startsMenu->enabled = false;
    ArsTimoris::UI::UILayer* roomGeneralMenu = &gameState.uiManager.uiLayers.at("Room General Menu");
    roomGeneralMenu->enabled = false;
    ArsTimoris::UI::UILayer* roomMoveMenu = &gameState.uiManager.uiLayers.at("Room Move Menu");
    roomMoveMenu->enabled = false;
    ArsTimoris::UI::UILayer* roomActionsMenu = &gameState.uiManager.uiLayers.at("Room Actions Menu");
    roomActionsMenu->enabled = false;
    ArsTimoris::UI::UILayer* combatMenu = &gameState.uiManager.uiLayers.at("Combat Menu");
    combatMenu->enabled = false;

    #pragma region Main Menu
    mainMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Play"), 
        std::forward_as_tuple(
            std::string_view("Play"), 
            ArsTimoris::UI::UIRect{{500, 400, 200, 60}}
        )
    );
    element = &mainMenu->uiElements.at("Play");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, mainMenu, element);
    element->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        a_uiLayer->enabled = false;
        startsMenu->enabled = true; 
        a_gameState.screen = Screen::GAME_SELECT;
        a_gameState.menu = Menu::NONE;
        return true;
    });
    //element->components.emplace(std::piecewise_construct, 
    //    std::forward_as_tuple("Text"), 
    //    std::forward_as_tuple(
    //        std::make_shared<ArsTimoris::UI::UILazyTextComponent>(
    //            "Play",
    //            "NotoSansMono-Regular",
    //            36.0f,
    //            6,
    //            ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
    //        )
    //    )
    //).first->second->Hookup(gameState, mainMenu, element);

    /*
    \[FCB:11111111,11111111,11111111,11111111]
    \[FCO:377,377,377,377]
    \[FCD:255,255,255,255]
    \[FCH:FF,FF,FF,FF]
    \b
    \n
    */
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Play",
                "BitCrusher",
                4.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, mainMenu, element);

    mainMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Quit"), 
        std::forward_as_tuple(
            std::string_view("Quit"), 
            ArsTimoris::UI::UIRect{{500, 500, 200, 60}}
        )
    );
    element = &mainMenu->uiElements.at("Quit");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, mainMenu, element);
    element->onMouseLeftDown.emplace_back([](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        a_gameState.running = false;
        return true;
    });
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Quit",
                "BitCrusher",
                4.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, mainMenu, element);
    #pragma endregion

    #pragma region Starts
    startsMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Main Panel"), 
        std::forward_as_tuple(
            std::string_view("Main Panel"), 
            ArsTimoris::UI::UIRect{{0, 0, 1000, 400}}
        )
    );
    element = &startsMenu->uiElements.at("Main Panel");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, startsMenu, element);
    
    startsMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Scroll Panel"), 
        std::forward_as_tuple(
            std::string_view("Scroll Panel"), 
            ArsTimoris::UI::UIRect{{0, 400, 1000, 40}}
        )
    );
    element = &startsMenu->uiElements.at("Scroll Panel");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, startsMenu, element);
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Slider"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UISliderComponent>(
                0.0f,
                (float)(200 * GameState::Starts.size()),
                992.0f
            )
        )
    ).first->second->Hookup(gameState, startsMenu, element);
    
    //otherElement = element;
    std::shared_ptr<ArsTimoris::UI::UISliderComponent> slider = std::dynamic_pointer_cast<ArsTimoris::UI::UISliderComponent>(element->components.at("Slider"));

    element = startsMenu->uiElements.at("Main Panel").AddChild(std::string_view("ScrollClip"), ArsTimoris::UI::UIRect{{4, 4, 992, 392}})->AddChild(std::string_view("Scroll"), ArsTimoris::UI::UIRect{{0, 0, (float)(200 * GameState::Starts.size()), 392}}).get();
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, startsMenu, element);
    element->onRecalculate.emplace_back([slider](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element) {
        a_element->internalArea.SetX(-(slider->value));
        //a_element->Recalculate(a_gameState, a_uiLayer);
    });

    std::function<void(const StartData&)> MakeStart = [&](const StartData& a_startData){
        static int32_t posX = 0;
        std::shared_ptr<ArsTimoris::UI::UIElement> pot = element->AddChild(
            std::string_view(a_startData.name), 
            ArsTimoris::UI::UIRect{{(float)posX, 0, 200, 392}}
        );
        pot->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("UIPanel"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, startsMenu, pot.get());
        std::shared_ptr<ArsTimoris::UI::UIElement> motley = pot->AddChild(
            std::string_view("Name"), 
            ArsTimoris::UI::UIRect{{0, 0, 200, 92}}
        );
        motley->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("UIPanel"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, startsMenu, motley.get());
        motley->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Text"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                    a_startData.name,
                    "BitCrusher",
                    3.0f,
                    6,
                    ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
                )
            )
        ).first->second->Hookup(gameState, startsMenu, motley.get());
        motley = pot->AddChild(
            std::string_view("Description"), 
            ArsTimoris::UI::UIRect{{0, 92, 200, 240}}
        );
        motley->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("UIPanel"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, startsMenu, motley.get());
        motley->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Text"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                    a_startData.description,
                    "BitCrusher",
                    3.0f,
                    8,
                    ArsTimoris::UI::UIAnchor::TOP_CENTER
                )
            )
        ).first->second->Hookup(gameState, startsMenu, motley.get());
        motley = pot->AddChild(
            std::string_view("Play"), 
            ArsTimoris::UI::UIRect{{0, 332, 200, 60}}
        );
        motley->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("UIPanel"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, startsMenu, motley.get());
        motley->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
            a_uiLayer->enabled = false;
            roomGeneralMenu->enabled = true;
            a_gameState.screen = Screen::GAME;
            a_gameState.menu = Menu::NONE;
            std::println("Selected Start: {}", a_startData.name);
            a_gameState.Start(a_startData.name);
            return true;
        });
        motley->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Text"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                    "Play",
                    "BitCrusher",
                    4.0f,
                    6,
                    ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
                )
            )
        ).first->second->Hookup(gameState, startsMenu, motley.get());
        posX += 200;
    };

    for (size_t i = 0; i < GameState::Starts.size(); ++i) {
        //std::cout << (i + 1) << ") " << GameState::Starts[i].name << "\n- " << GameState::Starts[i].description << "\n" << std::endl;

        MakeStart(GameState::Starts[i]);
    }
    #pragma endregion
    /*
    mainMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Panel"), 
        std::forward_as_tuple(
            std::string_view("Panel"), 
            ArsTimoris::UI::UIRect{{0, 0, 100, 240}}
        )
    );
    element = &mainMenu->uiElements.at("Panel");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, mainMenu, element);

    element = &*mainMenu->uiElements.at("Panel").AddChild(
        std::string_view("Compo1"), 
        ArsTimoris::UI::UIRect{{0, 0, 120, 100}}
    );
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, mainMenu, element);

    element = &*mainMenu->uiElements.at("Panel").AddChild(
        std::string_view("Compo2"), 
        ArsTimoris::UI::UIRect{{0, 110, 100, 100}}
    );
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, mainMenu, element);

    element = &*mainMenu->uiElements.at("Panel").AddChild(
        std::string_view("Compo3"), 
        ArsTimoris::UI::UIRect{{0, 220, 180, 260}}
    );
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, mainMenu, element);
    */
    
    #pragma region Room General
    roomGeneralMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Move"), 
        std::forward_as_tuple(
            std::string_view("Move"), 
            ArsTimoris::UI::UIRect{{0, 740, 240, 60}}
        )
    );
    element = &roomGeneralMenu->uiElements.at("Move");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);
    element->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        a_uiLayer->enabled = false;
        roomMoveMenu->enabled = true; 
        gameState.menu = Menu::MOVING;
        choice = 0;
        return true;
    });
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Move",
                "BitCrusher",
                4.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);
    
    roomGeneralMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Actions"), 
        std::forward_as_tuple(
            std::string_view("Actions"), 
            ArsTimoris::UI::UIRect{{240, 740, 240, 60}}
        )
    );
    element = &roomGeneralMenu->uiElements.at("Actions");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);
    element->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        a_uiLayer->enabled = false;
        startsMenu->enabled = true; 
        gameState.menu = Menu::ROOM_ACTIONS;
        return true;
    });
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Actions",
                "BitCrusher",
                4.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);

    roomGeneralMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Stats"), 
        std::forward_as_tuple(
            std::string_view("Stats"), 
            ArsTimoris::UI::UIRect{{480, 740, 240, 60}}
        )
    );
    element = &roomGeneralMenu->uiElements.at("Stats");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);
    element->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        a_uiLayer->enabled = false;
        startsMenu->enabled = true; 
        gameState.menu = Menu::STATS;
        return true;
    });
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Stats",
                "BitCrusher",
                4.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);

    roomGeneralMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Inventory"), 
        std::forward_as_tuple(
            std::string_view("Inventory"), 
            ArsTimoris::UI::UIRect{{720, 740, 240, 60}}
        )
    );
    element = &roomGeneralMenu->uiElements.at("Inventory");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);
    element->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        a_uiLayer->enabled = false;
        startsMenu->enabled = true;
        gameState.menu = Menu::INVENTORY;
        return true;
    });
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Inventory",
                "BitCrusher",
                4.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);

    roomGeneralMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Quit"), 
        std::forward_as_tuple(
            std::string_view("Quit"), 
            ArsTimoris::UI::UIRect{{960, 740, 240, 60}}
        )
    );
    element = &roomGeneralMenu->uiElements.at("Quit");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);
    element->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        a_uiLayer->enabled = false;
        mainMenu->enabled = true; 
        gameState.screen = Screen::TITLE;
        gameState.menu = Menu::NONE;
        return true;
    });
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Quit",
                "BitCrusher",
                4.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);

    roomGeneralMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("TopBar"), 
        std::forward_as_tuple(
            std::string_view("TopBar"), 
            ArsTimoris::UI::UIRect{{0, 0, 1200, 140}}
        )
    );
    element = &roomGeneralMenu->uiElements.at("TopBar");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, element);

    otherElement = element->AddChild("HP", ArsTimoris::UI::UIRect{{5, 5, 140, 40}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "HP: 100/100",
                "BitCrusher",
                2.0f,
                8,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_LEFT
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, otherElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> hpText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));
    
    otherElement = element->AddChild("MP", ArsTimoris::UI::UIRect{{5, 50, 140, 40}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "MP: 100/100",
                "BitCrusher",
                2.0f,
                8,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_LEFT
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, otherElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> mpText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));
    
    otherElement = element->AddChild("AC", ArsTimoris::UI::UIRect{{150, 5, 100, 40}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "AC: 10",
                "BitCrusher",
                2.0f,
                8,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_LEFT
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, otherElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> acText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));
    
    otherElement = element->AddChild("XP", ArsTimoris::UI::UIRect{{150, 50, 100, 40}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "XP: 10",
                "BitCrusher",
                2.0f,
                8,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_LEFT
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, otherElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> xpText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));
    
    otherElement = element->AddChild("GP", ArsTimoris::UI::UIRect{{5, 95, 140, 40}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "GP: 1000",
                "BitCrusher",
                2.0f,
                8,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_LEFT
            )
        )
    ).first->second->Hookup(gameState, roomGeneralMenu, otherElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> gpText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));
    #pragma endregion

    #pragma region Room Move
    roomMoveMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Enter"), 
        std::forward_as_tuple(
            std::string_view("Enter"), 
            ArsTimoris::UI::UIRect{{0, 740, 240, 60}}
        )
    );
    element = &roomMoveMenu->uiElements.at("Enter");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomMoveMenu, element);
    element->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        a_uiLayer->enabled = false;
        roomGeneralMenu->enabled = true; 
        gameState.menu = Menu::NONE;
        if (room->connections[choice].CanPass(gameState)) {
            room->connections[choice].Passes(gameState);
            gameState.curRoom = room->connections[choice].destination;
        }
        return true;
    });
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Enter",
                "BitCrusher",
                4.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, roomMoveMenu, element);

    roomMoveMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Label"), 
        std::forward_as_tuple(
            std::string_view("Label"), 
            ArsTimoris::UI::UIRect{{0, 300, 240, 60}}
        )
    );
    element = &roomMoveMenu->uiElements.at("Label");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomMoveMenu, element);
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "2/2",
                "BitCrusher",
                2.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, roomMoveMenu, element);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> labelText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(element->components.at("Text"));
    #pragma endregion

    #pragma region Combat
    combatMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Top Action"), 
        std::forward_as_tuple(
            std::string_view("Top Action"), 
            ArsTimoris::UI::UIRect{{720, 410, 260, 50}}
        )
    );
    element = &combatMenu->uiElements.at("Top Action");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, combatMenu, element);
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Scallion",
                "BitCrusher",
                2.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, combatMenu, element);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> topActionText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(element->components.at("Text"));
    
    combatMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Middle Action"), 
        std::forward_as_tuple(
            std::string_view("Middle Action"), 
            ArsTimoris::UI::UIRect{{700, 470, 300, 60}}
        )
    );
    element = &combatMenu->uiElements.at("Middle Action");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, combatMenu, element);
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Scallion",
                "BitCrusher",
                3.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, combatMenu, element);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> middleActionText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(element->components.at("Text"));
    
    combatMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Bottom Action"), 
        std::forward_as_tuple(
            std::string_view("Bottom Action"), 
            ArsTimoris::UI::UIRect{{720, 540, 260, 50}}
        )
    );
    element = &combatMenu->uiElements.at("Bottom Action");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, combatMenu, element);
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Scallion",
                "BitCrusher",
                2.0f,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, combatMenu, element);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> bottomActionText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(element->components.at("Text"));
    #pragma endregion
    #pragma endregion

    if (gameState.uiManager.dirtyRecalculate) {
        gameState.uiManager.Recalculate(gameState);
    }

    size_t encounter;
    bool runEncounter = false;
    SDL_Event event;
    uint64_t deltaTime;
    uint64_t lastTime = SDL_GetTicks();
    gameState.inputData.keys = SDL_GetKeyboardState(&gameState.inputData.numKeys);
    gameState.inputData.oldKeys = (bool*)malloc(gameState.inputData.numKeys);
    while (gameState.running) {
        //std::cout << "Very Start" << std::endl;
        deltaTime = SDL_GetTicks() - lastTime;
        lastTime = SDL_GetTicks();
        //std::cout << "Prae Cache Input" << std::endl;
        gameState.CacheInputState();
        //std::cout << "Post Cache Input" << std::endl;
        while (SDL_PollEvent(&event)) {
            gameState.inputData.mouse = SDL_GetMouseState(&gameState.inputData.mousePos.x, &gameState.inputData.mousePos.y);
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    gameState.running = false;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    switch (event.button.button) {
                        case SDL_BUTTON_LEFT:
                            if (!gameState.uiManager.OnMouseLeftDown(gameState, &gameState.inputData.mousePos)) {
                                switch (gameState.menu) {
                                    case Menu::COMBAT: {
                                        for (const NPCDisplay& npc : combatNPCs) {
                                            if (SDL_PointInRectFloat(&gameState.inputData.mousePos, &npc.area)) {

                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                            break;
                        case SDL_BUTTON_MIDDLE:
                            if (!gameState.uiManager.OnMouseMiddleDown(gameState, &gameState.inputData.mousePos)) {
                            
                            }
                            break;
                        case SDL_BUTTON_RIGHT:
                            if (!gameState.uiManager.OnMouseRightDown(gameState, &gameState.inputData.mousePos)) {
                                
                            }
                            break;
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    switch (event.button.button) {
                        case SDL_BUTTON_LEFT:
                            if (!gameState.uiManager.OnMouseLeftUp(gameState, &gameState.inputData.mousePos)) {

                            }
                            break;
                        case SDL_BUTTON_MIDDLE:
                            if (!gameState.uiManager.OnMouseMiddleUp(gameState, &gameState.inputData.mousePos)) {

                            }
                            break;
                        case SDL_BUTTON_RIGHT:
                            if (!gameState.uiManager.OnMouseRightUp(gameState, &gameState.inputData.mousePos)) {

                            }
                            break;
                    }
                    break;
                case SDL_EVENT_KEY_DOWN:
                    switch (gameState.screen) {
                        case Screen::GAME: {
                            switch (gameState.menu) {
                                case Menu::MOVING: {
                                    if (event.key.key == SDLK_A) {
                                        if (--choice < 0) {
                                            choice = gameState.rooms[gameState.curRoom].connections.size() - 1;
                                        }
                                    } else if (event.key.key == SDLK_D) {
                                        if (++choice >= gameState.rooms[gameState.curRoom].connections.size()) {
                                            choice = 0;
                                        }
                                    }
                                    break;
                                }
                                case Menu::COMBAT: {
                                    if (event.key.key == SDLK_UP) {
                                        if (--choice < 0) {
                                            choice = gameState.player.actions.size() - 1;
                                        }
                                    } else if (event.key.key == SDLK_DOWN) {
                                        if (++choice >= gameState.player.actions.size()) {
                                            choice = 0;
                                        }
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    break;
            }
        }

        //std::cout << "Prae Process" << std::endl;
        gameState.uiManager.Process(gameState);
        //std::cout << "Post Process" << std::endl;

        switch (gameState.screen) {
            case Screen::TITLE: {
                break;
            }
            case Screen::GAME_SELECT: {
                /*
                GameState::DisplayStarts();
                
                std::cout << "\nOption: ";

                SafeInput<int32_t>(choice);

                if (--choice < GameState::Starts.size()) {
                    gameState.screen = Screen::GAME;
                    gameState.Start(choice);
                }
                */
                break;
            }
            case Screen::GAME: {
                switch (gameState.menu) {
                    case Menu::NONE: {
                        if (!gameState.rooms[gameState.curRoom].initialized) {
                            gameState.InitializeRoom(&gameState.rooms[gameState.curRoom]);
                            encounter = gameState.GetRandomEncounter();
                            if (GameData::ENCOUNTERS[encounter].event != "{}") {
                                runEncounter = true;
                            }
                        }
                    
                        room = &gameState.rooms[gameState.curRoom];
                        
                        if (runEncounter) {
                            gameState.interpreter.ParseStatement<int32_t>(gameState, RegisterType::ERROR, GameData::ENCOUNTERS[encounter].event);
                            runEncounter = false;
                        }

                        room = &gameState.rooms[gameState.curRoom];
                        if (room->inhabitants.size() > 0 && !gameState.debug.noFights) {
                            roomGeneralMenu->enabled = false;
                            combatMenu->enabled = true;
                            choice = 0;
                            gameState.menu = Menu::COMBAT;
                            std::cout << "Moogle" << std::endl;
                            for (size_t i = 0; i < room->inhabitants.size(); ++i) {
                                std::println("({}, {}, {}, {}) {}", 20.0f + 40.0f * i, 30.0f, 40.0f, 60.0f, i);
                                combatNPCs.push_back(NPCDisplay(
                                    SDL_FRect{20.0f + 40.0f * i, 30.0f, 40.0f, 60.0f}, 
                                    gameState.assets.textures.at("UIPanel"), i
                                ));
                            }
                            break;
                        }

                        if (room->roomActions.size() > 0) {
                            for (const RoomAction& roomAction : room->roomActions) {
                                if (roomAction.roomDescription != nullptr) {
                                    roomAction.roomDescription(gameState);
                                }
                            }
                        }
                        
                        /*
                        std::cout << "\nActions:\n1) Move\n2) Actions\n3) Stats\n4) Inventory\n5) Quit\n\nOption: ";
                        SafeInput<int32_t>(choice);
                        switch (choice) {
                            case 1:
                                gameState.menu = Menu::MOVING;
                                break;
                            case 2:
                                gameState.menu = Menu::ROOM_ACTIONS;
                                break;
                            case 3:
                                gameState.menu = Menu::STATS;
                                break;
                            case 4:
                                gameState.menu = Menu::INVENTORY;
                                break;
                            case 5:
                                gameState.screen = Screen::TITLE;
                                gameState.menu = Menu::NONE;
                                break;
                            case 333: {
                                if (gameState.debug.enabled) {
                                    int32_t number = 0;
                                    std::cout << "\x1b[1mDebug Menu (EDIT)\x1b[22m\n1) Change Gold\n2) Change XP\n3) Back\n\nOption: ";
                                    SafeInput<int32_t>(choice);
                                    switch (choice) {
                                        case 1:
                                            std::cout << "Gold to add: ";
                                            SafeInput<int32_t>(number);
                                            gameState.player.gold += number;
                                            break;
                                        case 2:
                                            std::cout << "XP to add: ";
                                            SafeInput<int32_t>(number);
                                            gameState.player.xp += number;
                                            break;
                                        case 3:
                                            break;
                                    }
                                }
                                break;
                            }
                            case 444: {
                                if (gameState.debug.enabled) {
                                    int32_t number = 0;
                                    std::cout << "\x1b[1mDebug Menu (INFO)\x1b[22m\n1) View Classes\n2) View Encounters\n3) View Entity Templates\n4) Back\n\nOption: ";
                                    SafeInput<int32_t>(choice);
                                    switch (choice) {
                                        case 1:
                                            for (std::pair<std::string, ClassData> classPair : GameData::CLASSES) {
                                                const ClassData& classData = classPair.second;
                                                std::cout << classData.name << "\n- " << classData.description << "\nLEVELS\n";
                                                size_t levelIndex = 1;
                                                for (const ClassLevel& classLevel : classData.levels) {
                                                    std::cout << levelIndex++ << ". ";
                                                    classLevel.displayRequirements(gameState, gameState.player);
                                                    std::cout << "\n";
                                                }
                                                std::cout << std::endl;
                                            }
                                            break;
                                        case 2:
                                            for (const Encounter& encounter : GameData::ENCOUNTERS) {
                                                std::cout << std::format("Weight:\n{}\n\nEvent:\n{}", encounter.weight, encounter.event) << std::endl;
                                            }
                                            break;
                                        case 3:
                                            for (const std::pair<std::string, EntityTemplate>& entityTemplate : GameData::ENTITY_TEMPLATES) {
                                                std::cout << std::format("Name: {}\nHP: {}\nMana: {}\nMana Regeneration: {}\nArmor: {}\n\nSkills:\n", entityTemplate.first, entityTemplate.second.hp, entityTemplate.second.mana, entityTemplate.second.manaRegen, entityTemplate.second.armor);
                                                for (const std::pair<std::string, int32_t>& skill : entityTemplate.second.skills) {
                                                    std::cout << std::format("{}: {}\n", skill.first, skill.second);
                                                }
                                                std::cout << "\nActions:\n";
                                                for (const Action& action : entityTemplate.second.actions) {
                                                    std::cout << std::format("{}\n", action.name);
                                                }
                                                std::cout << std::endl;
                                            }
                                            break;
                                        case 4:
                                            break;
                                    }
                                }
                                break;
                            }
                            case 555: {
                                if (gameState.debug.enabled) {
                                    int32_t number = 0;
                                    std::cout << std::format("\x1b[1mDebug Menu (FLAG)\x1b[22m\n1) {}No Fights\x1b[39m\n2) ERR\n3) Back\n\nOption: ", (gameState.debug.noFights ? "\x1b[32m" : "\x1b[31m"));
                                    SafeInput<int32_t>(choice);
                                    switch (choice) {
                                        case 1:
                                            gameState.debug.noFights = !gameState.debug.noFights;
                                            break;
                                        case 2:
                                            break;
                                        case 3:
                                            break;
                                    }
                                }
                                break;
                            }
                            case 666: {
                                gameState.debug.enabled = !gameState.debug.enabled;
                                break;
                            }
                        }
                        */
                        break;
                    }
                    case Menu::MOVING: {
                        room = &gameState.rooms[gameState.curRoom];
                        labelText->SetText(gameState, &roomMoveMenu->uiElements.at("Label"), std::format("{}/{}) [{}] \\[FCH:{}]{}\b\b\b\\[FCH:AA,0B,C3,CF]boo", choice + 1, room->connections.size(), room->connections[choice].destination, (gameState.rooms[room->connections[choice].destination].initialized ? "FF,00,00" : "00,FF,00"), GameData::ROOM_DATA[gameState.rooms[room->connections[choice].destination].roomID].roomName));
                        /*
                        std::cout << "-------------------------------\nRooms:\n";
                        for (size_t connectionIndex = 0; connectionIndex < room->connections.size();) {
                            std::cout << ++connectionIndex << ") [" << room->connections[connectionIndex - 1].destination  << "] \x1b[38;5;" << (gameState.rooms[room->connections[connectionIndex - 1].destination].initialized ? "8m" : "15m") << GameData::ROOM_DATA[gameState.rooms[room->connections[connectionIndex - 1].destination].roomID].roomName << "\x1b[39m\n";
                        }
                        std::cout << (room->connections.size() + 1) << ") Back\n\nOption: ";

                        SafeInput<int32_t>(choice);

                        if (--choice < room->connections.size()) {
                            if (room->connections[choice].CanPass(gameState)) {
                                room->connections[choice].Passes(gameState);
                                gameState.curRoom = room->connections[choice].destination;
                            }
                            gameState.menu = Menu::NONE;
                        } else if (choice == room->connections.size()) {
                            gameState.menu = Menu::NONE;
                        }
                        */
                        break;
                    }
                    case Menu::ROOM_ACTIONS: {
                        room = &gameState.rooms[gameState.curRoom];
                        std::cout << "-------------------------------\nActions:\n";
                        for (size_t i = 0; i < room->roomActions.size(); ++i) {
                            std::cout << (i + 1) << ") " << room->roomActions[i].name << "\n- " << room->roomActions[i].description << "\n";
                        }
                        std::cout << (room->roomActions.size() + 1) << ") Back\n\nOption: ";

                        SafeInput<int32_t>(choice);

                        if (--choice < room->roomActions.size()) {
                            if (room->roomActions[choice].condition(gameState)) {
                                room->roomActions[choice].usage(gameState);
                            }
                            gameState.menu = Menu::NONE;
                        } else if (choice == room->roomActions.size()) {
                            gameState.menu = Menu::NONE;
                        }
                        break;
                    }
                    case Menu::STATS: {
                        std::cout 
                            << "-------------------------------\n\x1b[1mStats:\x1b[22m\nHP: " 
                            << gameState.player.curHP << "/" << gameState.player.maxHP 
                            << "\nMana: " 
                            << gameState.player.curMana << "/" << gameState.player.maxMana 
                            << "\nArmor: "
                            << gameState.player.GetEffectiveArmor()
                            << " ("
                            << gameState.player.armor
                            << " + "
                            << (gameState.player.GetEffectiveArmor() - gameState.player.armor)
                            << ")\nXP: "
                            << gameState.player.xp
                            << "\nGold: "
                            << gameState.player.gold
                            << "\n\n\x1b[1mClasses:\x1b[22m\n";

                        for (const std::pair<std::string, ClassInstance>& classPair : gameState.player.classes) {
                            std::cout << classPair.first << "(" << classPair.second.level << "/" << GameData::CLASSES.at(classPair.first).levels.size() << "): " << GameData::CLASSES.at(classPair.first).description << "\n";
                        }
                            
                        std::cout << "\n\x1b[1mSkills:\x1b[22m\n";
                        
                        for (std::pair<std::string, int32_t> skillPair : gameState.player.skills) {
                            std::cout << "- " << skillPair.first << ": " << skillPair.second << "\n";
                        }
                            
                        std::cout << "\nOptions:\n1) Level Up\n2) Back\n\nOption: ";

                        SafeInput<int32_t>(choice);

                        switch (choice) {
                            case 1:
                                gameState.menu = Menu::LEVEL_UP;
                                break;
                            case 2:
                                gameState.menu = Menu::NONE;
                                break;
                        }
                        break;
                    }
                    case Menu::LEVEL_UP: {
                        std::cout 
                            << "-------------------------------\nXP: " 
                            << gameState.player.xp
                            << "\nClasses:\n";
                        
                        std::vector<std::string> classics = std::vector<std::string>();
                        size_t accum = 1;
                        for (const std::pair<std::string, ClassData>& classPair : GameData::CLASSES) {
                            classics.push_back(classPair.first);
                            std::cout << (accum++) << ") " << classPair.first << "(" << (gameState.player.classes.contains(classPair.first) ? (gameState.player.classes.at(classPair.first).level) : 0) << "/" << classPair.second.levels.size() << "): " << classPair.second.description << "\n";
                            size_t levelToUse = (gameState.player.classes.contains(classPair.second.name) ? (gameState.player.classes.at(classPair.second.name).level + 1) : 1);
                            //std::cout << "Level: " << levelToUse << " : " << classPair.second.levels.size() << std::endl;
                            if (classPair.second.levels.size() >= levelToUse && classPair.second.levels[--levelToUse].displayRequirements != nullptr) {
                                std::cout << "- \x1b[" << ((classPair.second.levels[levelToUse].applicable == nullptr || classPair.second.levels[levelToUse].applicable(gameState, gameState.player)) ? "32m" : "31m");
                                classPair.second.levels[levelToUse].displayRequirements(gameState, gameState.player);
                                std::cout << "\x1b[39m\n";
                            }
                        }
                            
                        std::cout << accum << ") Back\n\nOption: ";

                        SafeInput<int32_t>(choice);

                        if (--choice < GameData::CLASSES.size()) {
                            const ClassData& classRef = GameData::CLASSES.at(classics[choice]);
                            size_t levelToUseRedone = (gameState.player.classes.contains(classRef.name) ? (gameState.player.classes.at(classRef.name).level + 1) : 1);
                            if (classRef.levels.size() < levelToUseRedone) {
                                std::cout << "Already max level." << std::endl;
                            } else  {
                                if (classRef.levels[levelToUseRedone - 1].applicable == nullptr || classRef.levels[levelToUseRedone - 1].applicable(gameState, gameState.player)) {
                                    for (size_t levelUpEffect = 0; levelUpEffect < classRef.levels[levelToUseRedone - 1].levelUpEffects.size(); ++levelUpEffect) {
                                        classRef.levels[levelToUseRedone - 1].levelUpEffects[levelUpEffect](gameState, gameState.player, false);
                                    }

                                    if (gameState.player.classes.contains(classRef.name)) {
                                        ++gameState.player.classes.at(classRef.name).level;
                                    } else {
                                        gameState.player.classes.insert(std::pair<std::string, ClassInstance>(classRef.name, ClassInstance(classRef.name, 1)));
                                    }
                                }
                            }
                        } else if (choice == GameData::CLASSES.size()) {
                            gameState.menu = Menu::STATS;
                        }
                        break;
                    }
                    case Menu::INVENTORY: {
                        std::cout << "-------------------------------\nInventory:\n";
                        for (size_t inventoryItem = 0; inventoryItem < gameState.player.items.size();) {
                            std::cout << ++inventoryItem << ") [" << (gameState.player.equipped[inventoryItem - 1] ? 'X' : ' ') << "] " << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].name << " x" << gameState.player.items[inventoryItem - 1].stackSize << "/" << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].maxStack << "\n -" << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].description << "\n";
                        }
                        std::cout << (gameState.player.items.size() + 1) << ") Back\n\nOption: ";

                        SafeInput<int32_t>(choice);

                        if (--choice < gameState.player.items.size()) {
                            size_t itemIndex = choice;
                            ItemStack& itemStack = gameState.player.items[itemIndex];
                            const ItemData& itemType = GameData::ITEM_DATA[itemStack.itemID];
                            std::cout << "-------------------------------\n" << itemType.name << " x" << itemStack.stackSize << "/" << itemType.maxStack << "\n- " << itemType.description << "\nEquipped [" << (gameState.player.equipped[itemIndex] ? 'X' : ' ') << "]\n";
                            for (const std::pair<std::string, int32_t>& modPair : itemStack.metadata) {
                                std::cout << "- " << modPair.first << ": " << modPair.second << "\n";
                            }
                            std::cout << "\n1) Use\n2) " << (gameState.player.equipped[itemIndex] ? "Unequip" : "Equip") << "\n3) Back\n\nOption: ";
                            SafeInput<int32_t>(choice);
                            switch (choice) {
                                case 1:
                                    if (itemType.usage.usage != nullptr) {
                                        if (itemType.usage.condition == nullptr || itemType.usage.condition(gameState, itemStack, itemIndex)) {
                                            itemType.usage.usage(gameState, itemStack, itemIndex);
                                        } else {
                                            std::cout << "You don't know why you would use this right now." << std::endl;
                                        }
                                    } else {
                                        std::cout << "This item is not usable." << std::endl;
                                    }
                                    break;
                                case 2:
                                    gameState.player.equipped[itemIndex] = !gameState.player.equipped[itemIndex];
                                    break;
                            }
                        } else if (choice == gameState.player.items.size()) {
                            gameState.menu = Menu::NONE;
                        }
                        break;
                    }
                    case Menu::COMBAT: {
                        room = &gameState.rooms[gameState.curRoom];
                        topActionText->SetText(gameState, &combatMenu->uiElements.at("Top Action"), std::format("{}", gameState.player.actions[(choice - 1 < 0) ? (gameState.player.actions.size() - 1) : (choice - 1)].name));
                        middleActionText->SetText(gameState, &combatMenu->uiElements.at("Middle Action"), std::format("{}", gameState.player.actions[choice].name));
                        bottomActionText->SetText(gameState, &combatMenu->uiElements.at("Bottom Action"), std::format("{}", gameState.player.actions[(choice + 1 >= gameState.player.actions.size()) ? 0 : (choice + 1)].name));

                        /*
                        if (room->inhabitants.size() > 0) {
                            bool enemiesGo = true;
                            std::cout << "-------------------------------\nOpponents:\n";
                            for (size_t npcIndex = 0; npcIndex < room->inhabitants.size(); ++npcIndex) {
                                std::cout << (npcIndex + 1)  << ") ";
                                PrintCombatNPCData(gameState, room->inhabitants[npcIndex]);
                            }
                            std::cout << "\n\nTurns: " << gameState.player.usedTurns << "/" << gameState.player.turns << "\n-------------------------------\nActions:\n";
                            for (size_t i = 0; i < gameState.player.actions.size(); ++i) {
                                std::cout << (i + 1) << ") " << gameState.player.actions[i].name << "\n";
                            }
                            std::cout << "\nOption: ";

                            SafeInput<int32_t>(choice);

                            if (--choice < gameState.player.actions.size()) {
                                size_t action = choice;
                                std::cout << "-------------------------------\nTargets:\n";
                                for (size_t npcIndex = 0; npcIndex < room->inhabitants.size(); ++npcIndex) {
                                    std::cout << (npcIndex + 1)  << ") ";
                                    PrintCombatNPCData(gameState, room->inhabitants[npcIndex]);
                                }
                                std::cout << (room->inhabitants.size() + 1)  << ") Yourself\n- HP [" << gameState.player.curHP << "/" << gameState.player.maxHP << "]\n- Mana [" << gameState.player.curMana << "/" << gameState.player.maxMana << "]\n"<< (room->inhabitants.size() + 2)  << ") Back\n\nOption: ";
                                SafeInput<int32_t>(choice);
                                if (--choice < room->inhabitants.size()) {
                                    std::cout << "-------------------------------\n";
                                    if (gameState.player.actions[action].condition == nullptr || gameState.player.actions[action].condition(gameState, &gameState.player, &room->inhabitants[choice])) {
                                        gameState.player.actions[action].action(gameState, &gameState.player, &room->inhabitants[choice]);
                                    }
                                } else if (choice == room->inhabitants.size()) {
                                    std::cout << "-------------------------------\n";
                                    if (gameState.player.actions[action].condition == nullptr || gameState.player.actions[action].condition(gameState, &gameState.player, &gameState.player)) {
                                        gameState.player.actions[action].action(gameState, &gameState.player, &gameState.player);
                                    }
                                } else {
                                    enemiesGo = false;
                                    if (choice != room->inhabitants.size() + 1) {
                                        std::cout << "Invalid choice." << std::endl;
                                    }
                                }

                                if (gameState.player.usedTurns < gameState.player.turns) {
                                    enemiesGo = false;
                                }

                                if (room->LivingInhabitants() <= 0) {
                                    enemiesGo = true;
                                }

                                if (enemiesGo) {
                                    gameState.player.usedTurns = 0;

                                    for (size_t npcIndex = 0; npcIndex < room->inhabitants.size(); ++npcIndex) {
                                        if (room->inhabitants[npcIndex].stunned) {
                                            room->inhabitants[npcIndex].stunned = false;
                                            std::cout << "The " << room->inhabitants[npcIndex].name << " is stunned." << std::endl;
                                        } else if (room->inhabitants[npcIndex].curHP <= 0) {
                                            std::cout << "The " << room->inhabitants[npcIndex].name << " is dead." << std::endl;
                                        } else if (room->inhabitants[npcIndex].aiFunction != nullptr) {
                                            (*room->inhabitants[npcIndex].aiFunction)(gameState, room->inhabitants[npcIndex]);
                                        }
                                        EatInput();
                                    }

                                    room = &gameState.rooms[gameState.curRoom];

                                    std::vector<size_t> remove = std::vector<size_t>();

                                    for (int32_t j = room->inhabitants.size() - 1; j > -1; --j) {
                                        if (room->inhabitants[j].curHP <= 0) {
                                            gameState.player.xp += room->inhabitants[j].xp;
                                            gameState.player.gold += room->inhabitants[j].gold;
                                            for (size_t k = 0; k < room->inhabitants[j].onDeath.size(); ++k) {
                                                gameState.rooms[gameState.curRoom].inhabitants[j].onDeath[k](gameState, gameState.rooms[gameState.curRoom].inhabitants[j]);
                                            }
                                            remove.push_back(j);
                                        } else {
                                            room->inhabitants[j].TurnEnd(gameState);
                                        }
                                    }

                                    for (const size_t& toRemove : remove) {
                                        room->inhabitants.erase(room->inhabitants.begin() + toRemove);
                                        room = &gameState.rooms[gameState.curRoom];
                                    }

                                    if (gameState.player.curHP <= 0) {
                                        std::cout << "\nYou collapse to the floor, dead." << std::endl;
                                        gameState.screen = Screen::GAME_OVER;
                                        gameState.menu = Menu::NONE;
                                    } else {
                                        gameState.player.TurnEnd(gameState);
                                    }
                                }
                            }
                        } else {
                            gameState.menu = Menu::NONE;
                        }
                            */
                        break;
                    }
                }
                break;
            }
            case Screen::GAME_OVER: {
                room = &gameState.rooms[gameState.curRoom];
                const RoomData& roomType = GameData::ROOM_DATA[room->roomID];
                std::cout 
                    << "-------------------------------\n> >> >>> MEMENTO  MORI <<< << <\n"
                    << ">>> STATISTICS\n>> Exploration\nRooms: "
                    << gameState.usedRooms
                    << "\n\n>> Death Room\nName: "
                    << roomType.roomName
                    << "\nDescription: "
                    << roomType.roomDescription
                    << "\nIndex: "
                    << gameState.curRoom
                    << "\n\n>>> PLAYER\n>> Stats\nHP: " 
                    << gameState.player.maxHP 
                    << "\nMana: " 
                    << gameState.player.maxMana 
                    << "\nXP: " 
                    << gameState.player.xp 
                    << "\nGold: " 
                    << gameState.player.gold 
                    << "\n\n>> Classes\n";

                for (const std::pair<std::string, ClassInstance>& classPair : gameState.player.classes) {
                    std::cout << classPair.first << "(" << classPair.second.level << "/" << GameData::CLASSES.at(classPair.first).levels.size() << "): " << GameData::CLASSES.at(classPair.first).description << "\n";
                }
                    
                std::cout << "\n>> Perks\n";

                for (size_t perkIndex = 0; perkIndex < (size_t)Perks::SIZE; ++perkIndex) {
                    if (gameState.player.perks.test(perkIndex)) {
                        switch ((Perks)perkIndex) {
                            case Perks::INSIGHT:
                                std::cout << " - Insight\n";
                                break;
                            case Perks::ARCANE_EYES:
                                std::cout << " - Arcane Eyes\n";
                                break;
                            case Perks::HARD_HITTER:
                                std::cout << " - Hard Hitter\n";
                                break;
                            case Perks::HORDE_SLAYER:
                                std::cout << " - Horde Slayer\n";
                                break;
                        }
                    }
                }
                
                std::cout << "\n>> Skills\n";

                for (std::pair<std::string, int32_t> skillPair : gameState.player.skills) {
                    std::cout << "- " << skillPair.first << ": " << skillPair.second << "\n";
                }

                std::cout << "\n>> Inventory\n";
                for (size_t inventoryItem = 0; inventoryItem < gameState.player.items.size();) {
                    std::cout << ++inventoryItem << ". [" << (gameState.player.equipped[inventoryItem - 1] ? 'X' : ' ') << "] " << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].name << " x" << gameState.player.items[inventoryItem - 1].stackSize << "/" << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].maxStack << "\n -" << GameData::ITEM_DATA[gameState.player.items[inventoryItem - 1].itemID].description << "\n";
                }

                std::cout << "-------------------------------\n1) Title Screen\n2) Quit\n------------------------------\n\nOption: ";
                SafeInput<int32_t>(choice);
                switch (choice) {
                    case 1:
                        gameState.screen = Screen::TITLE;
                        break;
                    case 2:
                        gameState.running = false;
                        break;
                }
                break;
            }
        }
    
        if (gameState.uiManager.dirtyRecalculate) {
            gameState.uiManager.Recalculate(gameState);
        }

        SDL_SetRenderDrawColor(gameState.renderer, 5, 5, 25, SDL_ALPHA_OPAQUE);
        SDL_SetRenderDrawBlendMode(gameState.renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderClear(gameState.renderer);

        switch (gameState.screen) {
            case Screen::TITLE: {
                break;
            }
            case Screen::GAME: {
                switch (gameState.menu) {
                    case Menu::NONE: {
                        SDL_FRect thingy = {0, 140, 1200, 600};
                        SDL_RenderTexture(gameState.renderer, gameState.assets.textures.at(GameData::ROOM_DATA[gameState.rooms[gameState.curRoom].roomID].roomName)->texture, NULL, &thingy);
                        break;
                    }
                    case Menu::COMBAT: {
                        SDL_FRect barBack = {0, 598, 24, 104};
                        SDL_SetRenderDrawColor(gameState.renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
                        SDL_RenderFillRect(gameState.renderer, &barBack);
                        SDL_FRect fillBack = {2, 600, 20, (102.0f - (2.0f * gameState.player.turns)) / gameState.player.turns};
                        SDL_SetRenderDrawColor(gameState.renderer, 55, 55, 55, SDL_ALPHA_OPAQUE);
                        for (size_t i = 0; i < gameState.player.usedTurns; ++i) {
                            SDL_RenderFillRect(gameState.renderer, &fillBack);
                            fillBack.y += fillBack.h + 2;
                        }
                        SDL_SetRenderDrawColor(gameState.renderer, 75, 155, 75, SDL_ALPHA_OPAQUE);
                        for (size_t i = 0; i < gameState.player.turns - gameState.player.usedTurns; ++i) {
                            SDL_RenderFillRect(gameState.renderer, &fillBack);
                            fillBack.y += fillBack.h + 2;
                        }

                        //SDL_SetRenderDrawColor(gameState.renderer, 205, 125, 125, 200);
                        for (size_t i = combatNPCs.size() - 1; i >= 0; --i) {
                            SDL_RenderTexture(gameState.renderer, combatNPCs[i].texture->texture, NULL, &combatNPCs[i].area);
                        }
                        break;
                    }
                }
                break;
            }
        }

        gameState.uiManager.Render(gameState);

        SDL_RenderPresent(gameState.renderer);
        SDL_Delay(8);
        //std::cout << "Very end" << std::endl;
    }

    DebugLogging::CloseParserLog();

    return 0;
}