#include <ArsTimoris/UI/UIImageComponent.h>
#include <ArsTimoris/UI/UIElement.h>
#include <ArsTimoris/Game/GameState.h>

namespace ArsTimoris::UI { 
    UIImageComponent::UIImageComponent(std::string_view a_texture, bool a_nineSliced) {
        texture = std::string(a_texture);
        nineSliced = a_nineSliced;
    }

    void UIImageComponent::Hookup(GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
        if (nineSliced) {
            a_element->onRender.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
                SDL_RenderTexture9Grid(a_gameState.renderer, a_gameState.assets.textures.at(this->texture)->texture, NULL, 6, 6, 6, 6, 2, &a_element->displayArea.rect);
            });
        } else {
            a_element->onRender.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
                SDL_RenderTexture(a_gameState.renderer, a_gameState.assets.textures.at(this->texture)->texture, NULL, &a_element->displayArea.rect);
            });
        }
    }
}