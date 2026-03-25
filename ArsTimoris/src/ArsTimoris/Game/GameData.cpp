#include <ArsTimoris/Game/GameData.h>

std::unordered_map<std::string, Action> GameData::STANDARD_ACTIONS = {
    {
        "Punch",  Action {
            "Punch",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_target != a_caster && a_target->curHP > 0; 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                std::cout << "The " << a_caster->name << " throws a punch ";
                int32_t modifier = 
                    a_caster->GetSkillModifier("Martial Combat") +
                    a_caster->GetSkillModifier("Unarmed Combat") +
                    a_caster->GetSkillModifier("Brawling");
                size_t opponents = 1;
                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                if (player != nullptr) {
                    opponents = a_gameState.rooms[a_gameState.curRoom].LivingInhabitants();
                }
                if (opponents > 1) {
                    modifier += a_caster->GetSkillModifier("Melee Combat");
                }
                int32_t roll = a_gameState.RollDice(1, 20);
                int32_t result = roll + modifier;
                if (roll == 20 || result >= a_target->GetEffectiveArmor()) {
                    int32_t damage = 0;
                    int32_t damageModifier = (int32_t)(modifier * 0.5);
                    int32_t damageTier = 1;
                    if (player != nullptr) {
                        if (player->perks.test((size_t)Perks::HARD_HITTER)) {
                            damageTier += 1;
                        }
                    }

                    if (modifier >= 12) {
                        damageTier += 2;
                    } else if (modifier >= 8) {
                        damageTier += 1;
                    } else if (modifier <= -4) {
                        damageTier -= 1;
                    } else if (modifier <= -8) {
                        damageTier -= 2;
                    }

                    switch (damageTier = std::clamp<int32_t>(damageTier, 0, 4)) {
                        case 0:
                            break;
                        case 1:
                            damage = a_gameState.RollDice(1, 12);
                            break;
                        case 2:
                            damage = a_gameState.RollDice(2, 6);
                            break;
                        case 3:
                            damage = a_gameState.RollDice(3, 4);
                            break;
                        case 4:
                            damage = a_gameState.RollDice(4, 3);
                            break;
                    }

                    a_target->Hurt(damage + damageModifier);

                    std::cout << "and hits with a " << result << " (" << roll << " + " << modifier << "), dealing " << (damage + damageModifier) << " (" << damage; 
                    switch (damageTier) {
                        case 0:
                            break;
                        case 1:
                            std::cout << " (1d12)";
                            break;
                        case 2:
                            std::cout << " (2d6)";
                            break;
                        case 3:
                            std::cout << " (3d4)";
                            break;
                        case 4:
                            std::cout << " (4d3)";
                            break;
                    }
                    if (damageModifier > 0) {
                        std::cout << " + " << damageModifier;
                    } else if (damageModifier < 0) {
                        
                        std::cout << " - " << -damageModifier;
                    }
                    std::cout << ").\n" << std::endl;
                } else {
                    std::cout << "and misses with a " << result << " (" << roll << " + " << modifier << ").\n" << std::endl;
                }

                EatInput();

                if (player != nullptr) {
                    ++player->usedTurns;
                }
            }
        }
    },
    {
        "Swift Strike",  Action {
            "Swift Strike",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_target != a_caster && a_target->curHP > 0 && a_gameState.player.HasMatchingEquippedItem(a_gameState, [](GameState& a_gameState, const ItemStack& a_itemStack) { return ITEM_DATA[a_itemStack.itemID].tags.contains("Blade"); }).has_value(); 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                ItemStack& itemStack = a_gameState.player.items[a_gameState.player.HasMatchingEquippedItem(a_gameState, [](GameState& a_gameState, const ItemStack& a_itemStack) { return ITEM_DATA[a_itemStack.itemID].tags.contains("Blade"); }).value()];
                const ItemData& itemType = ITEM_DATA[itemStack.itemID];
                BladeType bladeType = (BladeType)itemType.tags.at("Blade");
                std::cout << "The " << a_caster->name << " swings their " << itemType.name << " ";
                int32_t modifier = 
                    a_caster->GetSkillModifier("Martial Combat") +
                    itemStack.GetMetadata("ToHit", 0);
                switch (bladeType) {
                    case BladeType::SHORT_SWORD:
                    case BladeType::LONG_SWORD:
                        modifier += a_caster->GetSkillModifier("Swordsmanship");
                        break;
                }
                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                size_t opponents = 1;
                if (player != nullptr) {
                    opponents = a_gameState.rooms[a_gameState.curRoom].LivingInhabitants();
                }
                if (opponents == 1) {
                    modifier += a_caster->GetSkillModifier("Dueling");
                } else {
                    modifier += a_caster->GetSkillModifier("Melee Combat");
                }
                int32_t roll = a_gameState.RollDice(1, 20);
                NPCData* npc = dynamic_cast<NPCData*>(a_target);
                if (npc != nullptr) {
                    if (npc->stunned) {
                        roll = std::max<int32_t>(roll, a_gameState.RollDice(1, 20));
                    }
                }
                int32_t result = roll + modifier;
                if (roll == 20 || result >= a_target->GetEffectiveArmor()) {
                    int32_t damage = 0;
                    int32_t damageModifier = (int32_t)(modifier * 0.5);
                    int32_t damageTier = 1;
                    if (player != nullptr) {
                        if (opponents > 3 && player->perks.test((size_t)Perks::HORDE_SLAYER)) {
                            damageTier += 1;
                        }
                    }

                    if (modifier >= 12) {
                        damageTier += 3;
                    } else if (modifier >= 9) {
                        damageTier += 2;
                    } else if (modifier >= 6) {
                        damageTier += 1;
                    } else if (modifier <= -4) {
                        damageTier -= 1;
                    } else if (modifier <= -8) {
                        damageTier -= 2;
                    } else if (modifier <= -12) {
                        damageTier -= 3;
                    }

                    switch (bladeType) {
                        case BladeType::DAGGER:
                            switch (damageTier = std::clamp<int32_t>(damageTier, 0, 4)) {
                                case 0:
                                    damage = a_gameState.RollDice(1, 8);
                                    break;
                                case 1:
                                    damage = a_gameState.RollDice(2, 4);
                                    break;
                                case 2:
                                    damage = 2 * a_gameState.RollDice(1, 4);
                                    break;
                                case 3:
                                    damage = a_gameState.RollDice(3, 3);
                                    break;
                                case 4:
                                    damage = 3 * a_gameState.RollDice(1, 3);
                                    break;
                            }
                            break;
                        case BladeType::SHORT_SWORD:
                        case BladeType::LONG_SWORD:
                            switch (damageTier = std::clamp<int32_t>(damageTier, 0, 4)) {
                                case 0:
                                    damage = a_gameState.RollDice(1, 12);
                                    break;
                                case 1:
                                    damage = a_gameState.RollDice(1, 16);
                                    break;
                                case 2:
                                    damage = a_gameState.RollDice(2, 8);
                                    break;
                                case 3:
                                    damage = a_gameState.RollDice(4, 4);
                                    break;
                                case 4:
                                    damage = a_gameState.RollDice(4, 6);
                                    break;
                            }
                            break;
                    }

                    a_target->Hurt(damage + damageModifier);

                    std::cout << "and hits with a " << result << " (" << roll << " + " << modifier << "), dealing " << (damage + damageModifier) << " (" << damage;
                    
                    switch (bladeType) {
                        case BladeType::DAGGER:
                            switch (damageTier) {
                                case 0:
                                    std::cout << " (1d8)";
                                    break;
                                case 1:
                                    std::cout << " (2d4)";
                                    break;
                                case 2:
                                    std::cout << " (1d4 * 2)";
                                    break;
                                case 3:
                                    std::cout << " (3d3)";
                                    break;
                                case 4:
                                    std::cout << " (1d3 * 3)";
                                    break;
                            }
                            break;
                        case BladeType::SHORT_SWORD:
                        case BladeType::LONG_SWORD:
                            switch (damageTier) {
                                case 0:
                                    std::cout << " (1d12)";
                                    break;
                                case 1:
                                    std::cout << " (1d16)";
                                    break;
                                case 2:
                                    std::cout << " (2d8)";
                                    break;
                                case 3:
                                    std::cout << " (4d4)";
                                    break;
                                case 4:
                                    std::cout << " (4d6)";
                                    break;
                            }
                            break;
                    }
                    if (damageModifier > 0) {
                        std::cout << " + " << damageModifier;
                    } else if (damageModifier < 0) {
                        
                        std::cout << " - " << -damageModifier;
                    }
                    std::cout << ").\n" << std::endl;
                } else {
                    std::cout << "and misses with a " << result << " (" << roll << " + " << modifier << ").\n" << std::endl;
                }

                EatInput();

                if (player != nullptr && a_gameState.rooms[a_gameState.curRoom].LivingInhabitants() <= 1 || a_target->curHP > 0 || a_gameState.RollDice(1, 3) != 1) {
                    ++player->usedTurns;
                }
            }
        }
    },
    {
        "Call of the Void",  Action {
            "Call of the Void",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_target != a_caster && a_target->curHP > 0 && a_caster->curMana > 15; 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                std::cout << "The " << a_caster->name << " murmers a cursed incantation ";
                int32_t modifier = 
                    a_caster->GetSkillModifier("Persuasion") + 
                    a_caster->GetSkillModifier("Knowledge of Death") + 
                    a_caster->GetSkillModifier("Necromancy");
                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                size_t opponents = 1;
                if (player != nullptr) {
                    for (size_t i = 0; i < a_gameState.player.items.size(); ++i) { 
                        if (a_gameState.player.equipped[i] && ITEM_DATA[a_gameState.player.items[i].itemID].tags.contains("Necromantic Focus")) {
                            modifier += ITEM_DATA[a_gameState.player.items[i].itemID].tags.at("Necromantic Focus");
                        }
                    }

                    opponents = a_gameState.rooms[a_gameState.curRoom].LivingInhabitants();
                }
                int32_t roll = a_gameState.RollDice(1, 20);
                int32_t result = roll + modifier;
                if (roll == 20 || result >= a_target->GetEffectiveArmor()) {
                    int32_t damage = 0;
                    int32_t damageModifier = (int32_t)(modifier + opponents);

                    damage += a_gameState.RollDice(2, 4) + a_gameState.RollDice(1, 6);

                    a_target->Hurt(damage + damageModifier);

                    std::cout << "and a torrent of whispers comes forth from the ether with a " << result << " (" << roll << " + " << modifier << "), dealing " << (damage + damageModifier) << " (" << damage << " (2d4 + 1d6)"; 
                    if (damageModifier > 0) {
                        std::cout << " + " << damageModifier;
                    } else if (damageModifier < 0) {
                        std::cout << " - " << -damageModifier;
                    }
                    std::cout << ").\n" << std::endl;
                    EatInput();

                    if (player != nullptr) {
                        for (size_t i = 0; i < a_gameState.rooms[a_gameState.curRoom].inhabitants.size(); ++i) {
                            if (a_gameState.rooms[a_gameState.curRoom].inhabitants[i].curHP > 0) {
                                std::cout << "The " << a_gameState.rooms[a_gameState.curRoom].inhabitants[i].name << " twitches, and then ";
                                switch (a_gameState.RollDice(1, 3)) {
                                    case 1:
                                        a_gameState.rooms[a_gameState.curRoom].inhabitants[i].Hurt((int32_t)(a_gameState.rooms[a_gameState.curRoom].inhabitants[i].curHP * 0.5));
                                        std::cout << "experiences a violent shudder, their muscles contracting and flailing with seemingly unatural origin." << std::endl;
                                        break;
                                    case 2:
                                    case 3:
                                        std::cout << "locks up completely, color itself draining from the area around them." << std::endl;
                                        a_gameState.rooms[a_gameState.curRoom].inhabitants[i].stunned = true;
                                        break;
                                }
                            } else {
                                std::cout << "The corpse of the " << a_gameState.rooms[a_gameState.curRoom].inhabitants[i].name << " twitches, and the player feels a surge of ";
                                if (a_caster->curHP < a_caster->maxHP) {
                                    std::cout << "life rush into them." << std::endl;
                                    a_caster->Heal((int32_t)(a_gameState.rooms[a_gameState.curRoom].inhabitants[i].maxHP * 0.15));
                                } else if (a_caster->curMana < a_caster->maxMana) {
                                    std::cout << "mana rush into them." << std::endl;
                                    a_caster->RegainMana((int32_t)(a_gameState.rooms[a_gameState.curRoom].inhabitants[i].maxHP * 0.15));
                                }
                            }

                            EatInput();
                        }
                    } else {
                        if (a_gameState.player.curHP > 0) {
                            std::cout << "The " << a_gameState.player.name << " twitches, and then ";
                            switch (a_gameState.RollDice(1, 3)) {
                                case 1:
                                    a_gameState.player.Hurt((int32_t)(a_gameState.player.curHP * 0.5));
                                    std::cout << "experiences a violent shudder, their muscles contracting and flailing with seemingly unatural origin." << std::endl;
                                    break;
                                case 2:
                                case 3:
                                    std::cout << "locks up completely, color itself draining from the area around them." << std::endl;
                                    a_gameState.player.usedTurns += 1;
                                    break;
                            }
                        } else {
                            std::cout << "The corpse of the " << a_gameState.player.name << " twitches, and the player feels a surge of ";
                            if (a_caster->curHP < a_caster->maxHP) {
                                std::cout << "life rush into them." << std::endl;
                                a_caster->Heal((int32_t)(a_gameState.player.maxHP * 0.15));
                            } else if (a_caster->curMana < a_caster->maxMana) {
                                std::cout << "mana rush into them." << std::endl;
                                a_caster->RegainMana((int32_t)(a_gameState.player.maxHP * 0.15));
                            }
                        }

                        EatInput();
                    }
                } else {
                    std::cout << "and misses with a " << result << " (" << roll << " + " << modifier << ").\n" << std::endl;
                    EatInput();
                }

                if (player != nullptr) {
                    ++player->usedTurns;
                }

                a_caster->DrainMana(15);
                a_caster->manaSickness += 1;
            }
        }
    },
    {
        "Mark for Death",  Action {
            "Mark for Death",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_target != a_caster && a_target->curHP > 0 && a_caster->curMana > 20; 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                std::cout << "The player gestures a wicked glyph in the air ";
                int32_t modifier = 
                    a_caster->GetSkillModifier("Persuasion") + 
                    a_caster->GetSkillModifier("Knowledge of Death") + 
                    a_caster->GetSkillModifier("Necromancy");
                for (size_t i = 0; i < a_gameState.player.items.size(); ++i) { 
                    if (a_gameState.player.equipped[i] && ITEM_DATA[a_gameState.player.items[i].itemID].tags.contains("Necromantic Focus")) {
                        modifier += ITEM_DATA[a_gameState.player.items[i].itemID].tags.at("Necromantic Focus");
                    }
                }
                int32_t roll = a_gameState.RollDice(1, 20);
                int32_t result = roll + modifier;
                if (roll == 20 || result >= a_target->GetEffectiveArmor()) {
                    NPCData* npc = dynamic_cast<NPCData*>(a_target);
                    if (npc != nullptr) {
                        std::cout << "and a dull thud rings out as the color drains away from the " << npc->name << "." << std::endl;
                        npc->onDeath.push_back(
                            [](GameState& a_gameState, NPCData& a_npc) {
                                a_gameState.player.xp += a_npc.curMana;
                                a_gameState.player.RegainMana(a_npc.xp);
                                std::cout << "The corpse of the " << a_npc.name << " violently shudders before a strange burst of mana rips away and surges into the player." << std::endl;
                            }
                        );
                    }
                } else {
                    std::cout << "and misses with a " << result << " (" << roll << " + " << modifier << ").\n" << std::endl;
                }

                EatInput();

                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                if (player != nullptr) {
                    ++player->usedTurns;
                }

                a_caster->DrainMana(20);
                a_caster->manaSickness += 1;
            }
        }
    },
    {
        "Bone Shield",  Action {
            "Bone Shield",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_target->curHP > 0 && a_caster->curMana > 10 && !a_target->timedEffects.contains("Bone Shield."); 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                std::cout << "The player raises their hands, and forward burst bones, creating a protective carapace around ";
                
                a_target->armor += 2;
                a_target->timedEffects.emplace("Bone Shield", STANDARD_TIMED_EFFECTS.at("Bone Shield"));

                if (a_caster == a_target) {
                    std::cout << "themselves." << std::endl;
                } else {
                    NPCData* npc = dynamic_cast<NPCData*>(a_target);
                    if (npc != nullptr) {
                        std::cout << "the " << npc->name << "." << std::endl;
                    }
                }

                EatInput();

                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                if (player != nullptr) {
                    ++player->usedTurns;
                }

                a_caster->DrainMana(10);
                a_caster->manaSickness += 1;
            }
        }
    },
    {
        "Garden of Rot",  Action {
            "Garden of Rot",
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { 
                return a_caster->curMana > 45; 
            },
            [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {
                std::cout << "The player crushes a heart, and from the ground a flower bursts from the center of the room." << std::endl;
                int32_t modifier = 
                    a_caster->GetSkillModifier("Persuasion") + 
                    a_caster->GetSkillModifier("Knowledge of Death") + 
                    a_caster->GetSkillModifier("Necromancy");
                for (size_t i = 0; i < a_gameState.player.items.size(); ++i) { 
                    if (a_gameState.player.equipped[i] && ITEM_DATA[a_gameState.player.items[i].itemID].tags.contains("Necromantic Focus")) {
                        modifier += GameData::ITEM_DATA[a_gameState.player.items[i].itemID].tags.at("Necromantic Focus");
                    }
                }
                size_t opponents = a_gameState.rooms[a_gameState.curRoom].LivingInhabitants();
                int32_t roll = a_gameState.RollDice(1, 20);
                int32_t result = roll + modifier;

                EatInput();

                PlayerData* player = dynamic_cast<PlayerData*>(a_caster);
                if (player != nullptr) {
                    ++player->usedTurns;
                }

                a_caster->DrainMana(45);
                a_caster->manaSickness += 1;
            }
        }
    },
};

