#include <ArsTimoris/Game/GameState.h>
#include <ArsTimoris/Util/Random.hpp>
#include <print>

GameState::GameState(std::mt19937 a_generator, const Interpreter& a_interpreter) {
    this->generator = a_generator;
    this->interpreter = a_interpreter;
    this->screen = Screen::TITLE;
    this->menu = Menu::NONE;
    this->usedRooms = 0;
    this->curRoom = 0;
    this->rooms = std::vector<RoomInstance>();
    this->player = PlayerData(EntityTemplate("Player", 100, 30, 5, 10, {}, {Action {"Example", [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) { return true; }, [](GameState& a_gameState, EntityData* a_caster, EntityData* a_target) {}}}));
    this->inputData = InputData{};
    this->messageStack = std::vector<std::string>();
}

void GameState::CacheInputState(void) {
    inputData.oldKeys = (bool*)memcpy(inputData.oldKeys, inputData.keys, inputData.numKeys);
    inputData.oldMods = inputData.mods;
    inputData.oldMouse = inputData.mouse;
    inputData.oldMousePos = inputData.mousePos;
    inputData.mods = SDL_GetModState();
    inputData.mouse = SDL_GetRelativeMouseState(&inputData.mousePos.x, &inputData.mousePos.y);
    
}

void GameState::DisplayStarts(void) {
    for (size_t i = 0; i < GameState::Starts.size(); ++i) {
        std::cout << (i + 1) << ") " << GameState::Starts[i].name << "\n- " << GameState::Starts[i].description << "\n" << std::endl;
    }
}

void GameState::Start(size_t a_start) {
    this->start = a_start;
    this->usedRooms = 0;
    this->curRoom = 0;
    this->rooms.clear();
    this->player = PlayerData(GameState::Starts[a_start].playerEntity, GameState::Starts[a_start].perks, GameState::Starts[a_start].turns);
    for (const std::pair<std::string, ClassInstance>& classPair : GameState::Starts[a_start].classes) {
        for (size_t level = 0; level < classPair.second.level; ++level) {
            for (size_t levelUpEffect = 0; levelUpEffect < GameData::CLASSES.at(classPair.second.classType).levels[level].levelUpEffects.size(); ++levelUpEffect) {
                GameData::CLASSES.at(classPair.second.classType).levels[level].levelUpEffects[levelUpEffect](*this, player, true);
            }
        }
        this->player.classes.insert(std::pair<std::string, ClassInstance>(classPair.first, classPair.second));
    }
    this->player.xp = (int32_t)GameState::Starts[a_start].xp;
    this->player.gold = (int32_t)GameState::Starts[a_start].gold;
    this->player.items = GameState::Starts[a_start].inventory;
    this->player.equipped = std::vector<bool>(player.items.size());
    this->PushBackRoom(GameState::Starts[a_start].startRoom);
}

void GameState::Start(std::string a_startName) {
    int32_t index = -1;
    for (int32_t i = 0; i < GameState::Starts.size(); ++i) {
        if (GameState::Starts[i].name == a_startName) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        std::println("ERROR: {} can not be found as a start.", a_startName);
        throw std::exception();
    }
    this->start = index;
    this->usedRooms = 0;
    this->curRoom = 0;
    this->rooms.clear();
    this->player = PlayerData(GameState::Starts[index].playerEntity, GameState::Starts[index].perks, GameState::Starts[index].turns);
    for (const std::pair<std::string, ClassInstance>& classPair : GameState::Starts[index].classes) {
        for (size_t level = 0; level < classPair.second.level; ++level) {
            for (size_t levelUpEffect = 0; levelUpEffect < GameData::CLASSES.at(classPair.second.classType).levels[level].levelUpEffects.size(); ++levelUpEffect) {
                GameData::CLASSES.at(classPair.second.classType).levels[level].levelUpEffects[levelUpEffect](*this, player, true);
            }
        }
        this->player.classes.insert(std::pair<std::string, ClassInstance>(classPair.first, classPair.second));
    }
    this->player.xp = (int32_t)GameState::Starts[index].xp;
    this->player.gold = (int32_t)GameState::Starts[index].gold;
    this->player.items = GameState::Starts[index].inventory;
    this->player.equipped = std::vector<bool>(player.items.size());
    this->PushBackRoom(GameState::Starts[index].startRoom);
}

size_t GameState::PushBackRoom(size_t a_index) {
    this->rooms.push_back(RoomInstance(a_index, this->usedRooms));
    return this->usedRooms++;
}

