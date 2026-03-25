#pragma once
#ifndef ARS_TIMORIS__GAME__SHOP__SHOP_DATA_H
#define ARS_TIMORIS__GAME__SHOP__SHOP_DATA_H
#include <string>

struct ShopData;

#include <ArsTimoris/Game/Shop/ShopGenerationRule.h>

struct ShopData {
public:
    std::string shopName;
    std::vector<ShopGenerationRule> generationRules;
};

void GenerateShopInstance(GameState& a_gameState, const ShopData& a_shopData, ShopInstance& a_shopInstance);
#endif