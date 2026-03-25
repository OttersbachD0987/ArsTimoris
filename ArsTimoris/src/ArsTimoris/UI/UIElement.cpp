#include <ArsTimoris/UI/UIElement.h>
#include <ArsTimoris/Game/GameState.h>
#include <print>

namespace ArsTimoris::UI { 
    UIElement::UIElement(std::string_view a_id, UIRect a_internalArea) {
        id = std::string(a_id);
        enabled = true;
        internalArea = a_internalArea;
        parent = nullptr;
        children = std::vector<std::shared_ptr<UIElement>>();
        anchor = UIAnchor::TOP_LEFT;
        parentAnchor = UIAnchor::TOP_LEFT;
    }

    void UIElement::Render(GameState& a_gameState, UILayer* a_uiLayer) {
        for (const UIEventFnptr& ptr : onRender) {
            ptr(a_gameState, a_uiLayer, this);
        }

        //std::println("{}) X: {} Y: {} W: {} H: {}", id, displayArea.rect.x, displayArea.rect.y, displayArea.rect.w, displayArea.rect.h);

        SDL_Rect oldClipRect = {0, 0, 0, 0};
        SDL_GetRenderClipRect(a_gameState.renderer, &oldClipRect);
        SDL_Rect newClipRect = displayArea.GetIRect();
        if (!SDL_RectEmpty(&oldClipRect)) {
            int def = std::clamp<int>(oldClipRect.x - newClipRect.x, 0, std::numeric_limits<int>::max());
            int end1 = oldClipRect.x + oldClipRect.w;
            int end2 = newClipRect.x + newClipRect.w;
            newClipRect.x += def;
            newClipRect.w -= def;
            newClipRect.w -= std::clamp<int>(end2 - end1, 0, std::numeric_limits<int>::max());
        }
        if (SDL_RectEmpty(&newClipRect)) {
            return;
        }
        //std::println("{}) X: {} Y: {} W: {} H: {}", id, newClipRect.x, newClipRect.y, newClipRect.w, newClipRect.h);
        SDL_SetRenderClipRect(a_gameState.renderer, &newClipRect);
        for (const std::shared_ptr<UIElement> child : children) {
            child->Render(a_gameState, a_uiLayer);
        }
        SDL_SetRenderClipRect(a_gameState.renderer, SDL_RectEmpty(&oldClipRect) ? NULL : &oldClipRect);
    }
    
