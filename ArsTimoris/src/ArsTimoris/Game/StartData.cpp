#include <ArsTimoris/Game/StartData.h>
#include <ArsTimoris/Game/Entity/PlayerData.h>
#include <ArsTimoris/Game/GameData.h>
#include <fstream>
#include <iostream>

StartData::StartData(void) {
    this->name = "";
    this->description = "";
    this->playerEntity = EntityTemplate();
    this->classes = std::unordered_map<std::string, ClassInstance>();
    this->perks = std::vector<size_t>();
    this->inventory = std::vector<ItemStack>();
    this->turns = 1;
    this->startRoom = -1;
    this->xp = 0;
    this->gold = 0;
}

StartData::StartData(std::string_view a_name, std::string_view a_description, const EntityTemplate& a_playerEntity, const std::unordered_map<std::string, ClassInstance>& a_classes, const std::vector<size_t>& a_perks, const std::vector<ItemStack>& a_inventory, size_t a_turns, size_t a_startRoom, size_t a_xp, size_t a_gold) {
    this->name = a_name;
    this->description = a_description;
    this->playerEntity = a_playerEntity;
    this->classes = a_classes;
    this->perks = a_perks;
    this->inventory = a_inventory;
    this->turns = a_turns;
    this->startRoom = a_startRoom;
    this->xp = a_xp;
    this->gold = a_gold;
}

std::vector<StartData> LoadStartData(std::filesystem::path a_path) {
    std::vector<StartData> loadedStarts = std::vector<StartData>();
    std::ifstream reader = std::ifstream(a_path);
    char buffer[256];
    std::string stringStorage;
    //int32_t intStorage;
    std::string name;
    std::string description;

    // ENTITY TEMPLATE
    int32_t hp;
    int32_t mana;
    int32_t manaRegen;
    int32_t armor;
    std::unordered_map<std::string, int32_t> skills = std::unordered_map<std::string, int32_t>();
    std::vector<Action> actions = std::vector<Action>();
    // ENTITY TEMPLATE

    std::unordered_map<std::string, ClassInstance> classes = std::unordered_map<std::string, ClassInstance>();
    std::vector<size_t> perks = std::vector<size_t>();
    std::vector<ItemStack> inventory = std::vector<ItemStack>();
    size_t turns;
    size_t startRoom;
    size_t xp;
    size_t gold;

    uint32_t step = 0;
    uint32_t mode = 0;

    bool running = true;

    while (running) {
        switch (mode) {
            case 0:
                reader.getline(buffer, 256);
                break;
            case 1:
                reader.getline(buffer, 256, ',');
                break;
            case 2:
                reader.getline(buffer, 256, ';');
                break;
            case 3:
                reader.getline(buffer, 256, ':');
                break;
            case 4:
                reader.getline(buffer, 256, 'x');
                break;
        }
        //std::cout << std::string(buffer) << std::endl;
        //std::cout << "S & M : " << step << ", " << mode << std::endl;
        switch (step) {
            case 0:
                name = buffer;
                mode = 0;
                ++step;
                break;
            case 1:
                description = buffer;
                mode = 2;
                ++step;
                reader.getline(buffer, 256);
                break;
            case 2:
                hp = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 3:
                mana = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 4:
                manaRegen = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 5:
                armor = std::stoi(buffer);
                mode = 3;
                ++step;
                reader.getline(buffer, 256);
                reader.getline(buffer, 256);
                break;
            case 6:
                if (mode == 3) {
                    stringStorage = buffer;
                    if (stringStorage == "END") {
                        mode = 1;
                        ++step;
                        reader.getline(buffer, 256);
                        reader.getline(buffer, 256);
                    } else {
                        mode = 1;
                    }
                } else if (mode == 1) {
                    skills.insert(std::pair(stringStorage, std::stoi(buffer)));
                    mode = 3;
                    reader.getline(buffer, 256);
                } else {
                    std::cout << "ERROR: mode{" << mode << "} IS NOT VALID" << std::endl;
                    running = false;
                }
                break;
            case 7:
                stringStorage = buffer;
                if (stringStorage == "END") {
                    mode = 3;
                    ++step;
                    reader.getline(buffer, 256);
                    reader.getline(buffer, 256);
                } else {
                    actions.push_back(GameData::STANDARD_ACTIONS.at(stringStorage));
                    mode = 1;
                    reader.getline(buffer, 256);
                }
                break;
            case 8:
                if (mode == 3) {
                    stringStorage = buffer;
                    if (stringStorage == "END") {
                        mode = 1;
                        ++step;
                        reader.getline(buffer, 256);
                        reader.getline(buffer, 256);
                    } else {
                        mode = 1;
                    }
                } else if (mode == 1) {
                    classes.insert(std::pair(stringStorage, ClassInstance(stringStorage, std::stoi(buffer))));
                    mode = 3;
                    reader.getline(buffer, 256);
                } else {
                    std::cout << "ERROR: mode{" << mode << "} IS NOT VALID" << std::endl;
                    running = false;
                }
                break;
            case 9:
                stringStorage = buffer;
                if (stringStorage == "END") {
                    mode = 1;
                    ++step;
                    reader.getline(buffer, 256);
                    reader.getline(buffer, 256);
                } else {
                    /*
                        INSIGHT
                        ARCANE_EYES
                        HARD_HITTER
                        HORDE_SLAYER
                    */
                    perks.push_back((size_t)PerkFromName(stringStorage));
                    mode = 1;
                    reader.getline(buffer, 256);
                }
                break;
            case 10:
                if (mode == 1) {
                    stringStorage = buffer;
                    if (stringStorage == "END") {
                        mode = 2;
                        ++step;
                        reader.getline(buffer, 256);
                        reader.getline(buffer, 256);
                    } else {
                        inventory.push_back(ItemStack::FromString(stringStorage));
                        mode = 1;
                    reader.getline(buffer, 256);
                    }
                } else {
                    std::cout << "ERROR: mode{" << mode << "} IS NOT VALID" << std::endl;
                    running = false;
                }
                break;
            case 11:
                turns = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 12:
                startRoom = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 13:
                xp = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 14:
                gold = std::stoi(buffer);
                mode = 0;
                ++step;
                loadedStarts.push_back(StartData(
                    name,
                    description,
                    EntityTemplate("Player", hp, mana, manaRegen, armor, skills, actions),
                    classes,
                    perks,
                    inventory,
                    turns,
                    startRoom,
                    xp,
                    gold
                ));
                skills = std::unordered_map<std::string, int32_t>();
                actions = std::vector<Action>();
                classes = std::unordered_map<std::string, ClassInstance>();
                perks = std::vector<size_t>();
                inventory = std::vector<ItemStack>();
                reader.getline(buffer, 256);
                reader.getline(buffer, 256);
                break;
            case 15:
                stringStorage = buffer;
                if (stringStorage == "FINAL") {
                    //std::cout << "Finished" << std::endl;
                    running = false;
                } else {
                    step = 0;
                    mode = 0;
                    reader.getline(buffer, 256);
                }
                break;
        }
    }

    reader.close();
    
    return loadedStarts;
}
