#pragma once
#ifndef UI_LAYER_H
#define UI_LAYER_H
#include <string>
#include <unordered_map>
#include <ArsTimoris/UI/UIElement.h>

namespace ArsTimoris::UI {
    /// @brief 
    class UILayer {
    public:
        /// @brief 
        /// @param a_id 
        UILayer(std::string_view a_id);

        /// @brief 
        std::string id;
        /// @brief 
        bool enabled;
        /// @brief 
        std::unordered_map<std::string, UIElement> uiElements;

        /// @brief 
        /// @param a_gameState
        void Render(GameState& a_gameState);
        /// @brief 
        /// @param a_gameState 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseLeftDown(GameState& a_gameState, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseLeftHeld(GameState& a_gameState, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseLeftUp(GameState& a_gameState, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseMiddleDown(GameState& a_gameState, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseMiddleHeld(GameState& a_gameState, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseMiddleUp(GameState& a_gameState, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseRightDown(GameState& a_gameState, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseRightHeld(GameState& a_gameState, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        /// @param a_mousePos 
        /// @return 
        bool OnMouseRightUp(GameState& a_gameState, SDL_FPoint* a_mousePos);
        /// @brief 
        /// @param a_gameState 
        void Recalculate(GameState& a_gameState);
        /// @brief 
        void Clear(void);
    };
}
#endif