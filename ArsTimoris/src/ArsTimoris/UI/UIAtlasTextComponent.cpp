#include <ArsTimoris/UI/UIAtlasTextComponent.h>
#include <ArsTimoris/UI/UIElement.h>
#include <ArsTimoris/UI/UIManager.h>
#include <ArsTimoris/Game/GameState.h>
#include <ArsTimoris/UI/Text/FontAtlas.h>
#include <print>

namespace ArsTimoris::UI {
    UIAtlasTextComponent::UIAtlasTextComponent(std::string a_text, std::string a_font, float a_size, int a_margins, UIAnchor a_anchor) {
        text = a_text;
        fontName = a_font;
        fontSize = a_size;
        topMargin = rightMargin = bottomMargin = leftMargin = a_margins;
        textAnchor = a_anchor;
    }
    
    UIAtlasTextComponent::UIAtlasTextComponent(std::string a_text, std::string a_font, float a_size, int a_horizontalMargins, int a_verticalMargins, UIAnchor a_anchor) {
        text = a_text;
        fontName = a_font;
        fontSize = a_size;
        rightMargin = leftMargin = a_horizontalMargins; 
        topMargin = bottomMargin = a_verticalMargins;       
        textAnchor = a_anchor;
    }
    
    UIAtlasTextComponent::UIAtlasTextComponent(std::string a_text, std::string a_font, float a_size, int a_topMargin, int a_rightMargin, int a_bottomMargin, int a_leftMargin, UIAnchor a_anchor) {
        text = a_text;
        fontName = a_font;
        fontSize = a_size;
        topMargin = a_topMargin;
        rightMargin = a_rightMargin;
        bottomMargin = a_bottomMargin;
        leftMargin = a_leftMargin;
        textAnchor = a_anchor;
    }

    void UIAtlasTextComponent::Hookup(GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
        SDL_Surface* surface = a_gameState.assets.fontAtlases.at(fontName)->fontAtlas->RenderWrapped(text, fontSize, {255, 255, 255, SDL_ALPHA_OPAQUE}, 100 + (int)a_element->displayArea.GetWidth() - (leftMargin + rightMargin));
        textTexture = SDL_CreateTextureFromSurface(a_gameState.renderer, surface);
        SDL_DestroySurface(surface);

        a_element->onRecalculate.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
            //std::println("Dirty Text: {}; w: {}; newMarg: {}; w > newMarg: {}", dirtyText, w, a_element->displayArea.GetWidth() - (leftMargin + rightMargin), w > (a_element->displayArea.GetWidth() - (leftMargin + rightMargin)));
            if (dirtyText || w > (a_element->displayArea.GetWidth() - (leftMargin + rightMargin))) {
                SDL_DestroyTexture(textTexture);
                SDL_Surface* surface = a_gameState.assets.fontAtlases.at(fontName)->fontAtlas->RenderWrapped(text, fontSize, {255, 255, 255, SDL_ALPHA_OPAQUE}, (int)a_element->displayArea.GetWidth() - (leftMargin + rightMargin));
                w = (int)(surface->w * fontSize);
                h = (int)(surface->h * fontSize);
                textTexture = SDL_CreateTextureFromSurface(a_gameState.renderer, surface);
                SDL_SetTextureScaleMode(textTexture, SDL_SCALEMODE_NEAREST);
                SDL_DestroySurface(surface);
                dirtyText = false;
            }

            textArea = {{a_element->displayArea.GetX() + leftMargin, a_element->displayArea.GetY() + topMargin, (float)w, (float)h}};
            switch (textAnchor) {
                case UIAnchor::TOP_LEFT:
                    textArea.SetTopLeft(a_element->displayArea.GetTopLeft());
                    textArea.SetX(textArea.GetX() + leftMargin);
                    textArea.SetY(textArea.GetY() + topMargin);
                    break;
                case UIAnchor::TOP_CENTER:
                    textArea.SetTopCenter(a_element->displayArea.GetTopCenter());
                    textArea.SetY(textArea.GetY() + topMargin);
                    break;
                case UIAnchor::TOP_RIGHT:
                    textArea.SetTopRight(a_element->displayArea.GetTopRight());
                    textArea.SetX(textArea.GetX() - rightMargin);
                    textArea.SetY(textArea.GetY() + topMargin);
                    break;
                case UIAnchor::MIDDLE_LEFT:
                    textArea.SetMiddleLeft(a_element->displayArea.GetMiddleLeft());
                    textArea.SetX(textArea.GetX() + leftMargin);
                    break;
                case UIAnchor::MIDDLE_CENTER:
                    textArea.SetMiddleCenter(a_element->displayArea.GetMiddleCenter());
                    break;
                case UIAnchor::MIDDLE_RIGHT:
                    textArea.SetMiddleRight(a_element->displayArea.GetMiddleRight());
                    textArea.SetX(textArea.GetX() - rightMargin);
                    break;
                case UIAnchor::BOTTOM_LEFT:
                    textArea.SetBottomLeft(a_element->displayArea.GetBottomLeft());
                    textArea.SetX(textArea.GetX() + leftMargin);
                    textArea.SetY(textArea.GetY() - bottomMargin);
                    break;
                case UIAnchor::BOTTOM_CENTER:
                    textArea.SetBottomCenter(a_element->displayArea.GetBottomCenter());
                    textArea.SetY(textArea.GetY() - bottomMargin);
                    break;
                case UIAnchor::BOTTOM_RIGHT:
                    textArea.SetBottomRight(a_element->displayArea.GetBottomRight());
                    textArea.SetX(textArea.GetX() - rightMargin);
                    textArea.SetY(textArea.GetY() - bottomMargin);
                    break;
            }
        });

        a_element->onRender.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
            SDL_RenderTexture(a_gameState.renderer, textTexture, NULL, &textArea.rect);
        });
        
        SetAnchor(a_gameState, a_element, textAnchor);
    }

    void UIAtlasTextComponent::SetText(GameState& a_gameState, UIElement* a_element, std::string a_text) {
        dirtyText = true;
        text = a_text;
        a_gameState.assets.uiManager->dirtyRecalculate = true;
    }

    void UIAtlasTextComponent::SetAnchor(GameState& a_gameState, UIElement* a_element, UIAnchor a_anchor) {
        dirtyText = true;
        textAnchor = a_anchor;
        a_gameState.assets.uiManager->dirtyRecalculate = true;
    }
}