std::unordered_map<std::string, ClassData> GameData::CLASSES = {
    {
        "Fighter", ClassData {
            "Fighter",
            "Skill with the blade.",
            {
                ClassLevel {
                    [](GameState& a_gameState, PlayerData& a_player) {
                        std::cout << "Have at least 10 xp.";
                    },
                    [](GameState& a_gameState, PlayerData& a_player) {
                        return a_player.xp >= 10;
                    },
                    {
                        [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                            if (!a_free) {
                                a_player.xp -= 10;
                            }
                            if (std::find(a_player.actions.begin(), a_player.actions.end(), STANDARD_ACTIONS.at("Swift Strike")) == a_player.actions.end()) {
                                a_player.actions.push_back(STANDARD_ACTIONS.at("Swift Strike"));
                            }
                        }
                    }
                },
                ClassLevel {
                    [](GameState& a_gameState, PlayerData& a_player) {
                        std::cout << "Have at least 7 xp.";
                    },
                    [](GameState& a_gameState, PlayerData& a_player) {
                        return a_player.xp >= 7;
                    },
                    {
                        [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                            if (!a_free) {
                                a_player.xp -= 7;
                            }
                            a_player.turns += 1;
                        }
                    }
                },
                ClassLevel {
                    [](GameState& a_gameState, PlayerData& a_player) {
                        std::cout << "Have at least 18 xp.";
                    },
                    [](GameState& a_gameState, PlayerData& a_player) {
                        return a_player.xp >= 18;
                    },
                    {
                        [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                            if (!a_free) {
                                a_player.xp -= 18;
                            }
                            a_player.curHP += 15;
                            a_player.maxHP += 15;
                        }
                    }
                },
            }
        }
    },
    {
        "Necromancer", ClassData {
            "Necromancer",
            "The dead are at your beck and call.",
            {
                ClassLevel {
                    [](GameState& a_gameState, PlayerData& a_player) {
                        std::cout << "Have at least 10 xp and 75 maximum mana.";
                    },
                    [](GameState& a_gameState, PlayerData& a_player) {
                        return a_player.xp >= 10 && a_player.maxMana >= 75;
                    },
                    {
                        [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                            if (!a_free) {
                                a_player.xp -= 10;
                            }
                            if (std::find(a_player.actions.begin(), a_player.actions.end(), GameData::STANDARD_ACTIONS.at("Call of the Void")) == a_player.actions.end()) {
                                a_player.actions.push_back(const_cast<const Action&>(GameData::STANDARD_ACTIONS.at("Call of the Void")));
                            }
                        }
                    }
                },
                ClassLevel {
                    [](GameState& a_gameState, PlayerData& a_player) {
                        std::cout << "Have at least 5 xp and 75 maximum mana.";
                    },
                    [](GameState& a_gameState, PlayerData& a_player) {
                        return a_player.xp >= 5 && a_player.maxMana >= 75;
                    },
                    {
                        [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                            if (!a_free) {
                                a_player.xp -= 5;
                            }
                            a_player.maxMana += 15;
                        }
                    }
                },
                ClassLevel {
                    [](GameState& a_gameState, PlayerData& a_player) {
                        std::cout << "Have at least 35 xp and 75 maximum mana.";
                    },
                    [](GameState& a_gameState, PlayerData& a_player) {
                        return a_player.xp >= 35 && a_player.maxMana >= 75;
                    },
                    {
                        [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                            if (!a_free) {
                                a_player.xp -= 35;
                            }
                            a_player.maxMana += 10;
                            if (std::find(a_player.actions.begin(), a_player.actions.end(), GameData::STANDARD_ACTIONS.at("Mark for Death")) == a_player.actions.end()) {
                                a_player.actions.push_back(GameData::STANDARD_ACTIONS.at("Mark for Death"));
                            }
                        }
                    }
                },
            }
        }
    },
    {
        "Death Knight", ClassData {
            "Death Knight",
            "Sword and bone meet flesh yet unsundered.",
            {
                ClassLevel {
                    [](GameState& a_gameState, PlayerData& a_player) {
                        std::cout << "Have at least 30 xp and " << ((a_player.perks.test((size_t)Perks::ARCANE_EYES) || a_player.LevelInClass("Necromancer") > 0) ? "at least 1 level in Necromancer and at least 1 level in Fighter" : "[UNDECIPHERABLE]") << ".";
                    },
                    [](GameState& a_gameState, PlayerData& a_player) {
                        return a_player.xp >= 30 && a_player.LevelInClass("Necromancer") >= 1 && a_player.LevelInClass("Fighter") >= 1;
                    },
                    {
                        [](GameState& a_gameState, PlayerData& a_player, bool a_free) {
                            if (!a_free) {
                                a_player.xp -= 30;
                            }
                            if (std::find(a_player.actions.begin(), a_player.actions.end(), STANDARD_ACTIONS.at("Bone Shield")) == a_player.actions.end()) {
                                a_player.actions.push_back(STANDARD_ACTIONS.at("Bone Shield"));
                            }
                        }
                    }
                },
            }
        }
    },
};

