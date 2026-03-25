#pragma once
#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

struct GameState;

namespace ArsTimoris::UI {
    class UIElement;
}

#include <ArsTimoris/UI/UIElementComponent.h>
#include <ArsTimoris/UI/UIRect.h>

namespace ArsTimoris::UI {
    /// @brief 
    typedef std::function<void(GameState&, UILayer*, UIElement*)> UIEventFnptr;
    /// @brief 
    typedef std::function<bool(GameState&, UILayer*, UIElement*, SDL_FPoint*)> UIMousePredicateFnptr;

    /// @brief 
    class UIElement {
    public:
        /// @brief 
        /// @param  
        /// @param  
        UIElement(std::string_view a_id, UIRect a_internalArea);

        /// @brief The enablement state of the UI Element.
        bool enabled;
        /// @brief The ID of the UI Element.
        std::string id;
        /// @brief 
        std::unordered_map<std::string, std::shared_ptr<UIElementComponent>> components;
        /// @brief 
        std::shared_ptr<UIElement> parent;
        /// @brief 
        std::vector<std::shared_ptr<UIElement>> children;

        /// @brief 
        std::vector<UIEventFnptr> onRender;
        /// @brief 
        std::vector<UIMousePredicateFnptr> onMouseLeftDown;
        /// @brief 
        std::vector<UIMousePredicateFnptr> onMouseLeftHeld;
        /// @brief 
        std::vector<UIMousePredicateFnptr> onMouseLeftUp;
        /// @brief 
        std::vector<UIMousePredicateFnptr> onMouseMiddleDown;
        /// @brief 
        std::vector<UIMousePredicateFnptr> onMouseMiddleHeld;
        /// @brief 
        std::vector<UIMousePredicateFnptr> onMouseMiddleUp;
        /// @brief 
        std::vector<UIMousePredicateFnptr> onMouseRightDown;
        /// @brief 
        std::vector<UIMousePredicateFnptr> onMouseRightHeld;
        /// @brief 
        std::vector<UIMousePredicateFnptr> onMouseRightUp;
        /// @brief 
        std::vector<UIEventFnptr> onRecalculate;

        /// @brief The UIRect for the display area.
        UIRect displayArea;
        /// @brief The UIRect for the calculated area on a 1920x1080 screen.
        UIRect calculatedArea;
        /// @brief The UIRect for the internal area on a 1920x1080 screen.
        UIRect internalArea;

        UIAnchor anchor;
        UIAnchor parentAnchor;

        float paddingLeft   = 0;
        float paddingTop    = 0;
        float paddingBottom = 0;
        float paddingRight  = 0;

        float marginLeft   = 0;
        float marginTop    = 0;
        float marginBottom = 0;
        float marginRight  = 0;

        /// @brief 
        /// @param  
        /// @param  
        /// @param  
        /// @param  
        void Render(GameState& a_gameState, UILayer* a_layer);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseLeftDown(GameState& a_gameState, UILayer* a_layer, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseLeftHeld(GameState& a_gameState, UILayer* a_layer, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseLeftUp(GameState& a_gameState, UILayer* a_layer, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseMiddleDown(GameState& a_gameState, UILayer* a_layer, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseMiddleHeld(GameState& a_gameState, UILayer* a_layer, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseMiddleUp(GameState& a_gameState, UILayer* a_layer, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseRightDown(GameState& a_gameState, UILayer* a_layer, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseRightHeld(GameState& a_gameState, UILayer* a_layer, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseRightUp(GameState& a_gameState, UILayer* a_layer, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_layer 
        void Recalculate(GameState& a_gameState, UILayer* a_layer);

        std::shared_ptr<UIElement> AddChild(std::string_view a_id, UIRect a_internalArea);
    };
}
#endif