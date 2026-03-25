#pragma once
#ifndef ARS_TIMORIS__GAME__CLASSES__CLASS_DATA_H
#define ARS_TIMORIS__GAME__CLASSES__CLASS_DATA_H
#include <string>
#include <vector>
#include <cstdint>

struct ClassData;

#include <ArsTimoris/Game/Classes/ClassLevel.h>

/// @struct Class
/// @brief A class for the player, holds the individual levels in an organized format.
struct ClassData {
public:
    std::string name;               //< The name of the class.
    std::string description;        //< The description of the class.
    std::vector<ClassLevel> levels; //< The levels of the class.

    /// @brief Get the max level of the class.
    /// @return The max level of the class.
    uint32_t MaxLevel(void) {
        return (int32_t)levels.size();
    }
};
#endif