std::unordered_map<std::string, TimedEffect> GameData::STANDARD_TIMED_EFFECTS = {
    {
        "Bone Shield", TimedEffect {
            1,
            nullptr,
            [](GameState& a_gameState, EntityData& a_entityData) {
                a_entityData.armor -= 2;
            }
        }
    },
    {
        "Decay", TimedEffect {
            4,
            [](GameState& a_gameState, EntityData& a_entityData) {
                a_entityData.Hurt(a_gameState.RollDice(1, 4));
            },
            nullptr
        }
    },
};

std::vector<RoomData> GameData::ROOM_DATA = {
    RoomData("Sanctuary", "A strange room with a strange promise.",
        [](GameState& a_gameState, const RoomData& a_roomData) { return 0; },
        false, 0, 0, 2, 4
    ),
    RoomData("Simple", "",
        [](GameState& a_gameState, const RoomData& a_roomData) { return 35; }, 
        true, 0, 1, 1, 3
    ),
    RoomData("Loopback", "",
        [](GameState& a_gameState, const RoomData& a_roomData) { return a_gameState.usedRooms > 8 ? 15 : 0; }, 
        true, 1, 3, 0, 2
    ),
    RoomData("Downfall", "",
        [](GameState& a_gameState, const RoomData& a_roomData) { return (a_gameState.curRoom % 6) > 3 ? 10 : 0; }, 
        false, 0, 1, 2, 4
    ),
    RoomData("Lotus", "The room branches into many different corridors, seemingly into new domains.",
        [](GameState& a_gameState, const RoomData& a_roomData) { return 20; }, 
        true, 0, 0, 2, 6
    ),
    RoomData("Sewer", "",
        [](GameState& a_gameState, const RoomData& a_roomData) { return (a_roomData.minNew >= 2 || a_roomData.backlink) ? 8 : 0; }, 
        true, 0, 0, 2, 4, 
        ConnectionTest {
            [](GameState& a_gameState, RoomInstance& a_destination) {
                return a_destination.flags.contains("OpenDoor") || a_gameState.player.HasItem(4, 1);
            },
            [](GameState& a_gameState, RoomInstance& a_destination, bool a_success) {
                if (!a_success) {
                    std::cout << "You require a key to get through the grate." << std::endl;
                }
            },
            [](GameState& a_gameState, RoomInstance& a_destination) {
                if (!a_destination.flags.contains("OpenDoor")) {
                    a_gameState.player.UseItemOf(4);
                    a_destination.flags.emplace("OpenDoor", 1);
                    std::cout << "You use one sewer key to open the grate." << std::endl;
                }
            }
        }, 
        ConnectionTest()
    ),
};

