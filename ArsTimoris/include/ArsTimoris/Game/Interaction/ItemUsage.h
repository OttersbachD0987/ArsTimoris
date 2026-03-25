#pragma once
#ifndef ARS_TIMORIS__GAME__INTERACTION__ITEM_USAGE_H
#define ARS_TIMORIS__GAME__INTERACTION__ITEM_USAGE_H
#include <functional>
#include <string>

struct GameState;
struct ItemStack;

/// @struct ItemUsage
/// @brief A holder class for an item's usage.
struct ItemUsage {
public:
    std::string name = "";
    std::function<bool(GameState&, const ItemStack&, size_t)> condition = nullptr;
    std::function<void(GameState&, const ItemStack&, size_t)> usage = nullptr;
};
#endif