#include <ArsTimoris/Game/Entity/PlayerData.h>
#include <ArsTimoris/Game/GameData.h>

std::string PerkDisplayName(Perks a_perk) {
    switch (a_perk) {
        case Perks::INSIGHT:
            return "INSIGHT";
        case Perks::ARCANE_EYES:
            return "ARCANE_EYES";
        case Perks::HARD_HITTER:
            return "HARD_HITTER";
        case Perks::HORDE_SLAYER:
            return "HORDE_SLAYER";
        default:
            return "INVALID";
    }
}

Perks PerkFromName(const std::string& a_perk) {
    if (a_perk == "INSIGHT") {
        return Perks::INSIGHT;
    } else if (a_perk == "ARCANE_EYES") {
        return Perks::ARCANE_EYES;
    } else if (a_perk == "HARD_HITTER") {
        return Perks::HARD_HITTER;
    } else if (a_perk == "HORDE_SLAYER") {
        return Perks::HORDE_SLAYER;
    } else {
        return Perks::SIZE;
    }
}

PlayerData::PlayerData(void) : EntityData() {
    this->classes = std::unordered_map<std::string, ClassInstance>();
    this->perks = std::bitset<static_cast<size_t>(Perks::SIZE)>();
    this->items = std::vector<ItemStack>();
    this->equipped = std::vector<bool>();
    this->gold = 0;
    this->xp = 0;
}

PlayerData::PlayerData(const EntityTemplate& a_entityTemplate) : EntityData(a_entityTemplate) {
    this->classes = std::unordered_map<std::string, ClassInstance>();
    this->perks = std::bitset<static_cast<size_t>(Perks::SIZE)>();
    this->items = std::vector<ItemStack>();
    this->equipped = std::vector<bool>();
    this->gold = 0;
    this->xp = 0;
}

PlayerData::PlayerData(const EntityTemplate& a_entityTemplate, const std::vector<size_t>& a_perks, size_t a_turns) : EntityData(a_entityTemplate) {
    this->classes = std::unordered_map<std::string, ClassInstance>();
    this->perks = std::bitset<static_cast<size_t>(Perks::SIZE)>();
    for (const size_t& perk : a_perks) {
        this->perks.set(perk);
    }
    this->items = std::vector<ItemStack>();
    this->equipped = std::vector<bool>();
    this->turns = (int32_t)a_turns;
    this->gold = 0;
    this->xp = 0;
}

#pragma region Inventory
void PlayerData::UseItemAt(size_t a_index) {
    if (--items[a_index].stackSize <= 0) {
        items.erase(items.begin() + a_index);
        equipped.erase(equipped.begin() + a_index);
    }
}

void PlayerData::UseItemOf(size_t a_itemID) {
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].itemID == a_itemID) {
            UseItemAt(i);
            return;
        }
    }
}

void PlayerData::AddItem(size_t a_itemID, uint32_t a_itemAmount) {
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].itemID == a_itemID) {
            items[i].stackSize += a_itemAmount;
            return;
        }
    }
    items.push_back(ItemStack(a_itemID, a_itemAmount));
    equipped.push_back(false);
}

bool PlayerData::HasItem(size_t a_itemID, uint32_t a_itemAmount) {
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].itemID == a_itemID) {
            return a_itemAmount <= items[i].stackSize;
        }
    }
    return false;
}

std::optional<size_t> PlayerData::HasMatchingEquippedItem(GameState& a_gameState, const std::function<bool(GameState&, const ItemStack&)>& a_predicate) {
    for (size_t i = 0; i < items.size(); ++i) {
        if (equipped[i] && a_predicate(a_gameState, items[i])) {
            return std::optional<size_t>(i);
        }
    }
    return std::optional<size_t>();
}
#pragma endregion

int32_t PlayerData::LevelInClass(const std::string& a_class) {
    return classes.contains(a_class) ? classes.at(a_class).level + 1 : 0;
}

int32_t PlayerData::GetEffectiveArmor(void) {
    int32_t effectiveArmor = armor;
    for (size_t i = 0; i < items.size(); ++i) {
        if (equipped[i]) {
            effectiveArmor += items[i].GetMetadata("ToArmor");
            if (GameData::ITEM_DATA[items[i].itemID].tags.contains("Armor")) {
                effectiveArmor += GameData::ITEM_DATA[items[i].itemID].tags.at("Armor");
            }
        }
    }
    return effectiveArmor;
}

void PlayerData::Hurt(int32_t a_damage) {
    curHP = std::max<int32_t>(curHP - a_damage, 0);
    if (curHP <= 0) {
        // Do stuff when the player dies. Can add things like revives or something. Probably going to add a signal to it.
    }
}

void PlayerData::Heal(int32_t a_amount) {
    curHP = std::min<int32_t>(curHP + a_amount, maxHP);
}

void PlayerData::DrainMana(int32_t a_amount) {
    curMana = std::max<int32_t>(curMana - a_amount, 0);
    if (curMana <= 0) {
        // Do stuff when the player runs out of mana. Probably going to add a signal to it.
    }
}

void PlayerData::RegainMana(int32_t a_amount) {
    curMana = std::min<int32_t>(curMana + a_amount, maxMana);
}