std::vector<ItemData> GameData::ITEM_DATA = {
    ItemData("Bean", "A single bean, you may only have 0b11111111 beans.", 
        0b11111111
    ), // 0
    ItemData("HP Potion", "A HP Potion, restores 20 HP.", {
            {"Consumable", 0},
            {"Potion"    , 0},
            {"HPRegain"  , 0},
        }, ItemUsage {
            "Drink",
            [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                return a_gameState.player.curHP < a_gameState.player.maxHP;
            },
            [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                a_gameState.player.Heal(20);
                a_gameState.player.UseItemAt(index);
            }
        }, 100
    ), // 1
    ItemData("Mana Potion", "A Mana Potion, restores 20 Mana.", {
            {"Consumable", 0},
            {"Potion"    , 0},
            {"ManaRegain", 0},
        }, ItemUsage {
            "Drink",
            [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                return a_gameState.player.curMana < a_gameState.player.maxMana;
            },
            [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                a_gameState.player.RegainMana(20);
                a_gameState.player.UseItemAt(index);
            }
        }, 100
    ), // 2
    ItemData("Dungeon Key", "An old key, goes to something important.", {
            {"Key", 0},
        }, 15
    ), // 3
    ItemData("Sewer Key", "A mucky key, might be useful.", {
            {"Key", 0},
        }, 15
    ), // 4
    ItemData("Long Sword", "A long sword, good in skilled hands.", {
            {"Blade", (int32_t)BladeType::LONG_SWORD},
        }, 1
    ), // 5
    ItemData("Short Sword", "A short sword, good in skilled hands.", {
            {"Blade", (int32_t)BladeType::SHORT_SWORD},
        }, 1
    ), // 6
    ItemData("Dagger", "A small dagger, good in skilled hands.", {
            {"Blade", (int32_t)BladeType::DAGGER},
        }, 1
    ), // 7
    ItemData("Ritual Dagger", "An ornate dagger, bleeding red metals comprise it's form.", {
            {"Blade", (int32_t)BladeType::DAGGER},
        }, 1
    ), // 8
    ItemData("Ars Puppetarii Mortui", "A grimoire containing some of the most abhorred gramrye known to mortals. It is woven from skin, but of what, you do not know.", {
            {"Tome"             , 0},
            {"Necromantic Focus", 5},
        }, ItemUsage {
            "Study",
            [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                return a_gameState.player.xp > 8;
            },
            [](GameState& a_gameState, const ItemStack& a_itemStack, size_t index = -1) {
                std::cout << "You flip through the pages of this wretched tome..." << std::endl;
                int32_t xpCost = a_gameState.RollDice(1, 8);
                switch (a_gameState.RollDice(1, 20)) {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        a_gameState.player.RegainMana(xpCost * a_gameState.RollDice(1, 4));
                        std::cout << "You feel your mana rejuvinate." << std::endl;
                        break;
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                        a_gameState.player.Heal(xpCost * a_gameState.RollDice(1, 4));
                        std::cout << "You feel your flesh knit together." << std::endl;
                        break;
                    case 15:
                    case 16:
                    case 17:
                        a_gameState.player.AddSkillModifier("Knowledge of Death", xpCost);
                        std::cout << "You feel your understanding of death increase." << std::endl;
                        break;
                    case 18:
                    case 19:
                        a_gameState.player.armor += 1;
                        std::cout << "You feel your skin calcify." << std::endl;
                        break;
                    case 20:
                        a_gameState.player.Hurt(xpCost * a_gameState.RollDice(1, 3));
                        std::cout << "You feel your lungs collapse, before ancient air refills them... Something has changed." << std::endl;
                        break;
                }
                a_gameState.player.xp -= xpCost;
            }
        }, 1
    ), // 9
    ItemData("Chainmail Armor", "Simple chain link armor for protection.", {
            {"Armor", 2},
        }, 1
    ), // 10
};

