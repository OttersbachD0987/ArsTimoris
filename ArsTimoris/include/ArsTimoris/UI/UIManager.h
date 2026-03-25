#pragma once
#ifndef UI_SCREEN_H
#define UI_SCREEN_H
#include <string>
#include <unordered_map>
#include <ArsTimoris/UI/UILayer.h>
#include <ArsTimoris/Assets/Assets.h>

namespace ArsTimoris::UI {
    /// @brief 
    class UIManager {
    public:
        /// @brief 
        UIManager();

        /// @brief 
        std::unordered_map<std::string, UILayer> uiLayers;
        /// @brief 
        bool dirtyRecalculate = true;

        bool mouseLeftHeld   = false;
        bool mouseMiddleHeld = false;
        bool mouseRightHeld  = false;

        /// @brief 
        /// @param  
        void Render(GameState& a_gameState);
        /// @brief 
        /// @param  
        /// @param   
        /// @return 
        bool OnMouseLeftDown(GameState&, SDL_FPoint*);
        /// @brief 
        /// @param  
        /// @param  
        /// @return 
        bool OnMouseLeftHeld(GameState&, SDL_FPoint*);
        /// @brief 
        /// @param  
        /// @param  
        /// @return 
        bool OnMouseLeftUp(GameState&, SDL_FPoint*);
        /// @brief 
        /// @param  
        /// @param  
        /// @return 
        bool OnMouseMiddleDown(GameState&, SDL_FPoint*);
        /// @brief 
        /// @param  
        /// @param  
        /// @return 
        bool OnMouseMiddleHeld(GameState&, SDL_FPoint*);
        /// @brief 
        /// @param  
        /// @param  
        /// @return 
        bool OnMouseMiddleUp(GameState&, SDL_FPoint*);
        /// @brief 
        /// @param  
        /// @param  
        /// @return 
        bool OnMouseRightDown(GameState&, SDL_FPoint*);
        /// @brief 
        /// @param  
        /// @param  
        /// @return 
        bool OnMouseRightHeld(GameState&, SDL_FPoint*);
        /// @brief 
        /// @param  
        /// @param  
        /// @return 
        bool OnMouseRightUp(GameState&, SDL_FPoint*);
        /// @brief 
        /// @param  
        void Process(GameState& a_gameState);
        /// @brief 
        /// @param  
        void Recalculate(GameState& a_gameState);
    };
}
#endif