#pragma once
#ifndef ARS_TIMORIS__GAME__ENTITY__ENTITY_DATA_H
#define ARS_TIMORIS__GAME__ENTITY__ENTITY_DATA_H
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

struct Action;
struct EntityTemplate;

#include <ArsTimoris/Game/Interaction/TimedEffect.h>

struct EntityData {
public:
    std::string name;                                //< The name of the entity.
    int32_t  maxHP;                                  //< The max HP of the entity.
    int32_t  curHP;                                  //< The current HP of the entity.
    uint32_t maxMana;                                //< The max mana of the entity.
    uint32_t curMana;                                //< The current mana of the entity.
    int32_t  manaRegen;                              //< The mana regeneration of the entity.
    int32_t  manaSickness;                           //< How long until the entity can regain mana.
    int32_t  armor;                                  //< The armor of the entity.
    std::unordered_map<std::string, int32_t> skills; //< The skills of the entity.
    std::vector<Action> actions;                     //< The actions of the entity.
    std::unordered_map<std::string, TimedEffect> timedEffects = std::unordered_map<std::string, TimedEffect>();

    EntityData(void);
    EntityData(const EntityTemplate& a_entityTemplate);

    virtual int32_t GetEffectiveArmor(void) = 0;

    virtual void Hurt(int32_t) = 0;
    virtual void Heal(int32_t) = 0;

    virtual void DrainMana(int32_t a_amount) = 0;
    virtual void RegainMana(int32_t a_amount) = 0;

    int32_t GetSkillModifier(const std::string& a_skill);
    int32_t SetSkillModifier(const std::string& a_skill, int32_t a_modifier);
    int32_t AddSkillModifier(const std::string& a_skill, int32_t a_modifier);

    void TurnEnd(GameState& a_gameState);

    virtual ~EntityData();
};

#endif