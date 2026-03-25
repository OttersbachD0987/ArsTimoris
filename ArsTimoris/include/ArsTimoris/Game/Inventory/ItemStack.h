#pragma once
#ifndef ARS_TIMORIS__GAME__INVENTORY__ITEM_STACK_H
#define ARS_TIMORIS__GAME__INVENTORY__ITEM_STACK_H
#include <cstdint>
#include <unordered_map>
#include <string>
#include <string_view>

enum struct ItemStackDeserializationMode {
    COLLECTING_ID,
    COLLECTING_STACK,
    COLLECTING_METAKEY,
    COLLECTING_METAVALUE,
    FINISHED
};

struct ItemStack {
public:
    size_t itemID;
    uint32_t stackSize;
    std::unordered_map<std::string, int32_t> metadata;

    ItemStack(void);
    ItemStack(size_t a_itemID, uint32_t a_stackSize);
    ItemStack(size_t a_itemID, uint32_t a_stackSize, const std::unordered_map<std::string, int32_t>& a_metadata);

    int32_t GetMetadata(const std::string& a_key, int32_t a_default = 0);

    static ItemStack FromString(std::string_view a_serialized);
};
#endif