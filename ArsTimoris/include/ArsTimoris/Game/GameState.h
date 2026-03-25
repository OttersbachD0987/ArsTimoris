#pragma once
#ifndef ARS_TIMORIS__GAME__GAME_STATE_H
#define ARS_TIMORIS__GAME__GAME_STATE_H
#include <vector>
#include <random>

#pragma region Screen and Menu
enum class Screen {
    TITLE,
    GAME_SELECT,
    GAME,
    GAME_OVER,
};

enum class Menu {
    PAUSE,
    MOVING,
    ROOM_ACTIONS,
    COMBAT,
    STATS,
    INVENTORY,
    LEVEL_UP,
    NONE
};
#pragma endregion

/// @struct DebugData
/// @brief A struct containing all the debug data for the player.
struct DebugData {
public:
    bool enabled = false;  //< If the debug mode is enabled.
    bool noFights = false; //< The No Fights debug flag to avoid starting the combat menu.
};

struct GameState;

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <ArsTimoris/Game/StartData.h>
#include <ArsTimoris/Game/Entity/PlayerData.h>
#include <ArsTimoris/Game/Map/RoomInstance.h>
#include <ArsTimoris/Sockscript/Interpreter.h>
#include <ArsTimoris/Assets/Assets.h>
#include <ArsTimoris/UI/UIManager.h>
#include <ArsTimoris/Game/InputData.h>

struct GameState {
public:
    static std::vector<StartData> Starts;

    DebugData debug = DebugData();

    bool running = true;
    Screen screen;
    Menu menu;

    size_t usedRooms;
    size_t curRoom;
    size_t start;
    std::vector<RoomInstance> rooms;
    PlayerData player;

    std::mt19937 generator;
    Interpreter interpreter;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AudioDeviceID audioDevice;
    ArsTimoris::Assets::Assets assets;
    ArsTimoris::UI::UIManager uiManager;
    InputData inputData;

    GameState(std::mt19937 a_generator, const Interpreter& a_interpreter);

    void CacheInputState(void);

    static void DisplayStarts(void);

    void Start(size_t a_start);

    void Start(std::string a_startName);

    size_t PushBackRoom(size_t a_index);

    int32_t RollDice(size_t a_amount, size_t a_die);

    size_t GetRandomRoom(const RoomData& a_roomData);

    size_t GetRandomEncounter(void);

    void InitializeRoom(RoomInstance* const a_roomInstance);
};

#include <ArsTimoris/Game/GameData.h>
#endif