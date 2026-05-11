// CSC 134
// M3LAB1
// Daley Ottersbach
// 3/6/2026

#define SCRIPT_PARSER_DEBUG_LOGGING 1
//#define LEGACY_COMMA_REPLACEMENT 1

//#define __STDC_WANT_LIB_EXT1__ 1
//#include <cstring>
#ifdef _WIN32_
#define _WIN32_WINDOWS
#endif
#include <ArsTimoris/Assets/Assets.h>
#include <ArsTimoris/Assets/AudioAsset.h>
#include <ArsTimoris/Assets/FontAsset.h>
#include <ArsTimoris/Assets/TextureAsset.h>
#include <ArsTimoris/Commands/CommandHandler.h>
#include <ArsTimoris/DataComponents/DataContainer.h>
#include <ArsTimoris/Game/Map/RoomData.h>
#include <ArsTimoris/Game/GameState.h>
#include <ArsTimoris/Game/TimeData.h>
#include <ArsTimoris/UI/Text/FontAtlas.h>
#include <ArsTimoris/UI/UIAtlasTextComponent.h>
#include <ArsTimoris/UI/UIImageComponent.h>
#include <ArsTimoris/UI/UILazyTextComponent.h>
#include <ArsTimoris/UI/UIManager.h>
#include <ArsTimoris/UI/UISliderComponent.h>
#include <ArsTimoris/Util/Input.hpp>
#include <ArsTimoris/Util/Random.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <chrono>
#include <print>
#include <thread>

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
    DebugLogging::OpenParserLog(std::filesystem::current_path() / "Logs" / "ParserLog.log");
    std::filesystem::path BaseAssetsPath = std::filesystem::current_path() / "Assets";
    std::filesystem::path AssetsPath = BaseAssetsPath / "Assets";
    std::filesystem::path ResourcesPath = BaseAssetsPath / "Resources";
    std::filesystem::path DataPath = BaseAssetsPath / "Data";

    std::println("Prae loading Starts");
    for (const StartData& start : LoadStartData(DataPath / "starts.data")) {
        GameState::Starts.push_back(start);
    }

    #pragma region Random Setup
    std::random_device random = std::random_device();
    GameState gameState = GameState(std::mt19937(random()), Interpreter());
    #pragma endregion

    #pragma region Initial Setup
    std::println("Prae loading Encounters");
    for (const Encounter& encounter : LoadEncountersData(DataPath / "encounters.data")) {
        GameData::ENCOUNTERS.push_back(encounter);
    }

    std::println("Prae loading Entity Templates");
    for (const std::pair<std::string, EntityTemplate>& entityTemplate : LoadEntityData(DataPath / "entities.data")) {
        GameData::ENTITY_TEMPLATES.emplace(entityTemplate);
    }

    struct NPCDisplay {
    public:
        SDL_FRect area;
        SDL_Texture* armorTexture;
        SDL_FRect armorArea;
        std::shared_ptr<ArsTimoris::Assets::NPCRendererAsset> renderer;
        size_t index;

        NPCDisplay(GameState& a_gameState, SDL_FRect a_area, int32_t a_armor, std::shared_ptr<ArsTimoris::Assets::NPCRendererAsset> a_renderer, size_t a_index) {
            this->area = a_area;
            this->renderer = a_renderer;
            this->index = a_index;
            SDL_Surface* armorSurface = a_gameState.assets.fontAtlases.at("BitCrusher")->fontAtlas->RenderWrapped(std::format("{}", a_armor), 2, {255, 255, 255, SDL_ALPHA_OPAQUE}, (int32_t)area.w);
            this->armorArea = SDL_FRect{area.x, area.y, (float)armorSurface->w * 2.0f, (float)armorSurface->h * 2.0f};
            this->armorTexture = SDL_CreateTextureFromSurface(a_gameState.renderer, armorSurface);
            SDL_SetTextureScaleMode(armorTexture, SDL_SCALEMODE_NEAREST);
            SDL_DestroySurface(armorSurface);
        }

        void UpdateArmor(GameState& a_gameState, int32_t a_armor) {
            SDL_DestroyTexture(armorTexture);
            SDL_Surface* armorSurface = a_gameState.assets.fontAtlases.at("BitCrusher")->fontAtlas->RenderWrapped(std::format("{}", a_armor), 2, {255, 255, 255, SDL_ALPHA_OPAQUE}, (int32_t)area.w);
            this->armorArea = SDL_FRect{area.x, area.y, (float)armorSurface->w * 2.0f, (float)armorSurface->h * 2.0f};
            this->armorTexture = SDL_CreateTextureFromSurface(a_gameState.renderer, armorSurface);
            SDL_SetTextureScaleMode(armorTexture, SDL_SCALEMODE_NEAREST);
            SDL_DestroySurface(armorSurface);
        }
    };
    std::vector<NPCDisplay> combatNPCs = std::vector<NPCDisplay>();
    
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

    SDL_SetRenderDrawBlendMode(gameState.renderer, SDL_BLENDMODE_BLEND);

    gameState.audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    
    std::println("Prae Assets");
    gameState.assets = ArsTimoris::Assets::Assets();

    std::println("Prae UI Manager");
    gameState.uiManager = ArsTimoris::UI::UIManager();
    gameState.assets.uiManager = &gameState.uiManager;

    std::println("Prae Resources");
    for (std::filesystem::directory_entry const& dir_entry : std::filesystem::recursive_directory_iterator(ResourcesPath)) {
        if (dir_entry.path().has_extension()) {
            if (dir_entry.path().extension().string() == ".png") {
                gameState.assets.AddTexture(gameState.renderer, dir_entry.path().string(), dir_entry.path().stem().string());
            } else if (dir_entry.path().extension().string() == ".fpng") {
                gameState.assets.AddFontAtlas(gameState.renderer, dir_entry.path().string(), dir_entry.path().stem().string());
            } else if (dir_entry.path().extension().string() == ".ttf") {
                gameState.assets.AddLazyFont(dir_entry.path().string(), dir_entry.path().stem().string());
            } else if (dir_entry.path().extension().string() == ".wav") {
                gameState.assets.AddSound(gameState.audioDevice, dir_entry.path().string(), dir_entry.path().stem().string());
            }
        }
    }

    std::println("Prae Data Assets");
    for (std::filesystem::directory_entry const& dir_entry : std::filesystem::recursive_directory_iterator(AssetsPath)) {
        if (dir_entry.path().has_extension()) {
            if (dir_entry.path().extension().string() == ".render") {
                gameState.assets.AddNPCRenderer(dir_entry.path().string(), dir_entry.path().stem().string());
            } else if (dir_entry.path().extension().string() == ".particle") {
                gameState.assets.AddParticle(dir_entry.path().string(), dir_entry.path().stem().string());
            }
        }
    }
    #pragma endregion

    #pragma region Setup Vars
    RoomInstance* room = nullptr;
    int32_t choice;
    std::vector<std::string> classNames = std::vector<std::string>();
    for (std::pair<std::string, ClassData> paired : GameData::CLASSES) {
        classNames.push_back(paired.first);
    }
    #pragma endregion

    
    std::function<void(GameState&)> menuChangedCallback = nullptr;
    bool menuToChange = false;

    #pragma region UI
    #pragma region Layers
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
        std::forward_as_tuple("Shop Menu"), 
        std::forward_as_tuple(std::string_view("Shop Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Room Actions Menu"), 
        std::forward_as_tuple(std::string_view("Room Actions Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Combat Menu"), 
        std::forward_as_tuple(std::string_view("Combat Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Level Menu"), 
        std::forward_as_tuple(std::string_view("Level Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Stats Menu"), 
        std::forward_as_tuple(std::string_view("Stats Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Inventory Menu"), 
        std::forward_as_tuple(std::string_view("Inventory Menu"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Message Overlay"), 
        std::forward_as_tuple(std::string_view("Message Overlay"))
    );
    gameState.uiManager.uiLayers.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Combat Menu Decoration"), 
        std::forward_as_tuple(std::string_view("Combat Menu Decoration"))
    );

    ArsTimoris::UI::UILayer* mainMenu = &gameState.uiManager.uiLayers.at("Main Menu");
    gameState.uiManager.renderOrder.emplace_back("Main Menu");
    mainMenu->enabled = true;
    ArsTimoris::UI::UILayer* settingsMenu = &gameState.uiManager.uiLayers.at("Settings Menu");
    gameState.uiManager.renderOrder.emplace_back("Settings Menu");
    settingsMenu->enabled = false;
    ArsTimoris::UI::UILayer* startsMenu = &gameState.uiManager.uiLayers.at("Starts Menu");
    gameState.uiManager.renderOrder.emplace_back("Starts Menu");
    startsMenu->enabled = false;
    ArsTimoris::UI::UILayer* roomGeneralMenu = &gameState.uiManager.uiLayers.at("Room General Menu");
    gameState.uiManager.renderOrder.emplace_back("Room General Menu");
    roomGeneralMenu->enabled = false;
    ArsTimoris::UI::UILayer* roomMoveMenu = &gameState.uiManager.uiLayers.at("Room Move Menu");
    gameState.uiManager.renderOrder.emplace_back("Room Move Menu");
    roomMoveMenu->enabled = false;
    ArsTimoris::UI::UILayer* roomActionsMenu = &gameState.uiManager.uiLayers.at("Room Actions Menu");
    gameState.uiManager.renderOrder.emplace_back("Room Actions Menu");
    roomActionsMenu->enabled = false;
    ArsTimoris::UI::UILayer* combatMenu = &gameState.uiManager.uiLayers.at("Combat Menu");
    gameState.uiManager.renderOrder.emplace_back("Combat Menu");
    combatMenu->enabled = false;
    ArsTimoris::UI::UILayer* statsMenu = &gameState.uiManager.uiLayers.at("Stats Menu");
    gameState.uiManager.renderOrder.emplace_back("Stats Menu");
    statsMenu->enabled = false;
    ArsTimoris::UI::UILayer* levelMenu = &gameState.uiManager.uiLayers.at("Level Menu");
    gameState.uiManager.renderOrder.emplace_back("Level Menu");
    levelMenu->enabled = false;
    ArsTimoris::UI::UILayer* inventoryMenu = &gameState.uiManager.uiLayers.at("Inventory Menu");
    gameState.uiManager.renderOrder.emplace_back("Inventory Menu");
    inventoryMenu->enabled = false;
    ArsTimoris::UI::UILayer* shopMenu = &gameState.uiManager.uiLayers.at("Shop Menu");
    gameState.uiManager.renderOrder.emplace_back("Shop Menu");
    shopMenu->enabled = false;
    ArsTimoris::UI::UILayer* messageOverlay = &gameState.uiManager.uiLayers.at("Message Overlay");
    gameState.uiManager.renderOrder.emplace_back("Message Overlay");
    messageOverlay->enabled = false;
    ArsTimoris::UI::UILayer* combatMenuDecoration = &gameState.uiManager.uiLayers.at("Combat Menu Decoration");
    gameState.uiManager.renderOrder.emplace_back("Combat Menu Decoration");
    combatMenuDecoration->enabled = false;
    #pragma endregion

    #pragma region UI Functions Forward Declaration
    std::function<void(void)> UpdateTopBar;
    std::function<void(void)> UpdateLevelUp;
    std::function<void(void)> UpdateInventoryMenu;
    std::function<void(void)> UpdateShopMenu;
    #pragma endregion

    ArsTimoris::UI::UIElement* element;
    ArsTimoris::UI::UIElement* otherElement;
    ArsTimoris::UI::UIElement* othererElement;

    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> classesText;
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> skillsText;
    int32_t classChoice = 0;

    struct InventoryRow {
    public:
        int32_t index;
        std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> equipped;
        std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> header;
        std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> description;
    };

    std::vector<InventoryRow> inventoryRows = std::vector<InventoryRow>();
    int32_t currentInventoryPage = 0;
    
    struct ShopCatalogUIEntry {
    public:
        int32_t index;
        std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> header;
        std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> description;
    };

    std::vector<ShopCatalogUIEntry> shopCatalogs = std::vector<ShopCatalogUIEntry>();
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> shopNameText;
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> shopGoldText;
    int32_t shopChoice;
    int32_t shopPage = 0;
    ShopInstance* shop;

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
        MakeStart(GameState::Starts[i]);
    }
    #pragma endregion
    
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
        roomActionsMenu->enabled = true; 
        gameState.menu = Menu::ROOM_ACTIONS;
        choice = 0;
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
        //a_uiLayer->enabled = false;
        statsMenu->enabled = true; 
        gameState.menu = Menu::STATS;
        std::string setter = "Classes: \n";
        for (const std::pair<std::string, ClassInstance>& classPair : gameState.player.classes) {
            setter += std::format("{}({}/{}): {}\n", classPair.first, classPair.second.level, GameData::CLASSES.at(classPair.first).levels.size(), GameData::CLASSES.at(classPair.first).description);
        }
        classesText->SetText(a_gameState, setter);
        setter = "Skills: \n";
        for (std::pair<std::string, int32_t> skillPair : gameState.player.skills) {
            setter += std::format("- {}: {}\n", skillPair.first, skillPair.second);
        }
        skillsText->SetText(a_gameState, setter);
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
        inventoryMenu->enabled = true;
        UpdateInventoryMenu();
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

    #pragma region Room Actions
    roomActionsMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Enter"), 
        std::forward_as_tuple(
            std::string_view("Enter"), 
            ArsTimoris::UI::UIRect{{0, 740, 240, 60}}
        )
    );
    element = &roomActionsMenu->uiElements.at("Enter");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomActionsMenu, element);
    element->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        a_uiLayer->enabled = false;
        roomGeneralMenu->enabled = true; 
        gameState.menu = Menu::NONE;
        if (!room->roomActions.empty() && room->roomActions[choice].condition(gameState)) {
            room->roomActions[choice].usage(gameState);
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
    ).first->second->Hookup(gameState, roomActionsMenu, element);

    roomActionsMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Label"), 
        std::forward_as_tuple(
            std::string_view("Label"), 
            ArsTimoris::UI::UIRect{{0, 300, 240, 60}}
        )
    );
    element = &roomActionsMenu->uiElements.at("Label");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomActionsMenu, element);
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
    ).first->second->Hookup(gameState, roomActionsMenu, element);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> roomActionText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(element->components.at("Text"));

    roomActionsMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Room Action"), 
        std::forward_as_tuple(
            std::string_view("Room Action"), 
            ArsTimoris::UI::UIRect{{300, 200, 800, 500}}
        )
    );
    element = &roomActionsMenu->uiElements.at("Room Action");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, roomActionsMenu, element);
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Scallion",
                "BitCrusher",
                3.0f,
                6,
                ArsTimoris::UI::UIAnchor::TOP_CENTER
            )
        )
    ).first->second->Hookup(gameState, roomActionsMenu, element);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> roomActionDescriptionText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(element->components.at("Text"));
    #pragma endregion

    #pragma region Combat
    combatMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Combat HUD"), 
        std::forward_as_tuple(
            std::string_view("Combat HUD"), 
            ArsTimoris::UI::UIRect{{302, 202, 796, 496}}
        )
    );
    element = &combatMenu->uiElements.at("Combat HUD");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("CombatHUDBack"), 
                true
            )
        )
    ).first->second->Hookup(gameState, combatMenu, element);

    otherElement = element->AddChild("Background Actions", ArsTimoris::UI::UIRect{{390, 206, 316, 184}}).get(); // {692, 408, 316, 184}
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("CombatActionBacking"), 
                true
            )
        )
    ).first->second->Hookup(gameState, combatMenu, otherElement);

    othererElement = otherElement->AddChild("Top Action", ArsTimoris::UI::UIRect{{28, 2, 260, 50}}).get(); // {720, 410, 260, 50}
    othererElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, combatMenu, othererElement);
    othererElement->components.emplace(std::piecewise_construct, 
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
    ).first->second->Hookup(gameState, combatMenu, othererElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> topActionText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(othererElement->components.at("Text"));
    
    othererElement = otherElement->AddChild("Middle Action", ArsTimoris::UI::UIRect{{8, 62, 300, 60}}).get(); // {700, 470, 300, 60}
    othererElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, combatMenu, othererElement);
    othererElement->components.emplace(std::piecewise_construct, 
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
    ).first->second->Hookup(gameState, combatMenu, othererElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> middleActionText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(othererElement->components.at("Text"));
    
    othererElement = otherElement->AddChild("Bottom Action", ArsTimoris::UI::UIRect{{28, 132, 260, 50}}).get(); // {720, 540, 260, 50}
    othererElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, combatMenu, othererElement);
    othererElement->components.emplace(std::piecewise_construct, 
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
    ).first->second->Hookup(gameState, combatMenu, othererElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> bottomActionText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(othererElement->components.at("Text"));
    #pragma endregion

    #pragma region Message Overlay
    messageOverlay->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Message Action"), 
        std::forward_as_tuple(
            std::string_view("Message Action"), 
            ArsTimoris::UI::UIRect{{300, 200, 800, 500}}
        )
    );
    element = &messageOverlay->uiElements.at("Message Action");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, messageOverlay, element);
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Scallion",
                "BitCrusher",
                3.0f,
                6,
                ArsTimoris::UI::UIAnchor::TOP_CENTER
            )
        )
    ).first->second->Hookup(gameState, messageOverlay, element);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> messageText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(element->components.at("Text"));
    #pragma endregion
    
    #pragma region Stats Menu
    statsMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("StatsPanel"), 
        std::forward_as_tuple(
            std::string_view("StatsPanel"), 
            ArsTimoris::UI::UIRect{{0, 140, 1200, 600}}
        )
    );
    element = &statsMenu->uiElements.at("StatsPanel");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, statsMenu, element);

    otherElement = element->AddChild("Classes", ArsTimoris::UI::UIRect{{5, 5, 700, 550}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, statsMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Classes:",
                "BitCrusher",
                2.0f,
                8,
                8,
                ArsTimoris::UI::UIAnchor::TOP_LEFT
            )
        )
    ).first->second->Hookup(gameState, statsMenu, otherElement);
    classesText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));

    otherElement = element->AddChild("Levels", ArsTimoris::UI::UIRect{{5, 550, 700, 45}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, statsMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Level Up",
                "BitCrusher",
                3.0f,
                8,
                6,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, statsMenu, otherElement);
    otherElement->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        levelMenu->enabled = true;
        statsMenu->enabled = false;
        gameState.menu = Menu::LEVEL_UP;
        UpdateLevelUp();
        return true;
    });

    otherElement = element->AddChild("Skills", ArsTimoris::UI::UIRect{{700, 5, 495, 590}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, statsMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Skills:",
                "BitCrusher",
                2.0f,
                8,
                8,
                ArsTimoris::UI::UIAnchor::TOP_LEFT
            )
        )
    ).first->second->Hookup(gameState, statsMenu, otherElement);
    skillsText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));
    #pragma endregion

    #pragma region Level Menu
    levelMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("LevelPanel"), 
        std::forward_as_tuple(
            std::string_view("LevelPanel"), 
            ArsTimoris::UI::UIRect{{0, 140, 1200, 600}}
        )
    );
    element = &levelMenu->uiElements.at("LevelPanel");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, levelMenu, element);

    //Header: "{} ({}/{})"
    //Description: "{}"
    //Cost: "{}"


    otherElement = element->AddChild("Index", ArsTimoris::UI::UIRect{{5, 5, 195, 55}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, levelMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "1/1",
                "BitCrusher",
                2.0f,
                8,
                8,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, levelMenu, otherElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> classIndexText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));

    otherElement = element->AddChild("Header", ArsTimoris::UI::UIRect{{205, 5, 790, 55}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, levelMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Test Class (0/1)",
                "BitCrusher",
                2.0f,
                8,
                8,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, levelMenu, otherElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> headerClassText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));

    otherElement = element->AddChild("Level", ArsTimoris::UI::UIRect{{1000, 5, 195, 55}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, levelMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Level",
                "BitCrusher",
                2.0f,
                8,
                8,
                ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
            )
        )
    ).first->second->Hookup(gameState, levelMenu, otherElement);
    otherElement->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
        const ClassData& classRef = GameData::CLASSES.at(classNames[classChoice]);
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
                UpdateLevelUp();
            }
        }
        return true;
    });

    otherElement = element->AddChild("Description", ArsTimoris::UI::UIRect{{5, 60, 890, 535}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, levelMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "A simple class.",
                "BitCrusher",
                2.0f,
                8,
                8,
                ArsTimoris::UI::UIAnchor::TOP_CENTER
            )
        )
    ).first->second->Hookup(gameState, levelMenu, otherElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> descriptionOfClassText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));

    otherElement = element->AddChild("Cost", ArsTimoris::UI::UIRect{{900, 60, 295, 535}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, levelMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Requires stuff.",
                "BitCrusher",
                2.0f,
                8,
                8,
                ArsTimoris::UI::UIAnchor::TOP_CENTER
            )
        )
    ).first->second->Hookup(gameState, levelMenu, otherElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> costlyClassText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));

    UpdateLevelUp = [&](void) {
        const ClassData& classData = GameData::CLASSES.at(classNames[classChoice]);
        classIndexText->SetText(gameState, std::format("{:>2}/{:<2}", classChoice + 1, classNames.size()));
        headerClassText->SetText(gameState, std::format("{} ({}/{})", classData.name, (gameState.player.classes.contains(classData.name) ? (gameState.player.classes.at(classData.name).level) : 0), classData.levels.size()));
        descriptionOfClassText->SetText(gameState, classData.description);
        size_t levelToUse = (gameState.player.classes.contains(classData.name) ? (gameState.player.classes.at(classData.name).level + 1) : 1);
        //std::cout << "Level: " << levelToUse << " : " << classPair.second.levels.size() << std::endl;
        if (classData.levels.size() >= levelToUse && classData.levels[--levelToUse].displayRequirements != nullptr) {
            costlyClassText->SetText(gameState, std::format("\\[FCH:{}]{}", ((classData.levels[levelToUse].applicable == nullptr || classData.levels[levelToUse].applicable(gameState, gameState.player)) ? "00,FF,00" : "FF,00,00"), classData.levels[levelToUse].displayRequirements(gameState, gameState.player)));
        }
    };
    #pragma endregion

    #pragma region Inventory Menu
    inventoryMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("InventoryPanel"), 
        std::forward_as_tuple(
            std::string_view("InventoryPanel"), 
            ArsTimoris::UI::UIRect{{0, 140, 1200, 600}}
        )
    );
    element = &inventoryMenu->uiElements.at("InventoryPanel");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, inventoryMenu, element);

    // 80 tall
    // 1190 to work with
    // 195 - 800 - 195 (20)
    // 1190 (60)

    for (int32_t i = 0; i < 7; ++i) {
        inventoryRows.push_back(InventoryRow{i, nullptr, nullptr, nullptr});
        otherElement = element->AddChild(std::format("Equipped{}", i), ArsTimoris::UI::UIRect{{5, 5.0f + 85 * i, 195, 30}}).get();
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("ItemFrame"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, inventoryMenu, otherElement);
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Text"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                    "Equipped",
                    "BitCrusher",
                    2.0f,
                    8,
                    8,
                    ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
                )
            )
        ).first->second->Hookup(gameState, inventoryMenu, otherElement);
        otherElement->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
            int32_t ia = stoi(a_element->id.substr(8, 1));
            std::println("{} Did: {} - {}", a_element->id.substr(8, 1), ia, inventoryRows[ia].index);
            if (inventoryRows[ia].index < gameState.player.items.size()) {
                gameState.player.equipped[inventoryRows[ia].index] = !gameState.player.equipped[inventoryRows[ia].index];
                UpdateInventoryMenu();
            }
            return true;
        });
        inventoryRows.back().equipped = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));

        otherElement = element->AddChild(std::format("Header{}", i), ArsTimoris::UI::UIRect{{200, 5.0f + 85 * i, 800, 30}}).get();
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("ItemFrame"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, inventoryMenu, otherElement);
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Text"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                    "Bla",
                    "BitCrusher",
                    2.0f,
                    8,
                    8,
                    ArsTimoris::UI::UIAnchor::MIDDLE_LEFT
                )
            )
        ).first->second->Hookup(gameState, inventoryMenu, otherElement);
        inventoryRows.back().header = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));

        otherElement = element->AddChild(std::format("Use{}", i), ArsTimoris::UI::UIRect{{1000, 5.0f + 85 * i, 195, 30}}).get();
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("ItemFrame"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, inventoryMenu, otherElement);
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Text"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                    "Use",
                    "BitCrusher",
                    2.0f,
                    8,
                    8,
                    ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
                )
            )
        ).first->second->Hookup(gameState, inventoryMenu, otherElement);
        otherElement->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
            int32_t ia = stoi(a_element->id.substr(3, 1));
            if (inventoryRows[ia].index < gameState.player.items.size()) {
                ItemStack& itemStack = gameState.player.items[inventoryRows[ia].index];
                const ItemData& itemType = GameData::ITEM_DATA[itemStack.itemID];
                if (itemType.usage.usage != nullptr) {
                    if (itemType.usage.condition == nullptr || itemType.usage.condition(gameState, itemStack, inventoryRows[ia].index)) {
                        itemType.usage.usage(gameState, itemStack, inventoryRows[ia].index);
                        UpdateInventoryMenu();
                    } else {
                        std::cout << "You don't know why you would use this right now." << std::endl;
                    }
                } else {
                    std::cout << "This item is not usable." << std::endl;
                }
            }
            return true;
        });
        
        otherElement = element->AddChild(std::format("Description{}", i), ArsTimoris::UI::UIRect{{5, 35.0f + 85 * i, 1190, 50}}).get();
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("GoldPanel"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, inventoryMenu, otherElement);
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Text"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                    "Description",
                    "BitCrusher",
                    2.0f,
                    8,
                    8,
                    ArsTimoris::UI::UIAnchor::TOP_LEFT
                )
            )
        ).first->second->Hookup(gameState, inventoryMenu, otherElement);
        inventoryRows.back().description = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));
    }
    #pragma endregion
    
    #pragma region Shop Menu
    shopMenu->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("ShopPanel"), 
        std::forward_as_tuple(
            std::string_view("ShopPanel"), 
            ArsTimoris::UI::UIRect{{0, 140, 1200, 600}}
        )
    );
    element = &shopMenu->uiElements.at("ShopPanel");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, shopMenu, element);

    otherElement = element->AddChild("ShopName", ArsTimoris::UI::UIRect{{5, 5, 990, 35}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, shopMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Buy",
                "BitCrusher",
                3.0f,
                8,
                8,
                ArsTimoris::UI::UIAnchor::MIDDLE_LEFT
            )
        )
    ).first->second->Hookup(gameState, shopMenu, otherElement);
    shopNameText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));

    otherElement = element->AddChild("ShopGold", ArsTimoris::UI::UIRect{{1000, 5, 195, 35}}).get();
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("UIPanel"), 
                true
            )
        )
    ).first->second->Hookup(gameState, shopMenu, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "GP: 0",
                "BitCrusher",
                3.0f,
                8,
                8,
                ArsTimoris::UI::UIAnchor::MIDDLE_LEFT
            )
        )
    ).first->second->Hookup(gameState, shopMenu, otherElement);
    shopGoldText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));

    for (int32_t i = 0; i < 6; ++i) {
        shopCatalogs.push_back(ShopCatalogUIEntry{i, nullptr, nullptr});
        otherElement = element->AddChild(std::format("Buy{}", i), ArsTimoris::UI::UIRect{{5, 105.0f + 90 * i, 95, 30}}).get();
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("UIPanel"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, shopMenu, otherElement);
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Text"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                    "Buy",
                    "BitCrusher",
                    2.0f,
                    8,
                    8,
                    ArsTimoris::UI::UIAnchor::MIDDLE_CENTER
                )
            )
        ).first->second->Hookup(gameState, shopMenu, otherElement);
        otherElement->onMouseLeftDown.emplace_back([&](GameState& a_gameState, ArsTimoris::UI::UILayer* a_uiLayer, ArsTimoris::UI::UIElement* a_element, SDL_FPoint* a_mousePos) {
            int32_t ia = stoi(a_element->id.substr(3, 1));
            shop = (ShopInstance*)gameState.rooms[gameState.curRoom].data.at("Shop");
            if (shopCatalogs[ia].index < shop->catalog.size()) {
                int32_t option = shopCatalogs[ia].index;
                if (shop->catalog[option].cost <= a_gameState.player.gold) {
                    a_gameState.player.gold -= (int32_t)shop->catalog[option].cost;
                    a_gameState.player.AddItemStack(shop->catalog[option].stack);
                    std::cout << "You bought 1 " << GameData::ITEM_DATA[shop->catalog[option].stack.itemID].name << " for " << shop->catalog[option].cost << " gold." << std::endl;
                    if (--shop->catalog[option].stack.stackSize <= 0) {
                        shop->catalog.erase(shop->catalog.begin() + option);
                    }
                    UpdateShopMenu();
                } else {
                    std::cout << "You do not have enough gold." << std::endl;
                }
            }
            return true;
        });

        otherElement = element->AddChild(std::format("Header{}", i), ArsTimoris::UI::UIRect{{100, 105.0f + 90 * i, 405, 30}}).get();
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("UIPanel"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, shopMenu, otherElement);
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Text"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                    "Bla",
                    "BitCrusher",
                    2.0f,
                    8,
                    8,
                    ArsTimoris::UI::UIAnchor::MIDDLE_LEFT
                )
            )
        ).first->second->Hookup(gameState, shopMenu, otherElement);
        shopCatalogs.back().header = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));

        otherElement = element->AddChild(std::format("Description{}", i), ArsTimoris::UI::UIRect{{5, 135.0f + 90 * i, 500, 60}}).get();
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Texture"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIImageComponent>(
                    std::string_view("UIPanel"), 
                    true
                )
            )
        ).first->second->Hookup(gameState, shopMenu, otherElement);
        otherElement->components.emplace(std::piecewise_construct, 
            std::forward_as_tuple("Text"), 
            std::forward_as_tuple(
                std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                    "Bla",
                    "BitCrusher",
                    2.0f,
                    8,
                    8,
                    ArsTimoris::UI::UIAnchor::TOP_LEFT
                )
            )
        ).first->second->Hookup(gameState, shopMenu, otherElement);
        shopCatalogs.back().description = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));
    }
    #pragma endregion
    
    #pragma region Combat Decorations
    combatMenuDecoration->uiElements.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Combat HUD Top"), 
        std::forward_as_tuple(
            std::string_view("Combat HUD Top"), 
            ArsTimoris::UI::UIRect{{0, 164, 1200, 636}}
        )
    );
    element = &combatMenuDecoration->uiElements.at("Combat HUD Top");
    element->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("CombatHUDTop"), 
                true
            )
        )
    ).first->second->Hookup(gameState, combatMenuDecoration, element);

    otherElement = element->AddChild("Armor Icon Label", ArsTimoris::UI::UIRect{{4, 542, 68, 62}}).get(); // {4, 706, 68, 62}
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Texture"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIImageComponent>(
                std::string_view("ArmorIcon"), 
                true
            )
        )
    ).first->second->Hookup(gameState, combatMenuDecoration, otherElement);
    otherElement->components.emplace(std::piecewise_construct, 
        std::forward_as_tuple("Text"), 
        std::forward_as_tuple(
            std::make_shared<ArsTimoris::UI::UIAtlasTextComponent>(
                "Scallion",
                "BitCrusher",
                2.0f,
                6,
                20,
                ArsTimoris::UI::UIAnchor::TOP_CENTER
            )
        )
    ).first->second->Hookup(gameState, combatMenuDecoration, otherElement);
    std::shared_ptr<ArsTimoris::UI::UIAtlasTextComponent> armorIconLabelText = std::dynamic_pointer_cast<ArsTimoris::UI::UIAtlasTextComponent>(otherElement->components.at("Text"));
    #pragma endregion

    #pragma region UI Functions IMPL
    UpdateTopBar = [&](void) {
        hpText->SetText(gameState, std::format("HP: {:>3}/{:<3}", gameState.player.curHP, gameState.player.maxHP));
        mpText->SetText(gameState, std::format("MP: {:>3}/{:<3}", gameState.player.curMana, gameState.player.maxMana));
        acText->SetText(gameState, std::format("AC: {}", gameState.player.GetEffectiveArmor()));
        xpText->SetText(gameState, std::format("XP: {}", gameState.player.xp));
        gpText->SetText(gameState, std::format("GP: {}", gameState.player.gold));
    };

    UpdateInventoryMenu = [&](void) {
        int32_t inventoryPages = (int32_t)ceilf((float)gameState.player.items.size() / 7.0f);
        if (currentInventoryPage > inventoryPages) {
            currentInventoryPage = inventoryPages;
        }
        int32_t startIndex = currentInventoryPage * 7;
        for (int32_t i = 0; i < 7; ++i) {
            inventoryRows[i].index = startIndex + i;
            if (i + startIndex < gameState.player.items.size()) {
                inventoryRows[i].equipped->SetText(gameState, gameState.player.equipped[i + startIndex] ? "Unequip" : "Equip");
                inventoryRows[i].header->SetText(gameState, std::format("{} x{}/{}", GameData::ITEM_DATA[gameState.player.items[i + startIndex].itemID].name, gameState.player.items[i + startIndex].stackSize, GameData::ITEM_DATA[gameState.player.items[i + startIndex].itemID].maxStack));
                std::string descriptor = GameData::ITEM_DATA[gameState.player.items[i + startIndex].itemID].description;
                for (const std::pair<std::string, int32_t>& modPair : gameState.player.items[i + startIndex].metadata) {
                    descriptor += std::format("; {}: {}", modPair.first, modPair.second);
                }
                inventoryRows[i].description->SetText(gameState, descriptor);
            } else {
                inventoryRows[i].equipped->SetText(gameState, "-");
                inventoryRows[i].header->SetText(gameState, "-");
                inventoryRows[i].description->SetText(gameState, "-");
            }
        }
    };

    UpdateShopMenu = [&](void) {
        shop = (ShopInstance*)gameState.rooms[gameState.curRoom].data.at("Shop");
        shopNameText->SetText(gameState, shop->shopName);
        int32_t shopPager = shopPage * 6;
        for (int32_t i = 0; i < 6; ++i) {
            shopCatalogs[i].index = shopPager + i;
            if (shopPager + i < shop->catalog.size()) {
                shopCatalogs[i].header->SetText(gameState, std::format("{} ({}) x{}", GameData::ITEM_DATA[shop->catalog[i].stack.itemID].name, shop->catalog[i].cost, shop->catalog[i].stack.stackSize));
                std::string descriptor = GameData::ITEM_DATA[shop->catalog[i].stack.itemID].description;
                for (const std::pair<std::string, int32_t>& modPair : shop->catalog[i].stack.metadata) {
                    descriptor += std::format("; {}: {}", modPair.first, modPair.second);
                }
                shopCatalogs[i].description->SetText(gameState, descriptor);
            } else {
                shopCatalogs[i].header->SetText(gameState, "-");
                shopCatalogs[i].description->SetText(gameState, "-");
            }
        }
        shopGoldText->SetText(gameState, std::format("GP: {}", gameState.player.gold));
    };
    #pragma endregion
    #pragma endregion

    if (gameState.uiManager.dirtyRecalculate) {
        gameState.uiManager.Recalculate(gameState);
    }

    size_t encounter;
    bool runEncounter = false;
    SDL_Event event;
    TimeData timeData = TimeData{0, SDL_GetTicks(), 0.0f, 0.0f};
    gameState.inputData.keys = SDL_GetKeyboardState(&gameState.inputData.numKeys);
    gameState.inputData.oldKeys = (bool*)malloc(gameState.inputData.numKeys);

    #pragma region Commands
    ArsTimoris::Commands::CommandHandler commandHandler = ArsTimoris::Commands::CommandHandler();

    commandHandler.AddCommand(ArsTimoris::Commands::Command(
        "help", 
        "Display all available commands.",
        [&](const ArsTimoris::Commands::CommandHandler& a_handler, const std::vector<ArsTimoris::Commands::Parameter>& a_parameters) {
            std::cout << "Available commands:" << std::endl;
            for (ArsTimoris::Commands::Command command : a_handler.commands) {
                std::cout << "\t" << command.name << ": " << command.description << std::endl;
            }
        }
    ));

    commandHandler.AddCommand(ArsTimoris::Commands::Command(
        "add_gold", 
        "Add an amount of gold to the player.",
        [&](const ArsTimoris::Commands::CommandHandler& handler, const std::vector<ArsTimoris::Commands::Parameter>& parameters) {
            if (parameters.size() < 1) {
                std::cout << "Not enough arguments." << std::endl;
                return;
            }

            if (parameters[0].index() != ArsTimoris::Commands::ParameterType::INT) {
                std::cout << "Expected integer for amount but got a different type." << std::endl;
                return;
            }

            gameState.player.gold += std::get<int>(parameters[0]);
        }, {ArsTimoris::Commands::ParameterType::INT}
    ));

    commandHandler.AddCommand(ArsTimoris::Commands::Command(
        "add_xp", 
        "Add an amount of xp to the player.",
        [&](const ArsTimoris::Commands::CommandHandler& handler, const std::vector<ArsTimoris::Commands::Parameter>& parameters) {
            if (parameters.size() < 1) {
                std::cout << "Not enough arguments." << std::endl;
                return;
            }

            if (parameters[0].index() != ArsTimoris::Commands::ParameterType::INT) {
                std::cout << "Expected integer for amount but got a different type." << std::endl;
                return;
            }
            
            gameState.player.xp += std::get<int>(parameters[0]);
        }, {ArsTimoris::Commands::ParameterType::INT}
    ));

    commandHandler.AddCommand(ArsTimoris::Commands::Command(
        "add_hp", 
        "Add an amount of hp to the player.",
        [&](const ArsTimoris::Commands::CommandHandler& handler, const std::vector<ArsTimoris::Commands::Parameter>& parameters) {
            if (parameters.size() < 1) {
                std::cout << "Not enough arguments." << std::endl;
                return;
            }

            if (parameters[0].index() != ArsTimoris::Commands::ParameterType::INT) {
                std::cout << "Expected integer for amount but got a different type." << std::endl;
                return;
            }

            int value = std::get<int>(parameters[0]);
            if (value < 0) {
                gameState.player.Hurt(value);
            } else {
                gameState.player.Heal(value);
            }
        }, {ArsTimoris::Commands::ParameterType::INT}
    ));

    commandHandler.AddCommand(ArsTimoris::Commands::Command(
        "add_mp", 
        "Add an amount of mana to the player.",
        [&](const ArsTimoris::Commands::CommandHandler& handler, const std::vector<ArsTimoris::Commands::Parameter>& parameters) {
            if (parameters.size() < 1) {
                std::cout << "Not enough arguments." << std::endl;
                return;
            }

            if (parameters[0].index() != ArsTimoris::Commands::ParameterType::INT) {
                std::cout << "Expected integer for amount but got a different type." << std::endl;
                return;
            }
            

            int value = std::get<int>(parameters[0]);
            if (value < 0) {
                gameState.player.DrainMana(value);
            } else {
                gameState.player.RegainMana(value);
            }
        }, {ArsTimoris::Commands::ParameterType::INT}
    ));

    commandHandler.AddCommand(ArsTimoris::Commands::Command(
        "add_turns", 
        "Add an amount of turns to the player.",
        [&](const ArsTimoris::Commands::CommandHandler& handler, const std::vector<ArsTimoris::Commands::Parameter>& parameters) {
            if (parameters.size() < 1) {
                std::cout << "Not enough arguments." << std::endl;
                return;
            }

            if (parameters[0].index() != ArsTimoris::Commands::ParameterType::INT) {
                std::cout << "Expected integer for amount but got a different type." << std::endl;
                return;
            }
            
            gameState.player.turns += std::get<int>(parameters[0]);
        }, {ArsTimoris::Commands::ParameterType::INT}
    ));

    commandHandler.AddCommand(ArsTimoris::Commands::Command(
        "debug", 
        "Toggle debug mode.",
        [&](const ArsTimoris::Commands::CommandHandler& handler, const std::vector<ArsTimoris::Commands::Parameter>& parameters) {
            gameState.debug.enabled = !gameState.debug.enabled;
            std::cout << "Debug mode " << (gameState.debug.enabled ? "enabled" : "disabled") << "." << std::endl;
        }
    ));

    commandHandler.AddCommand(ArsTimoris::Commands::Command(
        "no_fights", 
        "Toggle No Fights cheat.",
        [&](const ArsTimoris::Commands::CommandHandler& handler, const std::vector<ArsTimoris::Commands::Parameter>& parameters) {
            gameState.debug.noFights = !gameState.debug.noFights;
            std::cout << "No Fights " << (gameState.debug.noFights ? "enabled" : "disabled") << "." << std::endl;
        }
    ));

    commandHandler.AddCommand(ArsTimoris::Commands::Command(
        "draw_hitboxes", 
        "Toggle Draw Hitboxes cheat.",
        [&](const ArsTimoris::Commands::CommandHandler& handler, const std::vector<ArsTimoris::Commands::Parameter>& parameters) {
            gameState.debug.drawHitboxes = !gameState.debug.drawHitboxes;
            std::cout << "Draw Hitboxes " << (gameState.debug.drawHitboxes ? "enabled" : "disabled") << "." << std::endl;
        }
    ));

    //commandHandler.AddCommand(Command("", [&](const CommandHandler& handler, std::vector<Parameter> parameters) {}));

    //commandHandler.AddCommand(ArsTimoris::Commands::Command("", [&](const ArsTimoris::Commands::CommandHandler& handler, const std::vector<ArsTimoris::Commands::Parameter>& parameters) {}));
    #pragma endregion

    // Launch a separate thread to handle console input
    std::thread inputThread([&]() {
        std::string command = "";
        while (gameState.running) {
            std::getline(std::cin, command);

            if (command.empty()) {
                continue;
            }

            // Process the command
            // Trim leading and trailing whitespace
            command = command.substr(command.find_first_not_of(" \t\r\n"));
            if (command.empty()) {
                continue;
            }
            command = command.substr(0, command.find_last_not_of(" \t\r\n") + 1);

            // Split the command by whitespace
            std::vector<ArsTimoris::Commands::Parameter> tokens = std::vector<ArsTimoris::Commands::Parameter>();
            std::istringstream tokenStream =  std::istringstream(command);
            ArsTimoris::Commands::Command::Tokenize(tokenStream, tokens);

            if (tokens.empty()) {
                continue;
            }

            // Process the initializer
            std::string initializer = std::get<std::string>(tokens[0]);
            tokens.erase(tokens.begin());

            commandHandler.ExecuteCommand(initializer, tokens);
            std::cout << std::flush;
        }
    });

    bool messageOpen = false;

    std::function<void(void)> HandleMessageOverlayBounce = [&](void) {
        gameState.messageStack.erase(gameState.messageStack.begin());
        if (gameState.messageStack.size() <= 0) {
            messageOverlay->enabled = false;
            if (menuToChange) {
                menuChangedCallback(gameState);
                menuToChange = false;
            }
        } else {
            messageText->SetText(gameState, gameState.messageStack[0]);
        }
    };

    while (gameState.running) {
        // Timekeeping stuff, perhaps put this in a struct called TimeData for easier usage...?
        timeData.totalTime_s += (timeData.deltaTime_s = (0.001f * (float)(timeData.deltaTime_ms = (SDL_GetTicks() - timeData.lastTime_ms))));
        timeData.lastTime_ms = SDL_GetTicks();

        gameState.CacheInputState();

        if (!messageOverlay->enabled) {
            if (gameState.messageStack.size() > 0) {
                messageOverlay->enabled = true;
                messageText->SetText(gameState, gameState.messageStack[0]);
            } else if (menuToChange) {
                menuChangedCallback(gameState);
                menuToChange = false;
            }
        }

        while (SDL_PollEvent(&event)) {
            gameState.inputData.mouse = SDL_GetMouseState(&gameState.inputData.mousePos.x, &gameState.inputData.mousePos.y);
            switch (event.type) {
                case SDL_EVENT_QUIT: {
                    gameState.running = false;
                    break;
                }
                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    switch (event.button.button) {
                        case SDL_BUTTON_LEFT: {
                            if (messageOverlay->enabled) {
                                HandleMessageOverlayBounce();
                            } else if (!gameState.uiManager.OnMouseLeftDown(gameState, &gameState.inputData.mousePos)) {
                                switch (gameState.menu) {
                                    case Menu::COMBAT: {
                                        for (const NPCDisplay& npc : combatNPCs) {
                                            room = &gameState.rooms[gameState.curRoom];
                                            if (room->inhabitants[npc.index].curHP > 0 && SDL_PointInRectFloat(&gameState.inputData.mousePos, &npc.area)) {
                                                if (gameState.player.actions[choice].condition == nullptr || gameState.player.actions[choice].condition(gameState, &gameState.player, &room->inhabitants[npc.index])) {
                                                    gameState.player.actions[choice].action(gameState, &gameState.player, &room->inhabitants[npc.index]);
                                                }
                                                break;
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        case SDL_BUTTON_MIDDLE: {
                            if (!gameState.uiManager.OnMouseMiddleDown(gameState, &gameState.inputData.mousePos)) {
                                // Currently unused.
                            }
                            break;
                        }
                        case SDL_BUTTON_RIGHT: {
                            if (!gameState.uiManager.OnMouseRightDown(gameState, &gameState.inputData.mousePos)) {
                                // Currently unused.
                            }
                            break;
                        }
                    }
                    break;
                }
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    switch (event.button.button) {
                        case SDL_BUTTON_LEFT:
                            if (!gameState.uiManager.OnMouseLeftUp(gameState, &gameState.inputData.mousePos)) {
                                // Currently unused.
                            }
                            break;
                        case SDL_BUTTON_MIDDLE:
                            if (!gameState.uiManager.OnMouseMiddleUp(gameState, &gameState.inputData.mousePos)) {
                                // Currently unused.
                            }
                            break;
                        case SDL_BUTTON_RIGHT:
                            if (!gameState.uiManager.OnMouseRightUp(gameState, &gameState.inputData.mousePos)) {
                                // Currently unused.
                            }
                            break;
                    }
                    break;
                }
                case SDL_EVENT_KEY_DOWN: {
                    if (messageOverlay->enabled) {
                        if (event.key.key == SDLK_ESCAPE) {
                            HandleMessageOverlayBounce();
                        }
                        break;
                    }
                    switch (gameState.screen) {
                        case Screen::GAME: {
                            switch (gameState.menu) {
                                case Menu::NONE: {
                                    if (event.key.key == SDLK_I) {
                                        std::string message = std::format("{}\n{}\n", GameData::ROOM_DATA[gameState.rooms[gameState.curRoom].roomID].roomName, GameData::ROOM_DATA[gameState.rooms[gameState.curRoom].roomID].roomDescription);
                                        if (gameState.rooms[gameState.curRoom].roomActions.size() > 0) {
                                            for (const RoomAction& roomAction : gameState.rooms[gameState.curRoom].roomActions) {
                                                if (roomAction.roomDescription != nullptr) {
                                                    message += roomAction.roomDescription(gameState);
                                                }
                                            }
                                        } else {
                                            message += "The room is devoid of interactable objects.";
                                        }
                                        gameState.AddMessage(message);
                                    }
                                    break;
                                }
                                case Menu::MOVING: {
                                    switch (event.key.key) {
                                        case SDLK_ESCAPE:
                                            roomMoveMenu->enabled = false;
                                            roomGeneralMenu->enabled = true; 
                                            gameState.menu = Menu::NONE;
                                            break;
                                        case SDLK_A:
                                            if (--choice < 0) {
                                                choice = (int32_t)gameState.rooms[gameState.curRoom].connections.size() - 1;
                                            }
                                            break;
                                        case SDLK_D:
                                            if (++choice >= gameState.rooms[gameState.curRoom].connections.size()) {
                                                choice = 0;
                                            }
                                            break;
                                    }
                                    break;
                                }
                                case Menu::ROOM_ACTIONS: {
                                    if (event.key.key == SDLK_ESCAPE) {
                                        roomActionsMenu->enabled = false;
                                        roomGeneralMenu->enabled = true; 
                                        gameState.menu = Menu::NONE;
                                    } else if (event.key.key == SDLK_A) {
                                        if (--choice < 0) {
                                            choice = (int32_t)gameState.rooms[gameState.curRoom].roomActions.size() - 1;
                                        }
                                    } else if (event.key.key == SDLK_D) {
                                        if (++choice >= gameState.rooms[gameState.curRoom].roomActions.size()) {
                                            choice = 0;
                                        }
                                    }
                                    break;
                                }
                                case Menu::COMBAT: {
                                    if (event.key.key == SDLK_S) {
                                        if (--choice < 0) {
                                            choice = (int32_t)gameState.player.actions.size() - 1;
                                        }
                                    } else if (event.key.key == SDLK_W) {
                                        if (++choice >= gameState.player.actions.size()) {
                                            choice = 0;
                                        }
                                    } else if (event.key.key == SDLK_SPACE) {
                                        room = &gameState.rooms[gameState.curRoom];
                                        if (gameState.player.actions[choice].condition == nullptr || gameState.player.actions[choice].condition(gameState, &gameState.player, &gameState.player)) {
                                            gameState.player.actions[choice].action(gameState, &gameState.player, &gameState.player);
                                        }
                                    }
                                    break;
                                }
                                case Menu::LEVEL_UP: {
                                    if (event.key.key == SDLK_ESCAPE) {
                                        levelMenu->enabled = false;
                                        statsMenu->enabled = true;
                                        gameState.menu = Menu::STATS;
                                    } else if (event.key.key == SDLK_A) {
                                        if (--classChoice < 0) {
                                            classChoice = (int32_t)classNames.size() - 1;
                                        }
                                        UpdateLevelUp();
                                    } else if (event.key.key == SDLK_D) {
                                        if (++classChoice >= classNames.size()) {
                                            classChoice = 0;
                                        }
                                        UpdateLevelUp();
                                    }
                                    break;
                                }
                                case Menu::INVENTORY: {
                                    if (event.key.key == SDLK_ESCAPE) {
                                        inventoryMenu->enabled = false;
                                        gameState.menu = Menu::NONE;
                                    } else if (event.key.key == SDLK_A) {
                                        if (--currentInventoryPage < 0) {
                                            currentInventoryPage = 0;
                                        }
                                        UpdateInventoryMenu();
                                    } else if (event.key.key == SDLK_D) {
                                        ++currentInventoryPage;
                                        UpdateInventoryMenu();
                                    }
                                    break;
                                }
                                case Menu::STATS: {
                                    if (event.key.key == SDLK_ESCAPE) {
                                        statsMenu->enabled = false;
                                        gameState.menu = Menu::NONE;
                                    }
                                    break;
                                }
                                case Menu::SHOP: {
                                    if (event.key.key == SDLK_ESCAPE) {
                                        shopMenu->enabled = false;
                                        gameState.menu = Menu::NONE;
                                    } else if (event.key.key == SDLK_A) {
                                        if (--shopChoice < 0) {
                                            shopChoice = 0;
                                        }
                                        UpdateShopMenu();
                                    } else if (event.key.key == SDLK_D) {
                                        if (++shopChoice >= (int32_t)ceilf((float)shop->catalog.size() / 6.0f)) {
                                            --shopChoice;
                                        }
                                        UpdateShopMenu();
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
        }

        gameState.uiManager.Process(gameState);

        /*
        for (size_t i = 0; i < 10; ++i) {
            gameState.assets.particles.at("Smoke")->SpawnParticle(ArsTimoris::Visual::Particle::ParticleData{
                {200.0f, 200.0f}, 
                {
                    PERCENT_DISTRIBUTION(gameState.generator) * 25.0f - 12.5f,
                    PERCENT_DISTRIBUTION(gameState.generator) * 25.0f - 12.5f
                },
                PERCENT_DISTRIBUTION(gameState.generator) * 180.0f - 90.0f,
                PERCENT_DISTRIBUTION(gameState.generator) * 40.0f - 20.0f,
                1.0f + PERCENT_DISTRIBUTION(gameState.generator) * 3.0f,
                2.5f
            });
        }
        */

        switch (gameState.screen) {
            case Screen::GAME: {
                switch (gameState.menu) {
                    case Menu::NONE: {
                        if (messageOverlay->enabled) {
                            break;
                        }

                        if (!gameState.rooms[gameState.curRoom].initialized) {
                            gameState.InitializeRoom(&gameState.rooms[gameState.curRoom]);
                            encounter = gameState.GetRandomEncounter();
                            if (GameData::ENCOUNTERS[encounter].event != "{}") {
                                runEncounter = true;
                            }

                            #pragma region Room Description
                            std::string message = std::format("{}\n{}\n", GameData::ROOM_DATA[gameState.rooms[gameState.curRoom].roomID].roomName, GameData::ROOM_DATA[gameState.rooms[gameState.curRoom].roomID].roomDescription);
                            if (gameState.rooms[gameState.curRoom].roomActions.size() > 0) {
                                for (const RoomAction& roomAction : gameState.rooms[gameState.curRoom].roomActions) {
                                    if (roomAction.roomDescription != nullptr) {
                                        message += roomAction.roomDescription(gameState);
                                    }
                                }
                            } else {
                                message += "The room is devoid of interactable objects.";
                            }
                            gameState.AddMessage(message);
                            #pragma endregion
                        }
                    
                        room = &gameState.rooms[gameState.curRoom];
                        
                        if (runEncounter) {
                            gameState.interpreter.ParseStatement<int32_t>(gameState, RegisterType::ERROR_TYPE, GameData::ENCOUNTERS[encounter].event);
                            runEncounter = false;
                        }

                        room = &gameState.rooms[gameState.curRoom];
                        if (room->inhabitants.size() > 0 && !gameState.debug.noFights) {
                            choice = 0;
                            menuToChange = true;
                            menuChangedCallback = [&](GameState& a_gameState) {
                                roomGeneralMenu->enabled = false;
                                combatMenu->enabled = true;
                                combatMenuDecoration->enabled = true;
                                choice = 0;
                                a_gameState.menu = Menu::COMBAT;
                            };
                            float xi = 20.0f;
                            float yi = 30.0f;
                            for (size_t i = 0; i < room->inhabitants.size(); ++i) {
                                std::shared_ptr<ArsTimoris::Assets::NPCRendererAsset> renderer = gameState.assets.npcRenderers.at(room->inhabitants[i].texture);
                                combatNPCs.push_back(NPCDisplay(gameState,
                                    SDL_FRect{xi + room->inhabitants[i].x, yi + room->inhabitants[i].y, (float)renderer->width * 2, (float)renderer->height * 2}, 
                                    room->inhabitants[i].armor,
                                    renderer, i
                                ));
                            }
                            break;
                        }

                        UpdateTopBar();
                        break;
                    }
                    case Menu::MOVING: {
                        room = &gameState.rooms[gameState.curRoom];
                        labelText->SetText(gameState, std::format("{}/{}) [{}] \\[FCH:{}]{}", choice + 1, room->connections.size(), room->connections[choice].destination, (gameState.rooms[room->connections[choice].destination].initialized ? "FF,00,00" : "00,FF,00"), GameData::ROOM_DATA[gameState.rooms[room->connections[choice].destination].roomID].roomName));
                        break;
                    }
                    case Menu::ROOM_ACTIONS: {
                        room = &gameState.rooms[gameState.curRoom];
                        if (!room->roomActions.empty()) {
                            roomActionText->SetText(gameState, std::format("{}/{}) {}", choice + 1, room->roomActions.size(), room->roomActions[choice].name));
                            roomActionDescriptionText->SetText(gameState,
                                room->roomActions[choice].roomDescription != nullptr ? 
                                    std::format("{}\n\n{}", room->roomActions[choice].description, room->roomActions[choice].roomDescription(gameState)) : 
                                    room->roomActions[choice].description
                            );
                        } else {
                            roomActionText->SetText(gameState, "None");
                            roomActionDescriptionText->SetText(gameState, "None");
                        }
                        break;
                    }
                    case Menu::LEVEL_UP: {
                        hpText->SetText(gameState, std::format("HP: {:>3}/{:<3}", gameState.player.curHP, gameState.player.maxHP));
                        mpText->SetText(gameState, std::format("MP: {:>3}/{:<3}", gameState.player.curMana, gameState.player.maxMana));
                        acText->SetText(gameState, std::format("AC: {}", gameState.player.GetEffectiveArmor()));
                        xpText->SetText(gameState, std::format("XP: {}", gameState.player.xp));
                        gpText->SetText(gameState, std::format("GP: {}", gameState.player.gold));
                        break;
                    }
                    case Menu::INVENTORY: {
                        hpText->SetText(gameState, std::format("HP: {:>3}/{:<3}", gameState.player.curHP, gameState.player.maxHP));
                        mpText->SetText(gameState, std::format("MP: {:>3}/{:<3}", gameState.player.curMana, gameState.player.maxMana));
                        acText->SetText(gameState, std::format("AC: {}", gameState.player.GetEffectiveArmor()));
                        xpText->SetText(gameState, std::format("XP: {}", gameState.player.xp));
                        gpText->SetText(gameState, std::format("GP: {}", gameState.player.gold));
                        break;
                    }
                    case Menu::COMBAT: {
                        room = &gameState.rooms[gameState.curRoom];
                        
                        if (messageOverlay->enabled) {
                            break;
                        }

                        if (room->inhabitants.size() > 0) {
                            topActionText->SetText(gameState, std::format("{}", gameState.player.actions[(choice - 1 < 0) ? (gameState.player.actions.size() - 1) : (choice - 1)].name));
                            middleActionText->SetText(gameState, std::format("{}", gameState.player.actions[choice].name));
                            bottomActionText->SetText(gameState, std::format("{}", gameState.player.actions[(choice + 1 >= gameState.player.actions.size()) ? 0 : (choice + 1)].name));
                            armorIconLabelText->SetText(gameState, std::format("{}", gameState.player.GetEffectiveArmor()));
                        
                            if (gameState.player.usedTurns >= gameState.player.turns) {
                                gameState.player.usedTurns = 0;

                                for (size_t npcIndex = 0; npcIndex < room->inhabitants.size(); ++npcIndex) {
                                    if (room->inhabitants[npcIndex].stunned) {
                                        room->inhabitants[npcIndex].stunned = false;
                                        gameState.AddMessage(std::format("The {} is stunned.", room->inhabitants[npcIndex].name));
                                    } else if (room->inhabitants[npcIndex].curHP <= 0) {
                                        gameState.AddMessage(std::format("The {} is dead.", room->inhabitants[npcIndex].name));
                                    } else if (room->inhabitants[npcIndex].aiFunction != nullptr) {
                                        (*room->inhabitants[npcIndex].aiFunction)(gameState, room->inhabitants[npcIndex]);
                                    }
                                }

                                room = &gameState.rooms[gameState.curRoom];

                                std::vector<size_t> remove = std::vector<size_t>();

                                for (int32_t j = (int32_t)room->inhabitants.size() - 1; j > -1; --j) {
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
                                    combatNPCs.erase(combatNPCs.begin() + toRemove);
                                    room = &gameState.rooms[gameState.curRoom];
                                }

                                for (size_t i = 0; i < combatNPCs.size(); ++i) {
                                    combatNPCs[i].index = i;
                                }

                                if (gameState.player.curHP <= 0) {
                                    gameState.AddMessage("\nYou collapse to the floor, dead.");
                                    gameState.screen = Screen::GAME_OVER;
                                    gameState.menu = Menu::NONE;
                                    combatMenu->enabled = false;
                                    combatMenuDecoration->enabled = false;
                                } else {
                                    gameState.player.TurnEnd(gameState);
                                }
                            } else {
                                bool ok = false;
                                for (int32_t j = (int32_t)room->inhabitants.size() - 1; j > -1; --j) {
                                    if (room->inhabitants[j].curHP > 0) {
                                        ok = true;
                                        break;
                                    }
                                }
                                if (!ok) {
                                    gameState.player.usedTurns = gameState.player.turns;
                                }
                            }
                        } else {
                            menuToChange = true;
                            menuChangedCallback = [&roomGeneralMenu, &combatMenu, &combatMenuDecoration](GameState& a_gameState) {
                                roomGeneralMenu->enabled = true;
                                combatMenu->enabled = false;
                                combatMenuDecoration->enabled = false;
                                a_gameState.menu = Menu::NONE;
                            };
                        }
                        break;
                    }
                    case Menu::SHOP: {
                        if (gameState.justSwapped) {
                            shopMenu->enabled = true;
                            gameState.justSwapped = false;
                            UpdateShopMenu();
                        } else {
                            gpText->SetText(gameState, std::format("GP: {}", gameState.player.gold));
                        }
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
            case Screen::GAME: {
                switch (gameState.menu) {
                    case Menu::NONE: {
                        const SDL_FRect ROOM_BACKGROUND_RECT = {0, 140, 1200, 600};
                        SDL_RenderTexture(gameState.renderer, gameState.assets.textures.at(GameData::ROOM_DATA[gameState.rooms[gameState.curRoom].roomID].roomName)->texture, NULL, &ROOM_BACKGROUND_RECT);
                        break;
                    }
                    case Menu::COMBAT: {
                        const SDL_FRect TURN_BAR_BACK = {0, 598, 24, 104};
                        const SDL_FRect HP_BAR_BACK = {26, 598, 24, 104};
                        const SDL_FRect MP_BAR_BACK = {52, 598, 24, 104};

                        SDL_SetRenderDrawColor(gameState.renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
                        SDL_RenderFillRect(gameState.renderer, &TURN_BAR_BACK);
                        SDL_RenderFillRect(gameState.renderer, &HP_BAR_BACK);
                        SDL_RenderFillRect(gameState.renderer, &MP_BAR_BACK);

                        SDL_FRect turnBarNotch = {2, 600, 20, (102.0f - (2.0f * gameState.player.turns)) / gameState.player.turns};
                        SDL_SetRenderDrawColor(gameState.renderer, 55, 55, 55, SDL_ALPHA_OPAQUE);
                        for (size_t i = 0; i < gameState.player.usedTurns; ++i) {
                            SDL_RenderFillRect(gameState.renderer, &turnBarNotch);
                            turnBarNotch.y += turnBarNotch.h + 2;
                        }
                        SDL_SetRenderDrawColor(gameState.renderer, 75, 155, 75, SDL_ALPHA_OPAQUE);
                        for (size_t i = 0; i < gameState.player.turns - gameState.player.usedTurns; ++i) {
                            SDL_RenderFillRect(gameState.renderer, &turnBarNotch);
                            turnBarNotch.y += turnBarNotch.h + 2;
                        }

                        SDL_SetRenderDrawColor(gameState.renderer, 205, 75, 75, SDL_ALPHA_OPAQUE);
                        float hpPerc = (float)gameState.player.curHP / (float)gameState.player.maxHP;
                        SDL_FRect fillHP = {28, 700.0f - 100.0f * hpPerc, 20, 100.0f * hpPerc};
                        SDL_RenderFillRect(gameState.renderer, &fillHP);
                        
                        SDL_SetRenderDrawColor(gameState.renderer, 75, 75, 205, SDL_ALPHA_OPAQUE);
                        float mpPerc = (float)gameState.player.curMana / (float)gameState.player.maxMana;
                        SDL_FRect fillMP = {54, 700.0f - 100.0f * mpPerc, 20, 100.0f * mpPerc};
                        SDL_RenderFillRect(gameState.renderer, &fillMP);

                        for (int32_t i = (int32_t)combatNPCs.size() - 1; i >= 0; --i) {
                            combatNPCs[i].renderer->Render(gameState.renderer, combatNPCs[i].area, &timeData);

                            if (gameState.debug.drawHitboxes) {
                                SDL_SetRenderDrawColor(gameState.renderer, 205, 125, 125, 200);
                                SDL_RenderFillRect(gameState.renderer, &combatNPCs[i].area);
                            }

                            if (
                                room->inhabitants[combatNPCs[i].index].GetSkillModifier("Unknowability") < 1 &&
                                gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT))
                            ) {
                                SDL_RenderTexture(gameState.renderer, combatNPCs[i].armorTexture, NULL, &combatNPCs[i].armorArea);
                            }

                            if (
                                room->inhabitants[combatNPCs[i].index].GetSkillModifier("Unknowability") < 1 &&
                                (
                                    gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT)) ||
                                    gameState.player.GetSkillModifier("Brawler") >= 6 + room->inhabitants[combatNPCs[i].index].GetSkillModifier("Brawler")
                                )
                            ) {
                                SDL_SetRenderDrawColor(gameState.renderer, 25, 25, 25, 175);
                                SDL_FRect npcHPBarBack = {combatNPCs[i].area.x + 3, combatNPCs[i].area.y + combatNPCs[i].area.h - 19, (combatNPCs[i].area.w - 6), 8};
                                SDL_RenderFillRect(gameState.renderer, &npcHPBarBack);
                                SDL_SetRenderDrawColor(gameState.renderer, 205, 25, 25, 157);
                                SDL_FRect npcHPBarFill = {combatNPCs[i].area.x + 3, combatNPCs[i].area.y + combatNPCs[i].area.h - 19, (combatNPCs[i].area.w - 6) * ((float)room->inhabitants[combatNPCs[i].index].curHP / (float)room->inhabitants[combatNPCs[i].index].maxHP), 8};
                                SDL_RenderFillRect(gameState.renderer, &npcHPBarFill);
                            }

                            if (
                                room->inhabitants[combatNPCs[i].index].GetSkillModifier("Unknowability") < 1 &&
                                (
                                    gameState.player.perks.test(static_cast<size_t>(Perks::INSIGHT)) ||
                                    gameState.player.perks.test(static_cast<size_t>(Perks::ARCANE_EYES))
                                )
                            ) {
                                SDL_SetRenderDrawColor(gameState.renderer, 25, 25, 25, 175);
                                SDL_FRect npcMPBarBack = {combatNPCs[i].area.x + 3, combatNPCs[i].area.y + combatNPCs[i].area.h - 11, (combatNPCs[i].area.w - 6), 8};
                                SDL_RenderFillRect(gameState.renderer, &npcMPBarBack);
                                SDL_SetRenderDrawColor(gameState.renderer, 25, 25, 205, 157);
                                SDL_FRect npcMPBarFill = {combatNPCs[i].area.x + 3, combatNPCs[i].area.y + combatNPCs[i].area.h - 11, (combatNPCs[i].area.w - 6) * ((float)room->inhabitants[combatNPCs[i].index].curMana / (float)room->inhabitants[combatNPCs[i].index].maxMana), 8};
                                SDL_RenderFillRect(gameState.renderer, &npcMPBarFill);
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }

        for (std::pair<const std::string, std::shared_ptr<ArsTimoris::Assets::ParticleAsset>>& particlePair : gameState.assets.particles) {
            particlePair.second->Render(gameState.renderer, &timeData);
        }

        gameState.uiManager.Render(gameState);

        SDL_RenderPresent(gameState.renderer);
        SDL_Delay(8);
    }

    //for (int i = 0; i < 100; ++i) {
    //    std::cin.putback('\n');
    //    std::cout << "Pushed a new line." << std::endl;
    //}
    //
    //std::cout << "Should join." << std::endl;
    //
    //inputThread.join();

    // I give up, just let the OS clean it up.

    DebugLogging::CloseParserLog();

    return 0;
}