    bool UIElement::OnMouseLeftDown(GameState& a_gameState, UILayer* a_uiLayer, SDL_FPoint* a_mousePos) {
        if (!SDL_PointInRectFloat(a_mousePos, &displayArea.rect)) {
            return false;
        }
        
        bool blocking = false;

        for (const UIMousePredicateFnptr& ptr : onMouseLeftDown) {
            blocking = blocking || ptr(a_gameState, a_uiLayer, this, a_mousePos);
        }

        if (blocking) {
            return true;
        }

        for (const std::shared_ptr<UIElement>& child : children) {
            if (child->OnMouseLeftDown(a_gameState, a_uiLayer, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UIElement::OnMouseLeftHeld(GameState& a_gameState, UILayer* a_uiLayer, SDL_FPoint* a_mousePos) {
        if (!SDL_PointInRectFloat(a_mousePos, &displayArea.rect)) {
            return false;
        }
        
        bool blocking = false;

        for (const UIMousePredicateFnptr& ptr : onMouseLeftHeld) {
            blocking = blocking || ptr(a_gameState, a_uiLayer, this, a_mousePos);
        }

        if (blocking) {
            return true;
        }

        for (const std::shared_ptr<UIElement>& child : children) {
            if (child->OnMouseLeftHeld(a_gameState, a_uiLayer, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UIElement::OnMouseLeftUp(GameState& a_gameState, UILayer* a_uiLayer, SDL_FPoint* a_mousePos) {
        if (!SDL_PointInRectFloat(a_mousePos, &displayArea.rect)) {
            return false;
        }
        
        bool blocking = false;

        for (const UIMousePredicateFnptr& ptr : onMouseLeftUp) {
            blocking = blocking || ptr(a_gameState, a_uiLayer, this, a_mousePos);
        }

        if (blocking) {
            return true;
        }

        for (const std::shared_ptr<UIElement>& child : children) {
            if (child->OnMouseLeftUp(a_gameState, a_uiLayer, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UIElement::OnMouseMiddleDown(GameState& a_gameState, UILayer* a_uiLayer, SDL_FPoint* a_mousePos) {
        if (!SDL_PointInRectFloat(a_mousePos, &displayArea.rect)) {
            return false;
        }
        
        bool blocking = false;

        for (const UIMousePredicateFnptr& ptr : onMouseMiddleDown) {
            blocking = blocking || ptr(a_gameState, a_uiLayer, this, a_mousePos);
        }

        if (blocking) {
            return true;
        }

        for (const std::shared_ptr<UIElement>& child : children) {
            if (child->OnMouseMiddleDown(a_gameState, a_uiLayer, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UIElement::OnMouseMiddleHeld(GameState& a_gameState, UILayer* a_uiLayer, SDL_FPoint* a_mousePos) {
        if (!SDL_PointInRectFloat(a_mousePos, &displayArea.rect)) {
            return false;
        }
        
        bool blocking = false;

        for (const UIMousePredicateFnptr& ptr : onMouseMiddleHeld) {
            blocking = blocking || ptr(a_gameState, a_uiLayer, this, a_mousePos);
        }

        if (blocking) {
            return true;
        }

        for (const std::shared_ptr<UIElement>& child : children) {
            if (child->OnMouseMiddleHeld(a_gameState, a_uiLayer, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UIElement::OnMouseMiddleUp(GameState& a_gameState, UILayer* a_uiLayer, SDL_FPoint* a_mousePos) {
        if (!SDL_PointInRectFloat(a_mousePos, &displayArea.rect)) {
            return false;
        }
        
        bool blocking = false;

        for (const UIMousePredicateFnptr& ptr : onMouseMiddleUp) {
            blocking = blocking || ptr(a_gameState, a_uiLayer, this, a_mousePos);
        }

        if (blocking) {
            return true;
        }

        for (const std::shared_ptr<UIElement>& child : children) {
            if (child->OnMouseMiddleUp(a_gameState, a_uiLayer, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UIElement::OnMouseRightDown(GameState& a_gameState, UILayer* a_uiLayer, SDL_FPoint* a_mousePos) {
        if (!SDL_PointInRectFloat(a_mousePos, &displayArea.rect)) {
            return false;
        }
        
        bool blocking = false;

        for (const UIMousePredicateFnptr& ptr : onMouseRightDown) {
            blocking = blocking || ptr(a_gameState, a_uiLayer, this, a_mousePos);
        }

        if (blocking) {
            return true;
        }

        for (const std::shared_ptr<UIElement>& child : children) {
            if (child->OnMouseRightDown(a_gameState, a_uiLayer, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UIElement::OnMouseRightHeld(GameState& a_gameState, UILayer* a_uiLayer, SDL_FPoint* a_mousePos) {
        if (!SDL_PointInRectFloat(a_mousePos, &displayArea.rect)) {
            return false;
        }
        
        bool blocking = false;

        for (const UIMousePredicateFnptr& ptr : onMouseRightHeld) {
            blocking = blocking || ptr(a_gameState, a_uiLayer, this, a_mousePos);
        }

        if (blocking) {
            return true;
        }

        for (const std::shared_ptr<UIElement>& child : children) {
            if (child->OnMouseRightHeld(a_gameState, a_uiLayer, a_mousePos)) {
                return true;
            }
        }

        return false;
    }

    bool UIElement::OnMouseRightUp(GameState& a_gameState, UILayer* a_uiLayer, SDL_FPoint* a_mousePos) {
        if (!SDL_PointInRectFloat(a_mousePos, &displayArea.rect)) {
            return false;
        }
        
        bool blocking = false;

        for (const UIMousePredicateFnptr& ptr : onMouseRightUp) {
            blocking = blocking || ptr(a_gameState, a_uiLayer, this, a_mousePos);
        }

        if (blocking) {
            return true;
        }

        for (const std::shared_ptr<UIElement>& child : children) {
            if (child->OnMouseRightUp(a_gameState, a_uiLayer, a_mousePos)) {
                return true;
            }
        }

        return false;
    }
    
    void UIElement::Recalculate(GameState& a_gameState, UILayer* a_uiLayer) {
        int w, h;
        SDL_GetCurrentRenderOutputSize(a_gameState.renderer, &w, &h);

        calculatedArea = {{0, 0, internalArea.rect.w, internalArea.rect.h}};
        SDL_FPoint position = {0, 0};

        /*
        if (parent == nullptr) {
            std::cout << id << " has no parent." << std::endl;
        } else {
            std::println("The parent of {} is {} X: {} Y: {} W: {} H: {}", id, parent->id, parent->calculatedArea.rect.x, parent->calculatedArea.rect.y, parent->calculatedArea.rect.w, parent->calculatedArea.rect.h);
        }
        */

        switch (parentAnchor) {
            case UIAnchor::TOP_LEFT:
                if (parent != nullptr) {
                    position = parent->calculatedArea.GetTopLeft();
                    position.x += parent->paddingLeft;
                    position.y += parent->paddingTop;
                } else {
                    position = {0, 0};
                }
                break;
            case UIAnchor::TOP_CENTER:
                if (parent != nullptr) {
                    position = parent->calculatedArea.GetTopCenter();
                    position.y += parent->paddingTop;
                } else {
                    position = {w * 0.5f, 0};
                }
                break;
            case UIAnchor::TOP_RIGHT:
                if (parent != nullptr) {
                    position = parent->calculatedArea.GetTopRight();
                    position.x -= parent->paddingRight;
                    position.y += parent->paddingTop;
                } else {
                    position = {(float)w, 0};
                }
                break;
            case UIAnchor::MIDDLE_LEFT:
                if (parent != nullptr) {
                    position = parent->calculatedArea.GetMiddleLeft();
                    position.x += parent->paddingLeft;
                } else {
                    position = {0, h * 0.5f};
                }
                break;
            case UIAnchor::MIDDLE_CENTER:
                if (parent != nullptr) {
                    position = parent->calculatedArea.GetMiddleCenter();
                } else {
                    position = {w * 0.5f, h * 0.5f};
                }
                break;
            case UIAnchor::MIDDLE_RIGHT:
                if (parent != nullptr) {
                    position = parent->calculatedArea.GetMiddleRight();
                    position.x -= parent->paddingRight;
                } else {
                    position = {(float)w, h * 0.5f};
                }
                break;
            case UIAnchor::BOTTOM_LEFT:
                if (parent != nullptr) {
                    position = parent->calculatedArea.GetBottomLeft();
                    position.x += parent->paddingLeft;
                    position.y -= parent->paddingBottom;
                } else {
                    position = {0, (float)h};
                }
                break;
            case UIAnchor::BOTTOM_CENTER:
                if (parent != nullptr) {
                    position = parent->calculatedArea.GetBottomCenter();
                    position.y -= parent->paddingBottom;
                } else {
                    position = {w * 0.5f, (float)h};
                }
                break;
            case UIAnchor::BOTTOM_RIGHT:
                if (parent != nullptr) {
                    position = parent->calculatedArea.GetBottomRight();
                    position.x -= parent->paddingRight;
                    position.y -= parent->paddingBottom;
                } else {
                    position = {(float)w, (float)h};
                }
                break;
        }

        //std::println("{}) P: {} X: {} Y: {}", id, parent != nullptr ? parent->id : "None", position.x, position.y);

        switch (anchor) {
            case UIAnchor::TOP_LEFT:
                position.x += marginLeft;
                position.y += marginTop;
                calculatedArea.SetTopLeft(position);
                break;
            case UIAnchor::TOP_CENTER:
                position.y += marginTop;
                calculatedArea.SetTopCenter(position);
                break;
            case UIAnchor::TOP_RIGHT:
                position.x -= marginRight;
                position.y += marginTop;
                calculatedArea.SetTopRight(position);
                break;
            case UIAnchor::MIDDLE_LEFT:
                position.x += marginLeft;
                calculatedArea.SetMiddleLeft(position);
                break;
            case UIAnchor::MIDDLE_CENTER:
                calculatedArea.SetMiddleCenter(position);
                break;
            case UIAnchor::MIDDLE_RIGHT:
                position.x -= marginRight;
                calculatedArea.SetMiddleRight(position);
                break;
            case UIAnchor::BOTTOM_LEFT:
                position.x += marginLeft;
                position.y -= marginBottom;
                calculatedArea.SetBottomLeft(position);
                break;
            case UIAnchor::BOTTOM_CENTER:
                position.y -= marginBottom;
                calculatedArea.SetBottomCenter(position);
                break;
            case UIAnchor::BOTTOM_RIGHT:
                position.x -= marginRight;
                position.y -= marginBottom;
                calculatedArea.SetBottomRight(position);
                break;
        }

        position = calculatedArea.GetTopLeft();
        position.x += internalArea.rect.x;
        position.y += internalArea.rect.y;
        calculatedArea.SetTopLeft(position);

        float widthScale = (w / 1200.0f);
        float heightScale = (h / 800.0f);
        displayArea = {
            {
                calculatedArea.GetX() * widthScale, 
                calculatedArea.GetY() * heightScale, 
                calculatedArea.GetWidth() * widthScale, 
                calculatedArea.GetHeight() * heightScale
            }
        };
    
        for (const UIEventFnptr& ptr : onRecalculate) {
            ptr(a_gameState, a_uiLayer, this);
        }

        //std::println("Prae children of {}) P: {} X: {} Y: {} W: {} H: {}", id, parent != nullptr ? parent->id : "None", calculatedArea.rect.x, calculatedArea.rect.y, calculatedArea.rect.w, calculatedArea.rect.h);

        for (const std::shared_ptr<UIElement> child : children) {
            child->Recalculate(a_gameState, a_uiLayer);
        }

        //std::println("Post children of {}) P: {} X: {} Y: {} W: {} H: {}", id, parent != nullptr ? parent->id : "None", calculatedArea.rect.x, calculatedArea.rect.y, calculatedArea.rect.w, calculatedArea.rect.h);
    }

    std::shared_ptr<UIElement> UIElement::AddChild(std::string_view a_id, UIRect a_internalArea) {
        children.push_back(std::make_unique<UIElement>(a_id, a_internalArea));
        children.back()->parent = std::shared_ptr<UIElement>(this);
        return children.back();
    }
}