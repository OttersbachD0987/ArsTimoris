#pragma once
#ifndef ARS_TIMORIS__GAME__ENTITY__ENTITY_TEMPLATE_H
#define ARS_TIMORIS__GAME__ENTITY__ENTITY_TEMPLATE_H
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string_view>
#include <filesystem>
#include <ArsTimoris/Game/Interaction/Action.h>

/// @struct EntityTemplate
/// @brief A template for a base entity.
struct EntityTemplate {
public:

    std::string name;                                //< The name of the entity.
    int32_t hp;                                      //< The max HP of the entity.
    int32_t mana;                                    //< The max mana of the entity.
    int32_t manaRegen;                               //< The mana regeneration of the entity.
    int32_t armor;                                   //< The armor of the entity.
    std::unordered_map<std::string, int32_t> skills; //< The skills of the entity.
    std::vector<Action> actions;                     //< The actions of the entity.

    /// @brief An entity template with all its values set to the default.
    EntityTemplate(void);

    /// @brief An entity template with assigned values.
    ///
    /// @param a_name      The name of the entity.
    /// @param a_hp        The word max HP of the entity.
    /// @param a_mana      The max mana of the entity.
    /// @param a_manaRegen The mana regeneration of the entity.
    /// @param a_armor     The armor of the entity.
    /// @param a_skills    The skills of the entity.
    /// @param a_actions   The actions of the entity.
    EntityTemplate(std::string_view a_name, int32_t a_hp, int32_t a_mana, int32_t a_manaRegen, int32_t a_armor, const std::unordered_map<std::string, int32_t>& a_skills, const std::vector<Action>& a_actions);
};

std::unordered_map<std::string, EntityTemplate> LoadEntityData(std::filesystem::path a_path);
#endif