std::unordered_map<std::string, ShopGenerationRule> GameData::SHARED_GENERATION_RULES = {
    {
        "Standard Potions", ShopGenerationRule {
            [](GameState& a_gameState) {
                return true;
            },
            [](GameState& a_gameState, ShopInstance& a_shop) {
                a_shop.catalog.push_back(ShopCatalog { 5, ItemStack(1, 4 + a_gameState.RollDice(1, 16)) });
                a_shop.catalog.push_back(ShopCatalog { 5, ItemStack(2, 4 + a_gameState.RollDice(1, 16)) });
            }
        }
    },
    {
        "Standard Weaponry", ShopGenerationRule {
            [](GameState& a_gameState) {
                return a_gameState.RollDice(1, 6) > 1;
            },
            [](GameState& a_gameState, ShopInstance& a_shop) {
                switch (a_gameState.RollDice(1, 3)) {
                    case 1:
                        a_shop.catalog.push_back(ShopCatalog { 10, ItemStack(5, 1) });
                        break;
                    case 2:
                        a_shop.catalog.push_back(ShopCatalog { 10, ItemStack(6, 1) });
                        break;
                    case 3:
                        a_shop.catalog.push_back(ShopCatalog {  5, ItemStack(7, 1) });
                        break;
                }
                if (a_gameState.RollDice(1, 4) == 1) {
                    a_shop.catalog.back().stack.metadata.emplace("ToHit", a_gameState.RollDice(1, 6));
                }
            }
        }
    },
    {
        "Standard Keys", ShopGenerationRule {
            [](GameState& a_gameState) {
                return a_gameState.RollDice(1, 6) > 4;
            },
            [](GameState& a_gameState, ShopInstance& a_shop) {
                a_shop.catalog.push_back(ShopCatalog { 3, ItemStack(4, a_gameState.RollDice(2, 4)) });
            }
        }
    },
};

