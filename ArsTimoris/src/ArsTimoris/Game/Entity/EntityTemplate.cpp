#include <ArsTimoris/Game/Entity/EntityTemplate.h>
#include <ArsTimoris/Game/GameData.h>
#include <iostream>
#include <fstream>

EntityTemplate::EntityTemplate(void) {
    this->name = "";
    this->hp = this->mana = this->manaRegen = this->armor = 0;
    this->skills = std::unordered_map<std::string, int32_t>();
    this->actions = std::vector<Action>();
}

EntityTemplate::EntityTemplate(std::string_view a_name, int32_t a_hp, int32_t a_mana, int32_t a_manaRegen, int32_t a_armor, const std::unordered_map<std::string, int32_t>& a_skills, const std::vector<Action>& a_actions) {
    this->name      = a_name;
    this->hp        = a_hp;
    this->mana      = a_mana;
    this->manaRegen = a_manaRegen;
    this->armor     = a_armor;
    this->skills    = a_skills;
    this->actions   = a_actions;
}

std::unordered_map<std::string, EntityTemplate> LoadEntityData(std::filesystem::path a_path) {
    std::unordered_map<std::string, EntityTemplate> loadedEntities = std::unordered_map<std::string, EntityTemplate>();
    std::ifstream reader = std::ifstream(a_path);
    char buffer[256];
    std::string stringStorage;
    //int32_t intStorage;
    std::string name;
    
    // ENTITY TEMPLATE
    int32_t hp;
    int32_t mana;
    int32_t manaRegen;
    int32_t armor;
    std::unordered_map<std::string, int32_t> skills = std::unordered_map<std::string, int32_t>();
    std::vector<Action> actions = std::vector<Action>();
    // ENTITY TEMPLATE

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
                mode = 2;
                ++step;
                reader.getline(buffer, 256);
                break;
            case 1:
                hp = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 2:
                mana = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 3:
                manaRegen = std::stoi(buffer);
                mode = 2;
                ++step;
                break;
            case 4:
                armor = std::stoi(buffer);
                mode = 3;
                ++step;
                reader.getline(buffer, 256);
                reader.getline(buffer, 256);
                break;
            case 5:
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
            case 6:
                stringStorage = buffer;
                if (stringStorage == "END") {
                    mode = 0;
                    ++step;
                    reader.getline(buffer, 256);
                    reader.getline(buffer, 256);
                } else {
                    actions.push_back(GameData::STANDARD_ACTIONS.at(stringStorage));
                    mode = 1;
                    reader.getline(buffer, 256);
                }
                break;
            case 7:
                stringStorage = buffer;
                mode = 0;
                ++step;
                loadedEntities.emplace(name, EntityTemplate(name, hp, mana, manaRegen, armor, skills, actions));
                skills = std::unordered_map<std::string, int32_t>();
                actions = std::vector<Action>();
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
    
    return loadedEntities;
}