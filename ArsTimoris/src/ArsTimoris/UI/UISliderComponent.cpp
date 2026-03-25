#include <ArsTimoris/UI/UISliderComponent.h>
#include <ArsTimoris/UI/UIElement.h>
#include <ArsTimoris/Game/GameState.h>

namespace ArsTimoris::UI { 
    UISliderComponent::UISliderComponent(float a_value, float a_maxValue, float a_windowSize) {
        value = a_value;
        maxValue = a_maxValue;
        windowSize = a_windowSize;
        perWidth = 0;
        startX = 0;
        startValue = 0;
        handleRect = UIRect{{0, 0, 0, 0}};
    }

    void UISliderComponent::Hookup(GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
        a_element->onRender.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
            SDL_Color color;
            SDL_GetRenderDrawColor(a_gameState.renderer, &color.r, &color.g, &color.b, &color.a);
            SDL_SetRenderDrawColor(a_gameState.renderer, 255, 128, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(a_gameState.renderer, &handleRect.rect);
            SDL_SetRenderDrawColor(a_gameState.renderer, color.r, color.g, color.b, color.a);
        });
        a_element->onRecalculate.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
            perWidth = a_element->displayArea.rect.w / maxValue;
            handleRect.SetSize({perWidth * windowSize, a_element->displayArea.rect.h});
            handleRect.SetTopLeft({a_element->displayArea.rect.x + perWidth * value, a_element->displayArea.rect.y});
        });
        a_element->onMouseLeftDown.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element, SDL_FPoint* a_mousePos) {
            if (SDL_PointInRectFloat(a_mousePos, &handleRect.rect)) {
                startX = a_mousePos->x;
                startValue = value;
            }
            return true;
        });
        a_element->onMouseLeftHeld.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element, SDL_FPoint* a_mousePos) {
            if (startX != -1) {
                value = std::clamp<float>(startValue + perWidth * (a_mousePos->x - startX), 0, maxValue - windowSize);
                a_gameState.uiManager.dirtyRecalculate = true;
            }

            return startX != -1;
        });
        a_element->onMouseLeftUp.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element, SDL_FPoint* a_mousePos) {
            startX = -1;
            return true;
        });
    }
}