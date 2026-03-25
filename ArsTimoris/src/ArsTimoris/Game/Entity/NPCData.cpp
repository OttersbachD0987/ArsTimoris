#include <ArsTimoris/Game/Entity/NPCData.h>
#include <ArsTimoris/Game/Interaction/Action.h>

NPCData::NPCData(void) : EntityData() {
    this->name = "Unknown";
    this->xp = 0;
    this->gold = 0;
    this->aiFunction = nullptr;
}

NPCData::NPCData(const EntityTemplate& a_entityTemplate, const NPCTemplate* const a_npcTemplate) : EntityData(a_entityTemplate) {
    this->xp = a_npcTemplate->xp;
    this->gold = a_npcTemplate->gold;
    this->aiFunction = &a_npcTemplate->ai;
}

int32_t NPCData::GetEffectiveArmor(void) {
    return armor;
}

void NPCData::Hurt(int32_t a_damage) {
    curHP = std::max<int32_t>(curHP - a_damage, 0);
    if (curHP <= 0) {
        // Do stuff when the NPC dies. Adding a signal for this most definitely.
    }
}

void NPCData::Heal(int32_t a_amount) {
    curHP = std::min<int32_t>(curHP + a_amount, maxHP);
}

void NPCData::DrainMana(int32_t a_amount) {
    curMana = std::max<int32_t>(curMana - a_amount, 0);
    if (curMana <= 0) {
        // Do stuff when the NPC runs out of mana. Probably going to add a signal to it.
    }
}

void NPCData::RegainMana(int32_t a_amount) {
    curMana = std::min<int32_t>(curMana + a_amount, maxMana);
}