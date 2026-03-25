#pragma once
#ifndef ARS_TIMORIS__GAME__ENTITY__PLAYER_DATA_H
#define ARS_TIMORIS__GAME__ENTITY__PLAYER_DATA_H
#include <string>
#include <bitset>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <optional>

enum class Perks {
    INSIGHT,
    ARCANE_EYES,
    HARD_HITTER,
    HORDE_SLAYER,
    SIZE
};

std::string PerkDisplayName(Perks a_perk);

Perks PerkFromName(const std::string& a_perk);

struct PlayerData;

#include <ArsTimoris/Game/Entity/EntityData.h>
#include <ArsTimoris/Game/Inventory/ItemStack.h>
#include <ArsTimoris/Game/Classes/ClassInstance.h>

struct PlayerData : public EntityData {
public:
    std::unordered_map<std::string, ClassInstance> classes;
    std::bitset<static_cast<size_t>(Perks::SIZE)> perks;
    std::vector<ItemStack> items;
    std::vector<bool> equipped;
    int32_t turns = 1;
    int32_t usedTurns = 0;
    int32_t gold = 0;
    int32_t xp = 0;

    PlayerData(void);
    PlayerData(const EntityTemplate& a_entityTemplate);
    PlayerData(const EntityTemplate& a_entityTemplate, const std::vector<size_t>& a_perks, size_t a_turns);

    #pragma region Inventory
    void UseItemAt(size_t a_index);
    void UseItemOf(size_t a_itemID);

    void AddItem(size_t a_itemID, uint32_t a_itemAmount);

    bool HasItem(size_t a_itemID, uint32_t a_itemAmount);
    std::optional<size_t> HasMatchingEquippedItem(GameState& a_gameState, const std::function<bool(GameState&, const ItemStack&)>& a_predicate);
    #pragma endregion

    int32_t LevelInClass(const std::string& a_class);

    int32_t GetEffectiveArmor(void);

    void Hurt(int32_t a_damage);
    void Heal(int32_t a_amount);

    void DrainMana(int32_t a_amount);
    void RegainMana(int32_t a_amount);
};
#endif