std::unordered_map<std::string, ShopData> GameData::SHOP_DATA = {
    {
        "Barterer", ShopData {
            "Barterer",
            {
                SHARED_GENERATION_RULES.at("Standard Potions"),
                SHARED_GENERATION_RULES.at("Standard Weaponry"),
                SHARED_GENERATION_RULES.at("Standard Keys"),
            }
        }
    },
};

std::unordered_map<std::string, RoomAction> GameData::ROOM_ACTIONS = {
    {
        "Treasure Trove", RoomAction {
            "Treasure Trove",
            "A pile of riches.",
            [](GameState& a_gameState) {
                std::cout << (a_gameState.rooms[a_gameState.curRoom].flags.contains("TreasureTrove_Looted") ? "- There isn't even a single coin that remains.\n" : "- There is a glittering pile of riches.\n");
            },
            [](GameState& a_gameState) {
                return 
                    a_gameState.rooms[a_gameState.curRoom].flags.contains("TreasureTrove_Count") && 
                    a_gameState.rooms[a_gameState.curRoom].flags.contains("TreasureTrove_Dice") &&
                    !a_gameState.rooms[a_gameState.curRoom].flags.contains("TreasureTrove_Looted");
            },
            [](GameState& a_gameState) {
                std:: cout << "We are in." << std::endl;
                std::cout << std::format("Running with {}d{}", a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Count"), a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Dice")) << std::endl;
                a_gameState.rooms[a_gameState.curRoom].flags.emplace("TreasureTrove_Looted", 0);
                int32_t goldAmount = a_gameState.RollDice(a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Count"), a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Dice"));
                a_gameState.player.gold += goldAmount;
                std::cout << "You find " << goldAmount << " (" << a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Count") << "d" << a_gameState.rooms[a_gameState.curRoom].flags.at("TreasureTrove_Dice") << ") gold." << std::endl;
                EatInput();
            }
        }
    },
    {
        "Shop", RoomAction {
            "Shop",
            "Shop at the shop.",
            [](GameState& a_gameState) {
                std::cout << "- A barterer is humming away in their humble shop.\n";
            },
            [](GameState& a_gameState) {
                return true;
            },
            [](GameState& a_gameState) {
                size_t option = 0;
                while (option != 3) {
                    ShopInstance* shop = ((ShopInstance*)a_gameState.rooms[a_gameState.curRoom].data.at("Shop"));
                    std::cout << "-------------------------------\n\x1b[1m" << shop->shopName << "\x1b[22m\n\nOptions:\n1) Buy\n2) Sell\n3) Exit\n\nOption: ";
                    SafeInput<size_t>(option);
                    switch (option) {
                        case 1:
                            std::cout << "-------------------------------\nGold: " << a_gameState.player.gold << "\n\nCatalog:\n";
                            for (size_t i = 0; i < shop->catalog.size(); ++i) {
                                const ItemData& itemType = ITEM_DATA[shop->catalog[i].stack.itemID];
                                std::cout << (i + 1) << ") " << itemType.name << " (" << shop->catalog[i].cost << ") x" << shop->catalog[i].stack.stackSize << "\n";
                            }
                            std::cout << (shop->catalog.size() + 1) << ") Back\n\nOption: ";
                            SafeInput<size_t>(option);

                            if (--option < shop->catalog.size()) {
                                if (shop->catalog[option].cost <= a_gameState.player.gold) {
                                    a_gameState.player.gold -= (int32_t)shop->catalog[option].cost;
                                    a_gameState.player.AddItem(shop->catalog[option].stack.itemID, 1);
                                    std::cout << "You bought 1 " << ITEM_DATA[shop->catalog[option].stack.itemID].name << " for " << shop->catalog[option].cost << " gold." << std::endl;
                                    if (--shop->catalog[option].stack.stackSize <= 0) {
                                        shop->catalog.erase(shop->catalog.begin() + option);
                                    }
                                } else {
                                    std::cout << "You do not have enough gold." << std::endl;
                                }
                            }
                            option = 0;
                            break;
                        case 2:
                            std::cout << "They do not wish to buy anything from you..." << std::endl;
                            break;
                    }
                }
            }
        }
    }
};

