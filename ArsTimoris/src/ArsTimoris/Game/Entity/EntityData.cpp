#include <ArsTimoris/Game/Entity/EntityData.h>
#include <ArsTimoris/Game/Entity/EntityTemplate.h>
#include <ArsTimoris/Game/Interaction/Action.h>

EntityData::EntityData(void) {
    this->name = "";
    this->maxHP = this->curHP = this->maxMana = this->curMana = this->manaRegen = this->manaSickness = this->armor = 0;
    this->skills = std::unordered_map<std::string, int32_t>();
    this->actions = std::vector<Action>();
}

EntityData::EntityData(const EntityTemplate& a_entityTemplate) {
    this->name = a_entityTemplate.name;
    this->maxHP = this->curHP = a_entityTemplate.hp;
    this->maxMana = this->curMana = a_entityTemplate.mana;
    this->manaRegen = a_entityTemplate.manaRegen;
    this->manaSickness = 0;
    this->armor = a_entityTemplate.armor;
    this->skills = a_entityTemplate.skills;
    this->actions = a_entityTemplate.actions;
}

int32_t EntityData::GetSkillModifier(const std::string& a_skill) {
    return skills.find(a_skill) != skills.end() ? skills.at(a_skill) : 0;
}

int32_t EntityData::SetSkillModifier(const std::string& a_skill, int32_t a_modifier) {
    skills.insert_or_assign(a_skill, a_modifier);
    return skills.at(a_skill);
}

int32_t EntityData::AddSkillModifier(const std::string& a_skill, int32_t a_modifier) {
    skills.insert_or_assign(a_skill, GetSkillModifier(a_skill) + a_modifier);
    return skills.at(a_skill);
}

void EntityData::TurnEnd(GameState& a_gameState) {
    if (manaSickness <= 0) {
        if (manaRegen > 0) {
            RegainMana(manaRegen);
        } else if (manaRegen < 0) {
            DrainMana(-manaRegen);
        }
    } else {
        --manaSickness;
    }

    std::vector<std::string> toRemove = std::vector<std::string>();

    for (std::pair<std::string, TimedEffect> effectPair : timedEffects) {
        if (--effectPair.second.timeLeft <= 0) {
            toRemove.push_back(effectPair.first);
            if (effectPair.second.onRemove != nullptr) {
                effectPair.second.onRemove(a_gameState, *this);
            }
        } else {
            if (effectPair.second.onTick != nullptr) {
                effectPair.second.onTick(a_gameState, *this);
            }
        }
    }

    for (std::string key : toRemove) {
        timedEffects.erase(key);
    }
}

EntityData::~EntityData() {
    
}