int32_t GameState::RollDice(size_t a_amount, size_t a_die) {
    int32_t total = 0;
    std::uniform_int_distribution<uint32_t>& dieDistribution = DICE.at((uint32_t)a_die);
    for (size_t i = 0; i < a_amount; ++i) {
        total += dieDistribution(generator);
    }
    return total;
}

size_t GameState::GetRandomRoom(const RoomData& a_roomData) {
    size_t maximum = 0;
    size_t weight = 0;
    std::vector<int64_t> weights = std::vector<int64_t>();

    for (const RoomData& roomData : GameData::ROOM_DATA) {
        if (roomData.weight != nullptr && (weight = roomData.weight(*this, a_roomData)) > 0) {
            maximum += weight;
            weights.push_back(weight);
        } else {
            weights.push_back(-1);
        }
    }

    int64_t choice = static_cast<int64_t>(PERCENT_DISTRIBUTION(generator) * maximum);

    for (size_t i = 0; i < GameData::ROOM_DATA.size(); ++i) {
        if (weights[i] != -1 && (choice -= weights[i]) <= 0) {
            return i;
        }
    }

    return (size_t)0;
};

size_t GameState::GetRandomEncounter(void) {
    size_t maximum = 0;
    size_t weight = 0;
    std::vector<int64_t> weights = std::vector<int64_t>();

    for (Encounter encounter : GameData::ENCOUNTERS) {
        if (encounter.weight != "{}" && (weight = interpreter.ParseStatement<int32_t>(*this, RegisterType::INT, encounter.weight)) > 0) {
            maximum += weight;
            weights.push_back(weight);
            //std::cout << std::format("Encounter {:0>2} {:0>4}", weights.size(), weight) << std::endl;
        } else {
            weights.push_back(-1);
            //std::cout << std::format("Encounter {:0>2} {:0>4}", weights.size(), 0) << std::endl;
        }
    }

    int64_t choice = static_cast<int64_t>(PERCENT_DISTRIBUTION(generator) * maximum);

    for (size_t i = 0; i < GameData::ENCOUNTERS.size(); ++i) {
        if (weights[i] != -1 && (choice -= weights[i]) <= 0) {
            return i;
        }
    }

    return (size_t)0;
};

void GameState::InitializeRoom(RoomInstance* const a_roomInstance) {
    a_roomInstance->initialized = true;
    std::vector<ConnectionInstance> newConnections = a_roomInstance->connections;
    size_t newRoomThing = 0;
    const RoomData& roomType = GameData::ROOM_DATA[a_roomInstance->roomID];
    size_t roomIndex = a_roomInstance->roomIndex;
    size_t newRoomCount = roomType.minNew + (size_t)(PERCENT_DISTRIBUTION(generator) * (roomType.maxNew - roomType.minNew));
    for (size_t i = 0; i < std::min<size_t>(roomType.minExisting + (size_t)(PERCENT_DISTRIBUTION(generator) * (roomType.maxExisting - roomType.minExisting)), usedRooms); ++i) {
        newConnections.push_back(
            ConnectionInstance {
                (newRoomThing = size_t(PERCENT_DISTRIBUTION(generator) * usedRooms)),
                GameData::ROOM_DATA[rooms[newRoomThing].roomID].toConnection,
                roomType.fromConnection
            }
        );
    }
    for (size_t i = 0; i < newRoomCount; ++i) {
        newConnections.push_back(
            ConnectionInstance {
                (newRoomThing = PushBackRoom(GetRandomRoom(roomType))),
                GameData::ROOM_DATA[rooms[newRoomThing].roomID].toConnection,
                roomType.fromConnection
            }
        );

        if (GameData::ROOM_DATA[rooms[newRoomThing].roomID].backlink) {
            rooms[newRoomThing].connections.push_back(
                ConnectionInstance {
                    roomIndex,
                    ConnectionTest(),
                    ConnectionTest()
                }
            );
        }
    }
    rooms[roomIndex].connections = newConnections;
}


void GameState::AddMessage(std::string a_message) {
    messageStack.push_back(a_message);
}
    
void GameState::AppendMessage(std::string a_message) {
    if (messageStack.size() == 0) {
        messageStack.push_back(a_message);
    } else {
        messageStack[messageStack.size() - 1] += a_message;
    }
}

std::vector<StartData> GameState::Starts = {};