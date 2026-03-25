#pragma once
#ifndef ARS_TIMORIS__GAME__ENTITY__NPC_DATA_H
#define ARS_TIMORIS__GAME__ENTITY__NPC_DATA_H
struct NPCData;

#include <ArsTimoris/Game/Entity/EntityData.h>
#include <ArsTimoris/Game/Entity/NPCTemplate.h>

struct NPCData : public EntityData {
public:
    int32_t xp;
    int32_t gold;
    const std::function<void(GameState&, NPCData&)>* aiFunction;
    std::vector<std::function<void(GameState&, NPCData&)>> onDeath = std::vector<std::function<void(GameState&, NPCData&)>>();
    bool stunned = false;

    NPCData(void);
    NPCData(const EntityTemplate& a_entityTemplate, const NPCTemplate* const a_npcTemplate);

    int32_t GetEffectiveArmor(void);

    void Hurt(int32_t a_damage);
    void Heal(int32_t a_amount);

    void DrainMana(int32_t a_amount);
    void RegainMana(int32_t a_amount);
};
#endif