std::unordered_map<std::string, EntityTemplate> GameData::ENTITY_TEMPLATES = {};

std::unordered_map<std::string, NPCTemplate> GameData::NPC_TEMPLATES = {
    {
        "Shaman", NPCTemplate {
            12,
            8,
            [](GameState& a_gameState, NPCData& a_npc) {
                size_t healed = 0;
                if (a_npc.curMana > 10 && a_gameState.RollDice(1, 2) == 1) {
                    for (NPCData& npc : a_gameState.rooms[a_gameState.curRoom].inhabitants) {
                        if (npc.curHP < npc.maxHP) {
                            npc.Heal(a_gameState.RollDice(2, 4));
                            ++healed;
                        }
                    }
                }

                if (healed == 0) {
                    std::cout << "The " << a_npc.name << " swings their staff at the player and ";
                    if (a_gameState.RollDice(1, 20) >= a_gameState.player.GetEffectiveArmor()) {
                        int32_t damage = a_gameState.RollDice(1, 4);
                        a_gameState.player.Hurt(damage);
                        std::cout << "hits, dealing " << damage << " damage." << std::endl;
                    } else {
                        std::cout << "misses." << std::endl;
                    }
                } else {
                    a_npc.manaSickness += 1;
                    a_npc.DrainMana(10);
                    std::cout << "The " << a_npc.name << " murmurs under their breath; as they do this the opponents are engufled in a pale green mist, which quickly dissipates, and " << healed << " of the opponents seem rejuvinated." << std::endl;
                }
            }
        }
    },
    {
        "Grunt", NPCTemplate {
            4,
            6,
            [](GameState& a_gameState, NPCData& a_npc) {
                std::cout << "The " << a_npc.name << " swings their club at the player and ";
                if (a_gameState.RollDice(1, 20) >= a_gameState.player.GetEffectiveArmor()) {
                    int32_t damage = a_gameState.RollDice(1, 4);
                    a_gameState.player.Hurt(damage);
                    std::cout << "hits, dealing " << damage << " damage." << std::endl;
                } else {
                    std::cout << "misses." << std::endl;
                }
            }
        }, // 1
    },
    {
        "Squire", NPCTemplate {
            6,
            6,
            [](GameState& a_gameState, NPCData& a_npc) {
                std::cout << "The " << a_npc.name << " swings their shortsword at the player and ";
                int32_t modifier = 
                    a_npc.GetSkillModifier("Martial Combat") +
                    a_npc.GetSkillModifier("Swordsmanship");
                if (a_gameState.RollDice(1, 20) + modifier >= a_gameState.player.GetEffectiveArmor()) {
                    int32_t damage = a_gameState.RollDice(1, 6) + (int32_t)(modifier * 0.5);
                    a_gameState.player.Hurt(damage);
                    std::cout << "hits, dealing " << damage << " damage." << std::endl;
                } else {
                    std::cout << "misses." << std::endl;
                }
            }
        }
    },
    {
        "Priest", NPCTemplate {
            15,
            10,
            [](GameState& a_gameState, NPCData& a_npc) {
                size_t healed = 0;
                if (a_npc.curMana > 10 && a_gameState.RollDice(1, 2) == 1) {
                    for (NPCData& npc : a_gameState.rooms[a_gameState.curRoom].inhabitants) {
                        if (npc.curHP < npc.maxHP) {
                            npc.Heal(a_gameState.RollDice(3, 3));
                            ++healed;
                        }
                    }
                }

                if (healed == 0) {
                    int32_t roll = a_gameState.RollDice(1, 20);
                    int32_t modifier = 0;
                    if (a_npc.curMana > 5 && a_gameState.RollDice(1, 3) == 1) {
                        std::cout << "The " << a_npc.name << " raises their arms in prayer and a swarm of pebbles sling towards the player and they ";
                        modifier += a_npc.GetSkillModifier("Geomancy");
                        a_npc.DrainMana(5);
                        if (roll + modifier >= a_gameState.player.GetEffectiveArmor()) {
                            int32_t damage = a_gameState.RollDice(2, 4);
                            a_gameState.player.Hurt(damage);
                            std::cout << "hit, dealing " << damage << " damage." << std::endl;
                        } else {
                            std::cout << "miss." << std::endl;
                        }
                    } else {
                        std::cout << "The " << a_npc.name << " swings their staff at the player and ";
                        modifier += a_npc.GetSkillModifier("Martial Combat");
                        if (roll + modifier >= a_gameState.player.GetEffectiveArmor()) {
                            int32_t damage = 2 + a_gameState.RollDice(1, 4);
                            a_gameState.player.Hurt(damage);
                            std::cout << "hits, dealing " << damage << " damage." << std::endl;
                        } else {
                            std::cout << "misses." << std::endl;
                        }
                    }
                } else {
                    a_npc.manaSickness += 1;
                    a_npc.DrainMana(10);
                    std::cout << "The " << a_npc.name << " murmurs under their breath; as they do this the opponents are engufled in a pale green mist, which quickly dissipates, and " << healed << " of the opponents seem rejuvinated." << std::endl;
                }
            }
        }
    },
    {
        "Reaper", NPCTemplate {
            20,
            20,
            [](GameState& a_gameState, NPCData& a_npc) {
                size_t healed = 0;
                size_t amount = 0;
                if (a_npc.curMana > 10 && a_gameState.RollDice(1, 2) == 1) {
                    for (NPCData& npc : a_gameState.rooms[a_gameState.curRoom].inhabitants) {
                        if (npc.curHP < npc.maxHP) {
                            size_t healAmount = a_gameState.RollDice(3, 3);
                            npc.Heal((int32_t)healAmount);
                            amount += healAmount;
                            ++healed;
                        }
                    }
                }

                if (healed > 0) {
                    a_npc.manaSickness += 1;
                    a_npc.DrainMana(10);
                    a_gameState.player.Hurt((int32_t)amount);
                    std::cout << "The " << a_npc.name << " silently holds vigil, and " << healed << " stand taller as you feel your life being pulled out of you... You take " << amount <<  " damage." << std::endl;
                }

                std::cout << "The " << a_npc.name << " swings their scythe at the player and ";
                if (a_gameState.RollDice(1, 20) + 4 >= a_gameState.player.GetEffectiveArmor()) {
                    int32_t damage = a_gameState.RollDice(2, 8) + 3;
                    a_gameState.player.Hurt(damage);
                    std::cout << "hits, dealing " << damage << " damage." << std::endl;
                } else {
                    std::cout << "misses." << std::endl;
                }
            }
        }
    },
};

