#pragma once
#ifndef ARS_TIMORIS__GAME__GAME_DATA_H
#define ARS_TIMORIS__GAME__GAME_DATA_H
#include <functional>

struct GameData;

#include <ArsTimoris/Game/GameState.h>
#include <ArsTimoris/Util/Input.hpp>
#include <ArsTimoris/Game/Inventory/ItemData.h>
#include <ArsTimoris/Game/Entity/EntityTemplate.h>
#include <ArsTimoris/Game/Entity/NPCTemplate.h>
#include <ArsTimoris/Game/Map/Encounter.h>
#include <ArsTimoris/Game/Classes/ClassData.h>
#include <ArsTimoris/Game/Shop/ShopData.h>
#include <ArsTimoris/Game/Shop/ShopCatalog.h>
#include <ArsTimoris/Game/Shop/ShopInstance.h>
#include <ArsTimoris/Game/Interaction/Action.h>
#include <ArsTimoris/Game/Interaction/RoomAction.h>
#include <ArsTimoris/Game/Map/RoomData.h>

struct GameData {
public:
    static std::unordered_map<std::string, Action> STANDARD_ACTIONS;
    static std::unordered_map<std::string, ClassData> CLASSES;
    static std::unordered_map<std::string, TimedEffect> STANDARD_TIMED_EFFECTS;
    static std::vector<RoomData> ROOM_DATA;
    static std::vector<ItemData> ITEM_DATA;
    static std::unordered_map<std::string, ShopGenerationRule> SHARED_GENERATION_RULES;
    static std::unordered_map<std::string, ShopData> SHOP_DATA;
    static std::unordered_map<std::string, RoomAction> ROOM_ACTIONS;
    static std::unordered_map<std::string, EntityTemplate> ENTITY_TEMPLATES;
    static std::unordered_map<std::string, NPCTemplate> NPC_TEMPLATES;
    static std::unordered_map<std::string, std::function<NPCData(NPCData)>> NPC_MODIFIERS;
    static std::vector<Encounter> ENCOUNTERS;
};

#pragma region Stats
/*
Martial Combat     - 
Melee Combat       - Multiple Opponents
Unarmed Combat     - No weapons
Brawling           - 
Swordsmanship      - Skill with Sword
Dueling            - Fighting one opponent
Persuasion         - Convincing people and things through words
Knowledge of Death - Knowledge of death and the dead
Necromancy         - Knowledge in the arcane branch of necromancy (death magic)
Geomancy           - Knowledge in the arcane branch of geomancy (earth magic)
*/
#pragma endregion

#endif