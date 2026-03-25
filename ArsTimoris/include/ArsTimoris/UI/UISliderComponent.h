#pragma once
#ifndef UI_SLIDER_COMPONENT_H
#define UI_SLIDER_COMPONENT_H
#include <ArsTimoris/UI/UIElementComponent.h>
#include <ArsTimoris/UI/UIRect.h>

namespace ArsTimoris::UI {
    /// @brief 
    class UISliderComponent : public UIElementComponent {
    public:
        /// @brief 
        float value;
        float maxValue;
        float windowSize;
        float perWidth;
        float startX;
        float startValue;
        UIRect handleRect;

        /// @brief 
        /// @param a_value 
        /// @param a_maxValue 
        /// @param a_windowSize 
        UISliderComponent(float a_value, float a_maxValue, float a_windowSize);

        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_element 
        void Hookup(GameState& a_gameState, UILayer* a_layer, UIElement* a_element) override;
    };
}
#endif