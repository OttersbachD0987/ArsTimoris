#include <ArsTimoris/Game/Shop/ShopData.h>
#include <ArsTimoris/Game/GameState.h>

void GenerateShopInstance(GameState& a_gameState, const ShopData& a_shopData, ShopInstance& a_shopInstance) {
    a_shopInstance.shopName = a_shopData.shopName;
    for (const ShopGenerationRule& shopGenerationRule  : a_shopData.generationRules) {
        if (shopGenerationRule.valid(a_gameState)) {
            shopGenerationRule.generate(a_gameState, a_shopInstance);
        }
    }
}