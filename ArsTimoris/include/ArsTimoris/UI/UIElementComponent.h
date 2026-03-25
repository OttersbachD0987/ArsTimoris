#pragma once
#ifndef UI_ELEMENT_COMPONENT_H
#define UI_ELEMENT_COMPONENT_H
#include <string>
#include <ArsTimoris/Assets/Assets.h>

struct  GameState;

namespace ArsTimoris::UI {
    class UIElement;
    class UILayer;

    /// @brief 
    class UIElementComponent {
    public:
        /// @brief The id of the UI Component.
        std::string id;
        /// @brief The ID of the UI Element that contains this component.
        std::string elementID;

        /// @brief 
        /// @param  
        /// @param  
        /// @param  
        /// @param  
        /// @param  
        virtual void Hookup(GameState& a_gameState, UILayer*, UIElement*) = 0;
        
        //virtual ~UIElementComponent() = 0;
    };
}
#endif