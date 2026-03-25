#pragma once
#ifndef ARS_TIMORIS__GAME__START_DATA_H
#define ARS_TIMORIS__GAME__START_DATA_H
#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>

#include <ArsTimoris/Game/Entity/EntityTemplate.h>
#include <ArsTimoris/Game/Inventory/ItemStack.h>
#include <ArsTimoris/Game/Classes/ClassInstance.h>

struct StartData {
public:
    std::string name;
    std::string description;
    EntityTemplate playerEntity;
    std::unordered_map<std::string, ClassInstance> classes;
    std::vector<size_t> perks;
    std::vector<ItemStack> inventory;
    size_t turns;
    size_t startRoom;
    size_t xp;
    size_t gold;

    /// @brief Brief
    StartData(void);

    /// @brief Brief
    /// @param a_name a
    /// @param a_description b
    /// @param a_playerEntity c
    /// @param a_classes d
    /// @param a_perks e
    /// @param a_inventory f
    /// @param a_turns g
    /// @param a_startRoom h
    /// @param a_xp
    /// @param a_gold
    StartData(std::string_view a_name, std::string_view a_description, const EntityTemplate& a_playerEntity, const std::unordered_map<std::string, ClassInstance>& a_classes, const std::vector<size_t>& a_perks, const std::vector<ItemStack>& a_inventory, size_t a_turns, size_t a_startRoom, size_t a_xp, size_t a_gold);
};


std::vector<StartData> LoadStartData(std::filesystem::path a_path);

#endif