#pragma once
#ifndef UI_TEXT_COMPONENT_H
#define UI_TEXT_COMPONENT_H
#include <ArsTimoris/UI/UIElementComponent.h>
#include <ArsTimoris/UI/UIRect.h>

namespace ArsTimoris::UI {
    /// @brief 
    class UITextComponent : public UIElementComponent {
    public:
        /// @brief 
        /// @param  
        /// @param  
        /// @param  
        /// @param  
        UITextComponent(std::string a_text, std::string a_font, int a_margins, UIAnchor a_anchor);

        /// @brief 
        std::string fontName = "NotoSansMono-Regular-12";
        /// @brief 
        std::string text;
        /// @brief 
        SDL_Texture* textTexture;
        /// @brief 
        UIAnchor textAnchor;
        /// @brief 
        UIRect textArea;

        /// @brief 
        int w;
        /// @brief 
        int h;
        /// @brief 
        int margins;

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