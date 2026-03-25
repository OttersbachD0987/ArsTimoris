#include <ArsTimoris/UI/UILayer.h>
#include <ArsTimoris/Game/GameState.h>

namespace ArsTimoris::UI { 
    UILayer::UILayer(std::string_view a_id) {
        id = std::string(a_id);
        enabled = true;
        uiElements = std::unordered_map<std::string, UIElement>();
    }

    void UILayer::Render(GameState& a_gameState) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            if (!pair.second.enabled) {
                continue;
            }

            pair.second.Render(a_gameState, this);
        }
    }

    bool UILayer::OnMouseLeftDown(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseLeftDown(a_gameState, this, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UILayer::OnMouseLeftHeld(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseLeftHeld(a_gameState, this, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UILayer::OnMouseLeftUp(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseLeftUp(a_gameState, this, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UILayer::OnMouseMiddleDown(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseMiddleDown(a_gameState, this, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UILayer::OnMouseMiddleHeld(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseMiddleHeld(a_gameState, this, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UILayer::OnMouseMiddleUp(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseMiddleUp(a_gameState, this, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UILayer::OnMouseRightDown(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseRightDown(a_gameState, this, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UILayer::OnMouseRightHeld(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseRightHeld(a_gameState, this, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UILayer::OnMouseRightUp(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseRightUp(a_gameState, this, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    void UILayer::Recalculate(GameState& a_gameState) {
        for (std::pair<const std::string, UIElement>& pair : this->uiElements) {
            pair.second.Recalculate(a_gameState, this);
        }
    }

    void UILayer::Clear(void) {
        uiElements.clear();
    }
}