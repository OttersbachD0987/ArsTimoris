#pragma once
#ifndef UI_ATLAS_TEXT_COMPONENT_H
#define UI_ATLAS_TEXT_COMPONENT_H
#include <ArsTimoris/UI/UIElementComponent.h>
#include <ArsTimoris/UI/UIRect.h>

namespace ArsTimoris::UI {
    /// @brief 
    class UIAtlasTextComponent : public UIElementComponent {
    public:
        /// @brief 
        /// @param a_text 
        /// @param a_font 
        /// @param a_size 
        /// @param a_margins 
        /// @param a_anchor 
        UIAtlasTextComponent(std::string a_text, std::string a_font, float a_size, int a_margins, UIAnchor a_anchor);
        
        /// @brief 
        /// @param a_text 
        /// @param a_font 
        /// @param a_size 
        /// @param a_horizontalMargins 
        /// @param a_verticalMargins 
        /// @param a_anchor 
        UIAtlasTextComponent(std::string a_text, std::string a_font, float a_size, int a_horizontalMargins, int a_verticalMargins, UIAnchor a_anchor);

        /// @brief 
        /// @param a_text 
        /// @param a_font 
        /// @param a_size 
        /// @param a_topMargin 
        /// @param a_rightMargin 
        /// @param a_bottomMargin 
        /// @param a_leftMargin 
        /// @param a_anchor 
        UIAtlasTextComponent(std::string a_text, std::string a_font, float a_size, int a_topMargin, int a_rightMargin, int a_bottomMargin, int a_leftMargin, UIAnchor a_anchor);

        /// @brief 
        std::string fontName = "BitCrusherAtlas";
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
        int topMargin;
        /// @brief 
        int rightMargin;
        /// @brief 
        int bottomMargin;
        /// @brief 
        int leftMargin;
        /// @brief 
        float fontSize;

        /// @brief
        bool dirtyText = false;

        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_element 
        void Hookup(GameState& a_gameState, UILayer* a_layer, UIElement* a_element) override;
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_text 
        void SetText(GameState& a_gameState, UIElement* a_layer, std::string a_text);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_anchor 
        void SetAnchor(GameState& a_gameState, UIElement* a_layer, UIAnchor a_anchor);
    };
}
#endif