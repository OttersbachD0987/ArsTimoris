#include <ArsTimoris/UI/UIManager.h>
#include <ArsTimoris/Game/GameState.h>

namespace ArsTimoris::UI { 
    UIManager::UIManager() {
        uiLayers = std::unordered_map<std::string, UILayer>();
    }

    void UIManager::Render(GameState& a_gameState) {
        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            if (!pair.second.enabled) {
                continue;
            }

            pair.second.Render(a_gameState);
        }
    }

    bool UIManager::OnMouseLeftDown(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        mouseLeftHeld = true;

        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseLeftDown(a_gameState, a_mousePos)) {
                return true;
            }
        }
        
        return false;
    }

    bool UIManager::OnMouseLeftHeld(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseLeftHeld(a_gameState, a_mousePos)) {
                return true;
            }
        }
        
        return false;
    }

    bool UIManager::OnMouseLeftUp(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        mouseLeftHeld = false;

        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseLeftUp(a_gameState, a_mousePos)) {
                return true;
            }
        }
        
        return false;
    }

    bool UIManager::OnMouseMiddleDown(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        mouseMiddleHeld = true;

        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseMiddleDown(a_gameState, a_mousePos)) {
                return true;
            }
        }
        
        return false;
    }

    bool UIManager::OnMouseMiddleHeld(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseMiddleHeld(a_gameState, a_mousePos)) {
                return true;
            }
        }
        
        return false;
    }

    bool UIManager::OnMouseMiddleUp(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        mouseMiddleHeld = false;
        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseMiddleUp(a_gameState, a_mousePos)) {
                return true;
            }
        }
        
        return false;
    }

    bool UIManager::OnMouseRightDown(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        mouseRightHeld = true;
        
        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseRightDown(a_gameState, a_mousePos)) {
                return true;
            }
        }
        
        return false;
    }

    bool UIManager::OnMouseRightHeld(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseRightHeld(a_gameState, a_mousePos)) {
                return true;
            }
        }
        
        return false;
    }

    bool UIManager::OnMouseRightUp(GameState& a_gameState, SDL_FPoint* a_mousePos) {
        mouseRightHeld = false;

        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            if (!pair.second.enabled) {
                continue;
            }

            if (pair.second.OnMouseRightUp(a_gameState, a_mousePos)) {
                return true;
            }
        }
        
        return false;
    }

    void UIManager::Process(GameState& a_gameState) {
        if (mouseLeftHeld) {
            OnMouseLeftHeld(a_gameState, &a_gameState.inputData.mousePos);
        }

        if (mouseMiddleHeld) {
            OnMouseMiddleHeld(a_gameState, &a_gameState.inputData.mousePos);
        }

        if (mouseRightHeld) {
            OnMouseRightHeld(a_gameState, &a_gameState.inputData.mousePos);
        }
    }

    void UIManager::Recalculate(GameState& a_gameState) {
        dirtyRecalculate = false;
        for (std::pair<const std::string, UILayer>& pair : this->uiLayers) {
            pair.second.Recalculate(a_gameState);
        }
    }
}