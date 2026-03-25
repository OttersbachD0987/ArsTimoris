#pragma once
#ifndef ARS_TIMORIS__GAME__SHOP__SHOP_CATALOG_H
#define ARS_TIMORIS__GAME__SHOP__SHOP_CATALOG_H
struct ShopCatalog;

#include <ArsTimoris/Game/Inventory/ItemStack.h>

struct ShopCatalog {
public:
    size_t cost;
    ItemStack stack;
};
#endif