std::unordered_map<std::string, std::function<NPCData(NPCData)>> GameData::NPC_MODIFIERS = {
    {
        "Grunt", [](NPCData a_npc){
            a_npc.name += " Grunt";
            return a_npc;
        }
    },
    {
        "Shaman", [](NPCData a_npc){
            a_npc.name += " Shaman";
            a_npc.maxMana += 20;
            a_npc.curMana += 20;
            a_npc.manaRegen += 7;
            return a_npc;
        }
    },
    {
        "Squire", [](NPCData a_npc){
            a_npc.name += " Squire";
            a_npc.maxHP += 8;
            a_npc.curHP += 8;
            a_npc.armor += 2;
            a_npc.AddSkillModifier("Martial Combat", 1);
            a_npc.AddSkillModifier("Swordsmanship" , 1);
            return a_npc;
        }
    },
    {
        "Priest", [](NPCData a_npc){
            a_npc.name += " Priest";
            a_npc.maxHP += 6;
            a_npc.curHP += 6;
            a_npc.maxMana += 35;
            a_npc.curMana += 35;
            a_npc.manaRegen += 10;
            a_npc.AddSkillModifier("Martial Combat", 1);
            a_npc.AddSkillModifier("Geomancy", 1);
            return a_npc;
        }
    },
    {
        "Elder", [](NPCData a_npc){
            a_npc.name = "Elder " + a_npc.name;
            a_npc.maxHP += (int32_t)(a_npc.maxHP * 0.5);
            a_npc.curHP += (int32_t)(a_npc.curHP * 0.5);
            a_npc.maxMana += (uint32_t)(a_npc.maxMana * 0.5);
            a_npc.curMana += (uint32_t)(a_npc.curMana * 0.5);
            a_npc.manaRegen += 5;
            for (const std::pair<std::string, int32_t>& skillPair : a_npc.skills) {
                a_npc.AddSkillModifier(skillPair.first, 1);
            }
            return a_npc;
        }
    },
};

std::vector<Encounter> GameData::ENCOUNTERS = {};

