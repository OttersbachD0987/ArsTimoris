#pragma once
#ifndef ARS_TIMORIS__GAME__INVENTORY__ITEM_DATA_H
#define ARS_TIMORIS__GAME__INVENTORY__ITEM_DATA_H
#include <string>
#include <cstdint>
#include <unordered_map>

enum class BladeType {
    DAGGER,
    SHORT_SWORD,
    LONG_SWORD
};

struct ItemData;

#include <ArsTimoris/Game/Interaction/ItemUsage.h>

struct ItemData {
public:
    std::string name;
    std::string description;
    std::unordered_map<std::string, int32_t> tags;
    ItemUsage usage;
    uint32_t maxStack;

    ItemData(std::string_view a_name, std::string_view a_description, uint32_t a_maxStack);
    ItemData(std::string_view a_name, std::string_view a_description, const ItemUsage& a_usage, uint32_t a_maxStack);
    ItemData(std::string_view a_name, std::string_view a_description, const std::unordered_map<std::string, int32_t>& a_tags, int32_t a_maxStack);
    ItemData(std::string_view a_name, std::string_view a_description, const std::unordered_map<std::string, int32_t>& a_tags, const ItemUsage& a_usage, uint32_t a_maxStack);
};
#endif