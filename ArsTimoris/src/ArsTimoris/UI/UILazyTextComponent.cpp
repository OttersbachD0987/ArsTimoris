#include <ArsTimoris/UI/UILazyTextComponent.h>
#include <ArsTimoris/UI/UIElement.h>
#include <ArsTimoris/UI/UIManager.h>
#include <ArsTimoris/Game/GameState.h>
#include <print>

namespace ArsTimoris::UI {
    UILazyTextComponent::UILazyTextComponent(std::string a_text, std::string a_font, float a_size, int a_margins, UIAnchor a_anchor) : UITextComponent(a_text, a_font, a_margins, a_anchor) {
        fontSize = a_size;
    }

    void UILazyTextComponent::Hookup(GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
        SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(a_gameState.assets.lazyFonts.at(fontName)->GetFontSize(fontSize)->font, this->text.c_str(), this->text.size(), {255, 255, 255, SDL_ALPHA_OPAQUE}, (int)a_element->displayArea.GetWidth() - margins * 2);
        textTexture = SDL_CreateTextureFromSurface(a_gameState.renderer, surface);
        SDL_DestroySurface(surface);

        a_element->onRecalculate.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
            SDL_DestroyTexture(textTexture);
            SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(a_gameState.assets.lazyFonts.at(fontName)->GetFontSize(fontSize)->font, this->text.c_str(), this->text.size(), {255, 255, 255, SDL_ALPHA_OPAQUE}, (int)a_element->displayArea.GetWidth() - margins * 2);
            w = surface->w;
            h = surface->h;
            textTexture = SDL_CreateTextureFromSurface(a_gameState.renderer, surface);
            SDL_DestroySurface(surface);

            textArea = {{a_element->displayArea.GetX() + margins, a_element->displayArea.GetY() + margins, (float)w, (float)h}};
            switch (textAnchor) {
                case UIAnchor::TOP_LEFT:
                    textArea.SetTopLeft(a_element->displayArea.GetTopLeft());
                    textArea.SetX(textArea.GetX() + margins);
                    textArea.SetY(textArea.GetY() + margins);
                    break;
                case UIAnchor::TOP_CENTER:
                    textArea.SetTopCenter(a_element->displayArea.GetTopCenter());
                    textArea.SetY(textArea.GetY() + margins);
                    break;
                case UIAnchor::TOP_RIGHT:
                    textArea.SetTopRight(a_element->displayArea.GetTopRight());
                    textArea.SetX(textArea.GetX() - margins);
                    textArea.SetY(textArea.GetY() + margins);
                    break;
                case UIAnchor::MIDDLE_LEFT:
                    textArea.SetMiddleLeft(a_element->displayArea.GetMiddleLeft());
                    textArea.SetX(textArea.GetX() + margins);
                    break;
                case UIAnchor::MIDDLE_CENTER:
                    textArea.SetMiddleCenter(a_element->displayArea.GetMiddleCenter());
                    break;
                case UIAnchor::MIDDLE_RIGHT:
                    textArea.SetMiddleRight(a_element->displayArea.GetMiddleRight());
                    textArea.SetX(textArea.GetX() - margins);
                    break;
                case UIAnchor::BOTTOM_LEFT:
                    textArea.SetBottomLeft(a_element->displayArea.GetBottomLeft());
                    textArea.SetX(textArea.GetX() + margins);
                    textArea.SetY(textArea.GetY() - margins);
                    break;
                case UIAnchor::BOTTOM_CENTER:
                    textArea.SetBottomCenter(a_element->displayArea.GetBottomCenter());
                    textArea.SetY(textArea.GetY() - margins);
                    break;
                case UIAnchor::BOTTOM_RIGHT:
                    textArea.SetBottomRight(a_element->displayArea.GetBottomRight());
                    textArea.SetX(textArea.GetX() - margins);
                    textArea.SetY(textArea.GetY() - margins);
                    break;
            }
        });

        a_element->onRender.emplace_back([this](GameState& a_gameState, UILayer* a_uiLayer, UIElement* a_element) {
            SDL_RenderTexture(a_gameState.renderer, textTexture, NULL, &textArea.rect);
        });
        
       SetAnchor(a_gameState, a_element, textAnchor);
    }

    void UILazyTextComponent::SetText(GameState& a_gameState, UIElement* a_element, std::string a_text) {
        text = a_text;
        a_gameState.assets.uiManager->dirtyRecalculate = true;
    }

    void UILazyTextComponent::SetAnchor(GameState& a_gameState, UIElement* a_element, UIAnchor a_anchor) {
        textAnchor = a_anchor;
        a_gameState.assets.uiManager->dirtyRecalculate = true;
    }
}