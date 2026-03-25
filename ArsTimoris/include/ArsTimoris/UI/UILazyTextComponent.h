#pragma once
#ifndef UI_LAZY_TEXT_COMPONENT_H
#define UI_LAZY_TEXT_COMPONENT_H
#include <ArsTimoris/UI/UITextComponent.h>

namespace ArsTimoris::UI {
    /// @brief 
    class UILazyTextComponent : public UITextComponent {
    public:
        /// @brief 
        /// @param a_text 
        /// @param a_font 
        /// @param a_size 
        /// @param a_margins 
        /// @param a_anchor 
        UILazyTextComponent(std::string a_text, std::string a_font, float a_size, int a_margins, UIAnchor a_anchor);

        /// @brief 
        float fontSize;

        /// @brief 
        /// @param  
        /// @param  
        /// @param  
        /// @param  
        /// @param  
        void Hookup(GameState&, UILayer*, UIElement*) override;
        /// @brief 
        /// @param  
        /// @param  
        /// @param  
        /// @param  
        void SetText(GameState&, UIElement*, std::string);
        /// @brief 
        /// @param  
        /// @param  
        /// @param  
        /// @param  
        void SetAnchor(GameState&, UIElement*, UIAnchor);
    };
}
#endif