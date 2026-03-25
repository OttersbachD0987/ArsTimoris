#pragma once
#ifndef ARS_TIMORIS__GAME__SHOP__SHOP_GENERATION_RULE_H
#define ARS_TIMORIS__GAME__SHOP__SHOP_GENERATION_RULE_H
#include <functional>

struct GameState;
struct ShopInstance;

struct ShopGenerationRule {
public:
    std::function<bool(GameState&)> valid = nullptr;
    std::function<void(GameState&, ShopInstance&)> generate = nullptr;
};
#endif