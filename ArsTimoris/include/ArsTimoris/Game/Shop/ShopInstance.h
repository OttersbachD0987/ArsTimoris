#pragma once
#ifndef ARS_TIMORIS__GAME__SHOP__SHOP_INSTANCE_H
#define ARS_TIMORIS__GAME__SHOP__SHOP_INSTANCE_H
struct ShopInstance;

#include <ArsTimoris/Game/Shop/ShopCatalog.h>

struct ShopInstance {
public:
    std::string shopName;
    std::vector<ShopCatalog> catalog;
};
#endif