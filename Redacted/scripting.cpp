#define __STDC_WANT_LIB_EXT1__ 1
#include <cstring>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <cmath>

#define SCRIPT_PARSER_DEBUG_LOGGING 1

#pragma region StubData
void EatInput(void) {
    std::cout << "Eating input..." << std::endl;
}
#include <random>
struct EntityData {
public:
    int32_t  maxHP;                                  //< The max HP of the entity.
    int32_t  curHP;                                  //< The current HP of the entity.
    uint32_t maxMana;                                //< The max mana of the entity.
    uint32_t curMana;                                //< The current mana of the entity.
    int32_t  manaRegen;                              //< The mana regeneration of the entity.
    int32_t  manaSickness;                           //< How long until the entity can regain mana.
    int32_t  armor;                                  //< The armor of the entity.
    std::unordered_map<std::string, int32_t> skills; //< The skills of the entity.
    //std::vector<Action> actions;                     //< The actions of the entity.

    EntityData(void) {
        maxHP = curHP = 10;
        maxMana = curMana = 10;
        manaRegen = 1;
        manaSickness = 0;
        armor = 10;
        skills = {};
    }

    virtual int32_t GetEffectiveArmor(void) = 0;

    virtual void Hurt(int32_t) = 0;
    virtual void Heal(int32_t) = 0;

    virtual void DrainMana(int32_t a_amount) = 0;
    virtual void RegainMana(int32_t a_amount) = 0;

    int32_t GetSkillModifier(const std::string& a_skill) {
        return skills.find(a_skill) != skills.end() ? skills.at(a_skill) : 0;
    }

    int32_t SetSkillModifier(const std::string& a_skill, int32_t a_modifier) {
        skills.insert_or_assign(a_skill, a_modifier);
        return skills.at(a_skill);
    }

    int32_t AddSkillModifier(const std::string& a_skill, int32_t a_modifier) {
        skills.insert_or_assign(a_skill, GetSkillModifier(a_skill) + a_modifier);
        return skills.at(a_skill);
    }

    virtual ~EntityData() {}
};

struct PlayerData : public EntityData {
public:
    int32_t turns = 1;
    int32_t usedTurns = 0;
    int32_t gold = 0;
    int32_t xp = 0;

    PlayerData(void) : EntityData() {
        this->xp = 0;
        this->gold = 0;
    }

    #pragma region Inventory
    void UseItemAt(size_t a_index) {
        std::cout << "DEBUG: Used item at " << a_index << std::endl;
    }

    void UseItemOf(size_t a_itemID) {
        std::cout << "DEBUG: Used item of " << a_itemID << std::endl;
    }

    void AddItem(size_t a_itemID, uint32_t a_itemAmount) {
        std::cout << "DEBUG: Added item " << a_itemID << " x" << a_itemAmount << std::endl;
    }

    bool HasItem(size_t a_itemID, uint32_t a_itemAmount) {
        return false;
    }
    #pragma endregion

    int32_t LevelInClass(const std::string& a_class) {
        return 1;
    }

    int32_t GetEffectiveArmor(void) {
        return armor;
    }

    void Hurt(int32_t a_damage) {
        curHP = std::max<int32_t>(curHP - a_damage, 0);
    }

    void Heal(int32_t a_amount) {
        curHP = std::min<int32_t>(curHP + a_amount, maxHP);
    }

    void DrainMana(int32_t a_amount) {
        curMana = std::max<int32_t>(curMana - a_amount, 0);
    }

    void RegainMana(int32_t a_amount) {
        curMana = std::min<int32_t>(curMana + a_amount, maxMana);
    }
};

struct NPCData : public EntityData {
public:
    std::string name;
    int32_t xp;
    int32_t gold;
    bool stunned = false;

    NPCData(void) : EntityData() {
        this->name = "Unknown";
        this->xp = 0;
        this->gold = 0;
    }

    int32_t GetEffectiveArmor(void) {
        return armor;
    }

    void Hurt(int32_t a_damage) {
        curHP = std::max<int32_t>(curHP - a_damage, 0);
    }

    void Heal(int32_t a_amount) {
        curHP = std::min<int32_t>(curHP + a_amount, maxHP);
    }

    void DrainMana(int32_t a_amount) {
        curMana = std::max<int32_t>(curMana - a_amount, 0);
    }

    void RegainMana(int32_t a_amount) {
        curMana = std::min<int32_t>(curMana + a_amount, maxMana);
    }
};


struct RoomAction {
public:
    std::string name        = "";
    std::string description = "";
};

struct RoomActionReference {
public:
    RoomAction roomAction;
};

struct RoomInstance {
public:
    bool initialized = false;
    size_t roomID;
    size_t roomIndex;
    std::vector<NPCData> inhabitants;
    std::vector<RoomActionReference> roomActions;
    std::unordered_map<std::string, int32_t> flags;
    std::unordered_map<std::string, void*> data;

    RoomInstance(size_t a_roomID, size_t a_roomIndex) {
        this->roomID = a_roomID;
        this->roomIndex = a_roomIndex;
        this->inhabitants = std::vector<NPCData>();
        this->roomActions = std::vector<RoomActionReference>();
        this->flags = std::unordered_map<std::string, int32_t>();
        this->data = std::unordered_map<std::string, void*>();
    }

    size_t LivingInhabitants(void) {
        size_t toReturn = 0;
        for (const NPCData& npc : inhabitants) {
            if (npc.curHP > 0) {
                ++toReturn;
            }
        }
        return toReturn;
    }
};

struct GameState {
public:
    bool running = true;

    size_t usedRooms;
    size_t curRoom;
    size_t start;
    std::vector<RoomInstance> rooms;
    PlayerData player;

    std::mt19937 generator;

    GameState(std::mt19937 a_generator) {
        this->generator = a_generator;
        this->usedRooms = 0;
        this->curRoom = 0;
        this->rooms = {RoomInstance(1, 0)};
        this->player = PlayerData();
    }

    size_t PushBackRoom(size_t a_index) {
        this->rooms.push_back(RoomInstance(a_index, this->usedRooms));
        return this->usedRooms++;
    }

    int32_t RollDice(size_t a_amount, size_t a_die) {
        return 0;
    }
};

const std::unordered_map<std::string, RoomAction> ROOM_ACTIONS = {};

struct ShopCatalog {
public:
    size_t cost;
    //ItemStack stack;
};

struct ShopData {
public:
    std::string shopName;
};

struct ShopInstance {
public:
    std::string shopName;
    std::vector<ShopCatalog> catalog;
};

const std::unordered_map<std::string, ShopData> SHOP_DATA = {};

struct EntityTemplate {};
struct NPCTemplate {};

const std::unordered_map<std::string, EntityTemplate> ENTITY_TEMPLATES = {};
const std::unordered_map<std::string, NPCTemplate> NPC_TEMPLATES = {};
const std::unordered_map<std::string, std::function<NPCData(NPCData)>> NPC_MODIFIERS = {};

void GenerateShopInstance(GameState& a_gameState, const ShopData& a_shopData, ShopInstance& a_shopInstance) {}
#pragma endregion

#pragma region String Utils
bool IsStringBool(std::string_view a_string) {
    return a_string == "true" || a_string == "false";
}

bool IsStringInt(std::string_view a_string) {
    for (size_t i = (a_string.starts_with('-') ? 1 : 0); i < a_string.size(); ++i) {
        switch (a_string[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                break;
            default:
                return false;
                break;
        }
    }
    return true;
}

size_t SplitString(const std::string& a_string, std::string_view a_seperator, std::vector<std::string>& a_vector) {
    int32_t previous = 0;
    int32_t cur = 0;
    while ((cur = a_string.find(a_seperator, cur)) < a_string.size()) {
        a_vector.push_back(a_string.substr(previous, cur - previous));
        //std::cout << "Cur: " << cur << " Previous: " << previous << " Substring: " << a_vector.back() << std::endl;
        previous = ++cur;
    }
    a_vector.push_back(a_string.substr(previous, cur - previous));
    //std::cout << "Cur: " << cur << " Previous: " << previous << " Substring: " << a_vector.back() << std::endl;
    return a_vector.size();
}

void SanitizeString(std::string& a_string) {
    size_t startOff = 0;
    size_t endOff = 0;
    bool startDone = false;
    for (size_t i = 0; i < a_string.size(); ++i) {
        switch (a_string[i]) {
            case ' ':
            case '\n':
            case '\t':
            case '\v':
                break;
            default:
                if (startDone) {
                    endOff = i;
                } else {
                    startOff = i;
                    startDone = true;
                }
                break;
        }
    }
    a_string = a_string.substr(startOff, endOff - startOff + 1);
}
#pragma endregion

#pragma region Sockscript
struct VariablePtr {
public:
    void* data;
    size_t size;

    VariablePtr(const void* a_data, size_t a_size) {
        this->data = malloc(a_size);
        this->size = a_size;
        
        #ifdef __STDC_LIB_EXT1__
        memcpy_s(this->data, a_size, a_data, a_size);
        #else
        memcpy(this->data, a_data, a_size);
        #endif
    }

    void SetData(const void* a_data, size_t a_size) {
        this->data = realloc(this->data, a_size);
        this->size = a_size;

        #ifdef __STDC_LIB_EXT1__
        memcpy_s(this->data, a_size, a_data, a_size);
        #else
        memcpy(this->data, a_data, a_size);
        #endif
    }

    ~VariablePtr() {
        free(this->data);
    }
};

enum struct ReaderMode {
    READ,
    VAR_NAME,
    VAR_TYPE,
    VAR_VALUE,
    VAR_EXPR,
    RET_EXPR,
    OBJECT,
    EXPRESSION,
    ARGS,
    FALLTHROUGH,
    FINISHED,
};

std::string ReaderModeDisplayName(ReaderMode a_mode) {
    switch (a_mode) {
        case ReaderMode::READ:
            return "READING";
        case ReaderMode::VAR_NAME:
            return "VAR_NAM";
        case ReaderMode::VAR_TYPE:
            return "VAR_TYP";
        case ReaderMode::VAR_VALUE:
            return "VAR_VAL";
        case ReaderMode::VAR_EXPR:
            return "VAR_EXP";
        case ReaderMode::RET_EXPR:
            return "RET_EXP";
        case ReaderMode::OBJECT:
            return "OBJECTS";
        case ReaderMode::EXPRESSION:
            return "EXPRESS";
        case ReaderMode::ARGS:
            return "ARGUMEN";
        case ReaderMode::FALLTHROUGH:
            return "FALLTHR";
        case ReaderMode::FINISHED:
            return "FINISHE";
        default:
            return "INVALID";
    }
}

enum struct OperatorEvaluation {
    NONE,
    ADDI,
    SUBT,
    MULT,
    DIVI,
    MODU,
    NOT,
    AND,
    OR,
    GT,
    GTE,
    LT,
    LTE,
    EQ,
    NEQ,
};

enum struct RegisterType {
    ERROR,
    STRING,
    INT,
    FLOAT,
    BOOL,
};

struct InterpreterContext {
public:
    std::vector<std::string> stringRegisters = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
    std::vector<int32_t> intRegisters = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<float> floatRegisters = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<bool> boolRegisters = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
    std::vector<std::string> context = std::vector<std::string>();
    int32_t stringRegister = 0;
    int32_t intRegister = -1;
    int32_t floatRegister = -1;
    int32_t boolRegister = -1;
    size_t targetLevel = 0;
    size_t currentLevel = 0;
    OperatorEvaluation operatorEvaluation = OperatorEvaluation::NONE;
    std::vector<ReaderMode> mode = {ReaderMode::READ};

    void DumpStringRegister(void) {
        for (size_t i = 0; i < stringRegisters.size(); ++i) {
            std::cout << std::format("{}String Register {:0>2}: {}\x1b[39m", i == stringRegister ? "\x1b[32m" : "\x1b[34m", i + 1, stringRegisters[i]) << std::endl;
        }
    }

    void DumpIntRegister(void) {
        for (size_t i = 0; i < intRegisters.size(); ++i) {
            std::cout << (i == intRegister ? "\x1b[32m" : "\x1b[34m") << "Int Register " << (i + 1) << ": " << intRegisters[i] << "\x1b[39m" << std::endl;
        }
    }
    
    void DumpFloatRegister(void) {
        for (size_t i = 0; i < floatRegisters.size(); ++i) {
            std::cout << (i == floatRegister ? "\x1b[32m" : "\x1b[34m") << "Float Register " << (i + 1) << ": " << floatRegisters[i] << "\x1b[39m" << std::endl;
        }
    }
    
    void DumpBoolRegister(void) {
        for (size_t i = 0; i < boolRegisters.size(); ++i) {
            std::cout << (i == boolRegister ? "\x1b[32m" : "\x1b[34m") << "Bool Register " << (i + 1) << ": " << (boolRegisters[i] ? "true" : "false") << "\x1b[39m" << std::endl;
        }
    }

    RegisterType SetIntRegister(int32_t a_value) {
        switch (operatorEvaluation) {
            case OperatorEvaluation::NONE:
                intRegisters[++intRegister] = a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::INT;
            case OperatorEvaluation::ADDI:
                intRegisters[intRegister] = intRegisters[intRegister] + a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::INT;
            case OperatorEvaluation::SUBT:
                intRegisters[intRegister] = intRegisters[intRegister] - a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::INT;
            case OperatorEvaluation::MULT:
                intRegisters[intRegister] = intRegisters[intRegister] * a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::INT;
            case OperatorEvaluation::DIVI:
                intRegisters[intRegister] = intRegisters[intRegister] / a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::INT;
            case OperatorEvaluation::MODU:
                intRegisters[intRegister] = intRegisters[intRegister] % a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::INT;
            case OperatorEvaluation::GT:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(intRegisters[intRegister] > a_value);
            case OperatorEvaluation::GTE:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(intRegisters[intRegister] >= a_value);
            case OperatorEvaluation::LT:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(intRegisters[intRegister] < a_value);
            case OperatorEvaluation::LTE:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(intRegisters[intRegister] <= a_value);
            case OperatorEvaluation::EQ:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(intRegisters[intRegister] == a_value);
            case OperatorEvaluation::NEQ:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(intRegisters[intRegister] != a_value);
            default:
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::ERROR;
        }
        
    }

    RegisterType SetFloatRegister(float a_value) {
        switch (operatorEvaluation) {
            case OperatorEvaluation::NONE:
                floatRegisters[++floatRegister] = a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::FLOAT;
            case OperatorEvaluation::ADDI:
                floatRegisters[floatRegister] = floatRegisters[floatRegister] + a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::FLOAT;
            case OperatorEvaluation::SUBT:
                floatRegisters[floatRegister] = floatRegisters[floatRegister] - a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::FLOAT;
            case OperatorEvaluation::MULT:
                floatRegisters[floatRegister] = floatRegisters[floatRegister] * a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::FLOAT;
            case OperatorEvaluation::DIVI:
                floatRegisters[floatRegister] = floatRegisters[floatRegister] / a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::FLOAT;
            case OperatorEvaluation::GT:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(floatRegisters[floatRegister] > a_value);
            case OperatorEvaluation::GTE:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(floatRegisters[floatRegister] >= a_value);
            case OperatorEvaluation::LT:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(floatRegisters[floatRegister] < a_value);
            case OperatorEvaluation::LTE:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(floatRegisters[floatRegister] <= a_value);
            case OperatorEvaluation::EQ:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(floatRegisters[floatRegister] == a_value);
            case OperatorEvaluation::NEQ:
                operatorEvaluation = OperatorEvaluation::NONE;
                return SetBoolRegister(floatRegisters[floatRegister] != a_value);
            default:
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::ERROR;
        }
      
    }

    RegisterType SetBoolRegister(bool a_value) {
        switch (operatorEvaluation) {
            case OperatorEvaluation::NONE:
                boolRegisters[++boolRegister] = a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            case OperatorEvaluation::NOT:
                boolRegisters[++boolRegister] = !a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            case OperatorEvaluation::AND:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] && a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            case OperatorEvaluation::OR:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] || a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            case OperatorEvaluation::EQ:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] == a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            case OperatorEvaluation::NEQ:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] != a_value;
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::BOOL;
            default:
                operatorEvaluation = OperatorEvaluation::NONE;
                return RegisterType::ERROR;
        }
    }
};

struct Interpreter {
public:
    std::unordered_map<std::string, VariablePtr> variables = std::unordered_map<std::string, VariablePtr>();

    Interpreter(void) {

    }

    template <typename T>
    T ParseStatement(GameState& a_gameState, RegisterType a_returnType, std::string a_statement) {
        InterpreterContext context = InterpreterContext();
        std::vector<std::pair<int32_t, bool>> ifTrue = {};
        int32_t ifLevel = 0;
        bool inString = false;
        bool escaped = false;
        int32_t embedLevel = 0;
        size_t step = 0;
        for (char character : a_statement) {
            #ifdef SCRIPT_PARSER_DEBUG_LOGGING
            std::cout << std::format("\x1b[1mPARSER\x1b[22m Step {:0>3}) [{}] {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
            #endif
            switch (context.mode.back()) {
                case ReaderMode::READ: {
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v': {
                            if (context.stringRegisters[context.stringRegister] == "return") {
                                context.stringRegisters[context.stringRegister] = "";
                                context.mode.push_back(ReaderMode::RET_EXPR);
                            }
                            break;
                        }
                        case '<': {
                            context.stringRegister += 1;
                            context.mode.push_back(ReaderMode::VAR_TYPE);
                            break;
                        }
                        case ';': {
                            if (context.stringRegisters[context.stringRegister] == "end") {
                                if (ifTrue.size() > 0 && ifTrue.back().first == context.currentLevel) {
                                    ifTrue.pop_back();
                                } else {
                                    context.mode.push_back(ReaderMode::FINISHED);
                                }
                            }
                            break;
                        }
                        case '{': {
                            #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                            std::cout << "Bracket hit with: " << context.stringRegisters[context.stringRegister] << std::endl;
                            #endif
                            if (context.stringRegisters[context.stringRegister] == "else") {
                                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                                std::cout << "Truism: <" << ifTrue.back().first << ", " << (ifTrue.back().second ? "true" : "false") << ">" << std::endl;
                                #endif
                                if (ifTrue.back().second) {
                                    context.mode.push_back(ReaderMode::FALLTHROUGH);
                                } else {
                                    ++context.currentLevel;
                                }
                            } else {
                                ++context.currentLevel;
                            }
                            context.stringRegisters[context.stringRegister] = "";
                            break;
                        }
                        case '}': {
                            --context.currentLevel;
                            while (ifTrue.back().first > context.currentLevel) {
                                ifTrue.pop_back();
                            }
                            break;
                        }
                        case '.': {
                            ParseObjectName(a_gameState, context);
                            break;
                        }
                        case '(': {
                            if (context.stringRegisters[context.stringRegister] == "if") {
                                context.stringRegisters[context.stringRegister] = "";
                                context.targetLevel = context.currentLevel;
                                context.mode.push_back(ReaderMode::EXPRESSION);
                            } else if (context.stringRegisters[context.stringRegister] == "elif") {
                                context.stringRegisters[context.stringRegister] = "";
                                if (ifTrue.back().second) {
                                    context.mode.push_back(ReaderMode::FALLTHROUGH);
                                } else {
                                    ifTrue.pop_back();
                                    context.mode.push_back(ReaderMode::EXPRESSION);
                                }
                            }
                            break;
                        }
                        default: {
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                        }
                    }
                    break;
                }
                case ReaderMode::VAR_TYPE: {
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '>': {
                            context.mode.pop_back();
                            context.mode.push_back(ReaderMode::VAR_VALUE);
                            context.stringRegister += 1;
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::VAR_VALUE: {
                    switch (character) {
                        case ';':
                            std::cout << "[ERROR] Semicolon before variable expression provided." << std::endl;
                            break;
                        case '=': {
                            context.mode.pop_back();
                            context.mode.push_back(ReaderMode::VAR_EXPR);
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                case ReaderMode::VAR_EXPR: {
                    switch (character) {
                        case ' ':
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '"':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                } else {
                                    inString = false;
                                }
                                escaped = false;
                            } else {
                                inString = true;
                            }
                            break;
                        case '\\':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                    escaped = false;
                                } else {
                                    escaped = true;
                                }
                            }
                            break;
                        case ';': {
                            if (inString) { 
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                context.mode.pop_back();
                                std::string varName = context.stringRegisters[context.stringRegister - 2];
                                std::string varType = context.stringRegisters[context.stringRegister - 1];
                                if (varType == "uint32") {
                                    int32_t value = ParseIntExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                    variables.insert_or_assign(varName, VariablePtr((void*)&value, sizeof(int32_t)));
                                } else if (varType == "bool") {
                                    bool value = ParseBoolExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                    variables.insert_or_assign(varName, VariablePtr((void*)&value, sizeof(bool)));
                                } else if (varType == "string") {
                                    std::cout << "String is not a supported data type." << std::endl;
                                }

                                //context.DumpStringRegister();
                                for (size_t i = 0; i < 3; ++i) {
                                    context.stringRegisters[context.stringRegister--] = "";
                                }
                                context.stringRegister += 1;
                                //context.DumpStringRegister();
                            }
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                            break;
                    }
                    break;
                }
                case ReaderMode::RET_EXPR: {
                    switch (character) {
                        case ' ':
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '"':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                } else {
                                    inString = false;
                                }
                                escaped = false;
                            } else {
                                inString = true;
                            }
                            break;
                        case '\\':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                    escaped = false;
                                } else {
                                    escaped = true;
                                }
                            }
                            break;
                        case ';': {
                            if (inString) { 
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                context.mode.pop_back();
                                switch (a_returnType) {
                                    case RegisterType::INT: {
                                        int32_t value = ParseIntExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                        return *(T*)(&value);
                                    }
                                    case RegisterType::BOOL: {
                                        bool value = ParseBoolExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                        return *(T*)(&value);
                                    }
                                    case RegisterType::STRING: {
                                        std::string value = FormatString(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                        return *(T*)(&value);
                                    }
                                    case RegisterType::ERROR: {
                                        return (T)(0);
                                    }
                                }
                            }
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                            break;
                    }
                    break;
                }
                case ReaderMode::OBJECT: {
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                        case ')': {
                            ParseObjectMember(a_gameState, context);
                            break;
                        }
                        case '(': {
                            ParseObjectMethod(a_gameState, context);
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::ARGS: {
                    switch (character) {
                        case ' ':
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '"':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                } else {
                                    inString = false;
                                }
                                escaped = false;
                            } else {
                                inString = true;
                            }
                            break;
                        case '\\':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                    escaped = false;
                                } else {
                                    escaped = true;
                                }
                            }
                            break;
                        case 'n':
                            if (inString && escaped) {
                                context.stringRegisters[context.stringRegister] += "\\";
                            }
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                            break;
                        case ')': {
                            if (!inString) {
                                ParseObjectMethodArgs(a_gameState, context);
                            }
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                            break;
                    }
                    break;
                }
                case ReaderMode::EXPRESSION: {
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                        case '(':
                            embedLevel += 1;
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                        case ')': {
                            if (embedLevel-- <= 0) {
                                ifTrue.push_back(std::pair<int32_t, bool>(context.currentLevel, ParseBoolExpression(a_gameState, context, context.stringRegisters[context.stringRegister])));
                                context.mode.pop_back();
                                if (!ifTrue.back().second) {
                                    context.mode.push_back(ReaderMode::FALLTHROUGH);
                                }
                                context.stringRegisters[context.stringRegister] = "";
                            } else {
                                context.stringRegisters[context.stringRegister] += character;
                            }
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::FALLTHROUGH: {
                    switch (character) {
                        case '\\':
                            escaped = !escaped;
                            break;
                        case '{':
                            if (!escaped) {
                                ++context.currentLevel;
                            }
                            escaped = false;
                            break;
                        case '}':
                            if (!escaped) {
                                --context.currentLevel;
                                if (context.currentLevel == context.targetLevel) {
                                    context.mode.pop_back();
                                }
                            }
                            escaped = false;
                            break;
                        default:
                            escaped = false;
                            break;
                    }
                    break;
                }
            }
        }

        switch (a_returnType) {
            case RegisterType::INT:
                return (T)0;
            case RegisterType::BOOL:
                return (T)false;
            case RegisterType::STRING:
                return (T)"";
            case RegisterType::ERROR:
                return (T)(0);
        }
        
        return (T)(0);
    }

    int32_t ParseIntExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << "Parsing Int Expression: \n" << a_expression << std::endl;
        #endif
        InterpreterContext context = InterpreterContext();

        bool inString = false;
        bool escaped = false;
        bool justOperated = false;
        int32_t step = 0;
        for (char character : a_expression) {
            #ifdef SCRIPT_PARSER_DEBUG_LOGGING
            std::cout << std::format("\x1b[1mINT_EX\x1b[22m Step {:0>3}) [{}] {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
            #endif
            switch (context.mode.back()) {
                case ReaderMode::READ: {
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            if (context.stringRegisters[context.stringRegister] != "") {
                                ParseRawType(a_gameState, context);
                            }
                            justOperated = false;
                            break;
                        case '@':
                            context.stringRegister += 1;
                            context.mode.push_back(ReaderMode::VAR_NAME);
                            break;
                        case '{':
                        case '}':
                            justOperated = false;
                            std::cout << "[ERROR] Invalid characters." << std::endl;
                            break;
                        case '.':
                            justOperated = false;
                            ParseObjectName(a_gameState, context);
                            break;
                        case '+':
                            context.operatorEvaluation = OperatorEvaluation::ADDI;
                            justOperated = true;
                            break;
                        case '-':
                            context.operatorEvaluation = OperatorEvaluation::SUBT;
                            justOperated = true;
                            break;
                        case '*':
                            context.operatorEvaluation = OperatorEvaluation::MULT;
                            justOperated = true;
                            break;
                        case '/':
                            context.operatorEvaluation = OperatorEvaluation::DIVI;
                            justOperated = true;
                            break;
                        case '!':
                            context.operatorEvaluation = OperatorEvaluation::NOT;
                            justOperated = true;
                            break;
                        case '<':
                            context.operatorEvaluation = OperatorEvaluation::LT;
                            justOperated = true;
                            break;
                        case '>':
                            context.operatorEvaluation = OperatorEvaluation::GT;
                            justOperated = true;
                            break;
                        case '=':
                            switch (context.operatorEvaluation) {
                                case OperatorEvaluation::NONE:
                                    context.operatorEvaluation = OperatorEvaluation::EQ;
                                    break;
                                case OperatorEvaluation::NOT:
                                    context.operatorEvaluation = OperatorEvaluation::NEQ;
                                    break;
                                case OperatorEvaluation::LT:
                                    context.operatorEvaluation = OperatorEvaluation::LTE;
                                    break;
                                case OperatorEvaluation::GT:
                                    context.operatorEvaluation = OperatorEvaluation::GTE;
                                    break;
                            }
                            justOperated = true;
                            break;
                        default:
                            justOperated = false;
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::VAR_NAME: {
                    switch (character) {
                        case '@': {
                            ParseVariableName(a_gameState, context);
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::OBJECT: {
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                        case ')': {
                            ParseObjectMember(a_gameState, context);
                            break;
                        }
                        case '(': {
                            ParseObjectMethod(a_gameState, context);
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::ARGS: {
                    switch (character) {
                        case ' ':
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '"':
                            context.stringRegisters[context.stringRegister] += character;
                            if (inString) {
                                if (!escaped) {
                                    inString = false;
                                }
                                escaped = false;
                            } else {
                                inString = true;
                            }
                            break;
                        case '\\':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                    escaped = false;
                                } else {
                                    escaped = true;
                                }
                            }
                            break;
                        case 'n':
                            if (inString && escaped) {
                                context.stringRegisters[context.stringRegister] += "\\";
                            }
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                        case ')': {
                            ParseObjectMethodArgs(a_gameState, context);
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
            }
        }

        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << std::format("\x1b[1mINT_EX\x1b[22m Step {:0>3}) [{}] {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif

        if (context.stringRegisters[context.stringRegister] != "") {
            switch (context.mode.back()) {
                case ReaderMode::READ:
                    ParseRawType(a_gameState, context);
                    break;
                case ReaderMode::OBJECT:
                    ParseObjectMember(a_gameState, context);
                    break;
            }
        }

        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << "Returning: " << context.intRegisters[context.intRegister] << std::endl;
        #endif

        return context.intRegisters[context.intRegister];
    }

    int32_t ParseBoolExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << "Parsing Bool Expression: \n" << a_expression << std::endl;
        #endif
        InterpreterContext context = InterpreterContext();

        bool inString = false;
        bool escaped = false;
        bool justOperated = false;
        int32_t step = 0;
        for (char character : a_expression) {
            #ifdef SCRIPT_PARSER_DEBUG_LOGGING
            std::cout << std::format("\x1b[1mBOO_EX\x1b[22m Step {:0>3}) [{}] {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
            #endif
            switch (context.mode.back()) {
                case ReaderMode::READ: {
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            if (context.stringRegisters[context.stringRegister] != "") {
                                ParseRawType(a_gameState, context);
                            }
                            justOperated = false;
                            break;
                        case '@':
                            context.stringRegister += 1;
                            context.mode.push_back(ReaderMode::VAR_NAME);
                            break;
                        case '{':
                        case '}':
                            justOperated = false;
                            std::cout << "[ERROR] Invalid characters." << std::endl;
                            break;
                        case '.':
                            justOperated = false;
                            ParseObjectName(a_gameState, context);
                            break;
                        case '+':
                            context.operatorEvaluation = OperatorEvaluation::ADDI;
                            justOperated = true;
                            break;
                        case '-':
                            context.operatorEvaluation = OperatorEvaluation::SUBT;
                            justOperated = true;
                            break;
                        case '*':
                            context.operatorEvaluation = OperatorEvaluation::MULT;
                            justOperated = true;
                            break;
                        case '/':
                            context.operatorEvaluation = OperatorEvaluation::DIVI;
                            justOperated = true;
                            break;
                        case '!':
                            context.operatorEvaluation = OperatorEvaluation::NOT;
                            justOperated = true;
                            break;
                        case '<':
                            context.operatorEvaluation = OperatorEvaluation::LT;
                            justOperated = true;
                            break;
                        case '>':
                            context.operatorEvaluation = OperatorEvaluation::GT;
                            justOperated = true;
                            break;
                        case '=':
                            switch (context.operatorEvaluation) {
                                case OperatorEvaluation::NONE:
                                    context.operatorEvaluation = OperatorEvaluation::EQ;
                                    break;
                                case OperatorEvaluation::NOT:
                                    context.operatorEvaluation = OperatorEvaluation::NEQ;
                                    break;
                                case OperatorEvaluation::LT:
                                    context.operatorEvaluation = OperatorEvaluation::LTE;
                                    break;
                                case OperatorEvaluation::GT:
                                    context.operatorEvaluation = OperatorEvaluation::GTE;
                                    break;
                            }
                            justOperated = true;
                            break;
                        default:
                            justOperated = false;
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::VAR_NAME: {
                    switch (character) {
                        case '@': {
                            ParseVariableName(a_gameState, context);
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::OBJECT: {
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                        case ')': {
                            ParseObjectMember(a_gameState, context);
                            break;
                        }
                        case '(': {
                            ParseObjectMethod(a_gameState, context);
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::ARGS: {
                    switch (character) {
                        case ' ':
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '"':
                            context.stringRegisters[context.stringRegister] += character;
                            if (inString) {
                                if (!escaped) {
                                    inString = false;
                                }
                                escaped = false;
                            } else {
                                inString = true;
                            }
                            break;
                        case '\\':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                    escaped = false;
                                } else {
                                    escaped = true;
                                }
                            }
                            break;
                        case 'n':
                            if (inString && escaped) {
                                context.stringRegisters[context.stringRegister] += "\\";
                            }
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                        case ')': {
                            ParseObjectMethodArgs(a_gameState, context);
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
            }
        }

        if (context.stringRegisters[context.stringRegister] != "") {
            #ifdef SCRIPT_PARSER_DEBUG_LOGGING
            std::cout << context.stringRegister << ") " << context.stringRegisters[context.stringRegister] << std::endl;
            #endif
            switch (context.mode.back()) {
                case ReaderMode::READ:
                    ParseRawType(a_gameState, context);
                    break;
                case ReaderMode::OBJECT:
                    ParseObjectMember(a_gameState, context);
                    break;
            }
        }

        
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << "Returning: " << (context.boolRegisters[context.boolRegister] ? "true" : "false") << std::endl;
        #endif

        return context.boolRegisters[context.boolRegister];
    }

    std::string FormatString(GameState& a_gameState, InterpreterContext& a_context, std::string a_string) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << "Formatting String: \n" << a_string << std::endl;
        #endif
        InterpreterContext context = InterpreterContext();
        std::string result = "";
        RegisterType registerType = RegisterType::INT;
        bool inString = true;
        bool escaped = false;
        size_t charNum = 0;
        for (char character : a_string) {
            #ifdef SCRIPT_PARSER_DEBUG_LOGGING
            if (inString) {
                std::cout << std::format("\x1b[1mSTR_FM\x1b[22m Step {:0>3}) [{}] {}InString\x1b[39m {}Escaped\x1b[39m [{:0>3}] {}", charNum++, ReaderModeDisplayName(context.mode.back()), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), result.size(), result) << std::endl;
            } else {
                std::cout << std::format("\x1b[1mSTR_FM\x1b[22m Step {:0>3}) [{}] {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", charNum++, ReaderModeDisplayName(context.mode.back()), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
            }
            #endif

            switch (context.mode.back()) {
                case ReaderMode::READ: {
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            if (inString) {
                                result += character;
                            }
                            break;
                        case '\\':
                            if (inString) {
                                if (escaped) {
                                    result += character;
                                    escaped = false;
                                } else {
                                    escaped = true;
                                }
                            }
                            break;
                        case 'n':
                            if (inString) {
                                if (escaped) {
                                    result += '\n';
                                } else {
                                    result += character;
                                }
                                escaped = false;
                            } else {
                                context.stringRegisters[context.stringRegister] += character;
                            }
                            break;
                        case '@':
                            if (inString) {
                                result += character;
                            } else {
                                context.stringRegister += 1;
                                context.mode.push_back(ReaderMode::VAR_NAME);
                            }
                            break;
                        case '{':
                            if (inString) {
                                if (escaped) {
                                    result += character;
                                    escaped = false;
                                } else {
                                    inString = false;
                                }
                            } else {
                                std::cout << "[ERROR] Invalid characters." << std::endl;
                            }
                            break;
                        case '}':
                            if (inString) {
                                if (escaped) {
                                    result += character;
                                    escaped = false;
                                } else {
                                    std::cout << "[ERROR] Invalid characters." << std::endl;
                                }
                            } else {
                                inString = true;
                                // Match Register Type?
                                switch (registerType) {
                                    case RegisterType::STRING:
                                        result += context.stringRegisters[context.stringRegister];
                                        break;
                                    case RegisterType::INT:
                                        result += std::to_string(context.intRegisters[context.intRegister]);
                                        break;
                                    case RegisterType::FLOAT:
                                        result += std::to_string(context.floatRegisters[context.floatRegister]);
                                        break;
                                    case RegisterType::BOOL:
                                        result += context.boolRegisters[context.boolRegister] ? "true" : "false";
                                        break;
                                }
                            }
                            break;
                        case '.':
                            if (inString) {
                                result += character;
                            } else {
                                ParseObjectName(a_gameState, context);
                            }
                            break;
                        case '+':
                            if (inString) {
                                result += character;
                            } else {
                                context.operatorEvaluation = OperatorEvaluation::ADDI;
                            }
                            break;
                        case '-':
                            if (inString) {
                                result += character;
                            } else {
                                context.operatorEvaluation = OperatorEvaluation::SUBT;
                            }
                            break;
                        case '*':
                            if (inString) {
                                result += character;
                            } else {
                                context.operatorEvaluation = OperatorEvaluation::MULT;
                            }
                            break;
                        case '/':
                            if (inString) {
                                result += character;
                            } else {
                                context.operatorEvaluation = OperatorEvaluation::DIVI;
                            }
                            break;
                        default:
                            if (inString) {
                                result += character;
                            } else {
                                context.stringRegisters[context.stringRegister] += character;
                            }
                            break;
                    }
                    break;
                }
                case ReaderMode::VAR_NAME: {
                    switch (character) {
                        case '@': {
                            ParseVariableName(a_gameState, context);
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::OBJECT: {
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                        case ')': {
                            ParseObjectMember(a_gameState, context);
                            break;
                        }
                        case '(': {
                            ParseObjectMethod(a_gameState, context);
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
                case ReaderMode::ARGS: {
                    switch (character) {
                        case ' ':
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case ')': {
                            ParseObjectMethodArgs(a_gameState, context);
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                }
            }
        }

        return result;
    }

    void ParseRawType(GameState& a_gameState, InterpreterContext& a_context) {
        if (IsStringInt(a_context.stringRegisters[a_context.stringRegister])) {
            a_context.SetIntRegister(std::stoi(a_context.stringRegisters[a_context.stringRegister]));
            a_context.stringRegisters[a_context.stringRegister] = "";
        } else if (IsStringBool(a_context.stringRegisters[a_context.stringRegister])) {
            a_context.SetBoolRegister(a_context.stringRegisters[a_context.stringRegister] == "true");
            a_context.stringRegisters[a_context.stringRegister] = "";
        }
    }

    void ParseVariableName(GameState& a_gameState, InterpreterContext& a_context) {
        std::string varType = a_context.stringRegisters[a_context.stringRegister - 1];
        if (varType == "int32") {
            a_context.SetIntRegister(*((int32_t*)variables.at(a_context.stringRegisters[a_context.stringRegister]).data));

        } else if (varType == "uint32") {
            a_context.SetIntRegister(*((int32_t*)variables.at(a_context.stringRegisters[a_context.stringRegister]).data));

        } else if (varType == "bool") {
            a_context.SetBoolRegister(*((bool*)variables.at(a_context.stringRegisters[a_context.stringRegister]).data));

        }

        for (size_t i = 0; i < 2; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }
        a_context.stringRegister += 1;
        a_context.mode.pop_back();
    }

    void ParseObjectName(GameState& a_gameState, InterpreterContext& a_context) {
        if (a_context.stringRegisters[a_context.stringRegister] == "player") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::OBJECT);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "math") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::OBJECT);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "random") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::OBJECT);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "console") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::OBJECT);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "room") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::OBJECT);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "gameState") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::OBJECT);
        }
    }

    void ParseObjectMember(GameState& a_gameState, InterpreterContext& a_context) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << "Operating on member " << a_context.stringRegisters[a_context.stringRegister] << std::endl;
        #endif
        std::string object = a_context.stringRegisters[a_context.stringRegister - 1];
        if (object == "player") {
            if (a_context.stringRegisters[a_context.stringRegister] == "maxMana") {
                a_context.SetIntRegister(a_gameState.player.maxMana);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "curMana") {
                a_context.SetIntRegister(a_gameState.player.curMana);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "maxHP") {
                a_context.SetIntRegister(a_gameState.player.maxHP);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "curHP") {
                a_context.SetIntRegister(a_gameState.player.curHP);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "armor") {
                a_context.SetIntRegister(a_gameState.player.armor);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "gold") {
                a_context.SetIntRegister(a_gameState.player.gold);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "xp") {
                a_context.SetIntRegister(a_gameState.player.xp);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "manaSickness") {
                a_context.SetIntRegister(a_gameState.player.manaSickness);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "manaRegen") {
                a_context.SetIntRegister(a_gameState.player.manaRegen);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "turns") {
                a_context.SetIntRegister(a_gameState.player.turns);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "usedTurns") {
                a_context.SetIntRegister(a_gameState.player.usedTurns);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "effectiveArmor") {
                a_context.SetIntRegister(a_gameState.player.GetEffectiveArmor());
            }
        } else if (object == "room") {
            if (a_context.stringRegisters[a_context.stringRegister] == "livingInhabitants") {
                a_context.SetIntRegister(a_gameState.rooms[a_gameState.curRoom].LivingInhabitants());
            }
        } else if (object == "gameState") {
            if (a_context.stringRegisters[a_context.stringRegister] == "curRoom") {
                a_context.SetIntRegister(a_gameState.curRoom);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "usedRooms") {
                a_context.SetIntRegister(a_gameState.usedRooms);
            }
        }
        a_context.mode.pop_back();
    }

    void ParseObjectMethod(GameState& a_gameState, InterpreterContext& a_context) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        std::cout << "Operating on function " << a_context.stringRegisters[a_context.stringRegister] << std::endl;
        #endif
        std::string object = a_context.stringRegisters[a_context.stringRegister - 1];
        a_context.mode.pop_back();
        if (object == "player") {
            if (a_context.stringRegisters[a_context.stringRegister] == "HasPerk") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "Hurt") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "Heal") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "DrainMana") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "RegainMana") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "GetSkillModifier") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "SetSkillModifier") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "AddSkillModifier") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            }
        } else if (object == "math") {
            if (a_context.stringRegisters[a_context.stringRegister] == "Min") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "Max") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "Clamp") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            }
        } else if (object == "random") {
            if (a_context.stringRegisters[a_context.stringRegister] == "Range") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "RollDice") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            }
        } else if (object == "console") {
            if (a_context.stringRegisters[a_context.stringRegister] == "Print") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "EatInput") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            }
        } else if (object == "room") {
            if (a_context.stringRegisters[a_context.stringRegister] == "AddInhabitant") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "AddRoomAction") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "AddShop") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "HasFlag") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "GetFlag") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            } else if (a_context.stringRegisters[a_context.stringRegister] == "SetFlag") {
                a_context.stringRegister += 1;
                a_context.mode.push_back(ReaderMode::ARGS);
            }
        }
    }

    void ParseObjectMethodArgs(GameState& a_gameState, InterpreterContext& a_context) {
        std::vector<std::string> args = std::vector<std::string>();
        size_t argc = SplitString(a_context.stringRegisters[a_context.stringRegister], ",", args);
        for (size_t i = 0; i < argc; ++i) {
            SanitizeString(args[i]);
            int32_t index = 0;
            while ((index = args[i].find("<comma>", 0)) < args[i].size()) {
                args[i] = args[i].substr(0, index) + "," + args[i].substr(index + 7, args[i].size() - index - 7);
            }
        }
        std::string object = a_context.stringRegisters[a_context.stringRegister - 2];
        std::string method = a_context.stringRegisters[a_context.stringRegister - 1];
        if (object == "player") {
            if (method == "HasPerk") {
                if (argc == 1) {
                    if (args[0] == "ARCANE_EYES") {
                        a_context.SetBoolRegister(true);
                    } else {
                        std::cout << "[ERROR]: " << args[0] << " is not a valid perk id." << std::endl;
                    }
                } else {
                    std::cout << "[ERROR]: player.HasPerk must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "Hurt") {
                if (argc == 1) {
                    a_gameState.player.Hurt(ParseIntExpression(a_gameState, a_context, args[0]));
                } else {
                    std::cout << "[ERROR]: player.Hurt must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "Heal") {
                if (argc == 1) {
                    a_gameState.player.Heal(ParseIntExpression(a_gameState, a_context, args[0]));
                } else {
                    std::cout << "[ERROR]: player.Heal must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "DrainMana") {
                if (argc == 1) {
                    a_gameState.player.DrainMana(ParseIntExpression(a_gameState, a_context, args[0]));
                } else {
                    std::cout << "[ERROR]: player.DrainMana must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "RegainMana") {
                if (argc == 1) {
                    a_gameState.player.RegainMana(ParseIntExpression(a_gameState, a_context, args[0]));
                } else {
                    std::cout << "[ERROR]: player.RegainMana must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "GetSkillModifier") {
                if (argc == 1) {
                    a_context.SetIntRegister(a_gameState.player.GetSkillModifier(FormatString(a_gameState, a_context, args[0])));
                } else {
                    std::cout << "[ERROR]: player.GetSkillModifier must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "SetSkillModifier") {
                if (argc == 2) {
                    a_context.SetIntRegister(a_gameState.player.SetSkillModifier(FormatString(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
                } else {
                    std::cout << "[ERROR]: player.SetSkillModifier must have 2 arguments, this call has " << argc << "." << std::endl;
                }
            } else if (method == "AddSkillModifier") {
                if (argc == 2) {
                    a_context.SetIntRegister(a_gameState.player.SetSkillModifier(FormatString(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
                } else {
                    std::cout << "[ERROR]: player.AddSkillModifier must have 2 arguments, this call has " << argc << "." << std::endl;
                }
            }

            for (size_t i = 0; i < 3; ++i) {
                a_context.stringRegisters[a_context.stringRegister--] = "";
            }

            a_context.stringRegister += 1;
        } else if (object == "math") {
            if (method == "Min") {
                if (argc == 2) {
                    a_context.SetIntRegister(std::min<int32_t>(ParseIntExpression(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
                } else {
                    std::cout << "[ERROR]: math.Min must have 2 arguments, this call has " << argc << "." << std::endl;
                }
            } else if (method == "Max") {
                if (argc == 2) {
                    a_context.SetIntRegister(std::max<int32_t>(ParseIntExpression(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
                } else {
                    std::cout << "[ERROR]: math.Max must have 2 arguments, this call has " << argc << "." << std::endl;
                }
            } else if (method == "Clamp") {
                if (argc == 3) {
                    a_context.SetIntRegister(std::clamp<int32_t>(ParseIntExpression(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1]), ParseIntExpression(a_gameState, a_context, args[2])));
                } else {
                    std::cout << "[ERROR]: math.Clamp must have 3 arguments, this call has " << argc << "." << std::endl;
                }
            }

            for (size_t i = 0; i < 3; ++i) {
                a_context.stringRegisters[a_context.stringRegister--] = "";
            }

            a_context.stringRegister += 1;
        } else if (object == "random") {
            if (method == "Range") {
                if (argc == 2) {
                    int32_t minimum = ParseIntExpression(a_gameState, a_context, args[0]);
                    a_context.SetIntRegister(minimum + ((float)a_gameState.generator() / (float)a_gameState.generator.max()) * (ParseIntExpression(a_gameState, a_context, args[1]) - minimum));
                } else {
                    std::cout << "[ERROR]: random.Range must have 2 arguments, this call has " << argc << "." << std::endl;
                }
            } else if (method == "RollDice") {
                if (argc == 2) {
                    a_context.SetIntRegister(a_gameState.RollDice(ParseIntExpression(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
                } else {
                    std::cout << "[ERROR]: random.RollDice must have 2 arguments, this call has " << argc << "." << std::endl;
                }
            }


            for (size_t i = 0; i < 3; ++i) {
                a_context.stringRegisters[a_context.stringRegister--] = "";
            }

            a_context.stringRegister += 1;
        } else if (object == "console") {
            if (method == "Print") {
                if (argc == 1) {
                    std::cout << FormatString(a_gameState, a_context, args[0]);
                } else {
                    std::cout << "[ERROR]: console.Print must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "EatInput") {
                if (argc == 0) {
                    EatInput();
                } else {
                    std::cout << "[ERROR]: console.Print must have 0 arguments, this call has " << argc << "." << std::endl;
                }
            }

            for (size_t i = 0; i < 3; ++i) {
                a_context.stringRegisters[a_context.stringRegister--] = "";
            }

            a_context.stringRegister += 1;
        } else if (object == "room") {
            if (method == "AddInhabitant") {
                if (argc == 3) {
                    std::string entityTemplate = FormatString(a_gameState, a_context, args[0]);
                    std::string npcTemplate = FormatString(a_gameState, a_context, args[1]);
                    std::string modifierTemplates = FormatString(a_gameState, a_context, args[2]);
                    std::vector<std::string> modifiers = std::vector<std::string>();
                    size_t modifierCount = SplitString(modifierTemplates, ":", modifiers);
                    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                    std::cout << std::format("{}({}) = {}", entityTemplate, npcTemplate, modifierTemplates) << std::endl;
                    #endif
                    
                    NPCData npc = NPCData(/*ENTITY_TEMPLATES.at(entityTemplate), NPC_TEMPLATES.at(npcTemplate)*/);
                    for (const std::string& modifier : modifiers) {
                        npc = NPC_MODIFIERS.at(modifier)(npc);
                    }

                    a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(npc);
                } else {
                    std::cout << "[ERROR]: room.AddInhabitant must have 3 arguments, this call has " << argc << "." << std::endl;
                }
            } else if (method == "AddRoomAction") {
                if (argc == 1) {
                    std::string roomAction = FormatString(a_gameState, a_context, args[0]);
                    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                    std::cout << roomAction << std::endl;
                    #endif
                    a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(RoomActionReference { ROOM_ACTIONS.at(roomAction) });
                } else {
                    std::cout << "[ERROR]: room.AddRoomAction must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "AddShop") {
                if (argc == 1) {
                    std::string name = FormatString(a_gameState, a_context, args[0]);
                    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                    std::cout << name << std::endl;
                    #endif
                    a_gameState.rooms[a_gameState.curRoom].data.emplace("Shop", new ShopInstance());
                    GenerateShopInstance(a_gameState, SHOP_DATA.at(name), *(ShopInstance*)a_gameState.rooms[a_gameState.curRoom].data.at("Shop"));
                    a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(RoomActionReference { "Shop" });
                } else {
                    std::cout << "[ERROR]: room.AddShop must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "HasFlag") {
                if (argc == 1) {
                    std::string flag = FormatString(a_gameState, a_context, args[0]);
                    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                    std::cout << flag << std::endl;
                    #endif
                    a_context.SetBoolRegister(a_gameState.rooms[a_gameState.curRoom].flags.contains(flag));
                } else {
                    std::cout << "[ERROR]: room.HasFlag must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "GetFlag") {
                if (argc == 1) {
                    std::string flag = FormatString(a_gameState, a_context, args[0]);
                    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                    std::cout << flag << std::endl;
                    #endif
                    a_context.SetIntRegister(a_gameState.rooms[a_gameState.curRoom].flags.at(flag));
                } else {
                    std::cout << "[ERROR]: room.GetFlag must have 1 argument, this call has " << argc << "." << std::endl;
                }
            } else if (method == "SetFlag") {
                if (argc == 2) {
                    std::string flag = FormatString(a_gameState, a_context, args[0]);
                    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                    std::cout << flag << std::endl;
                    #endif
                    a_gameState.rooms[a_gameState.curRoom].flags.insert_or_assign(flag, ParseIntExpression(a_gameState, a_context, args[1]));
                } else {
                    std::cout << "[ERROR]: room.SetFlag must have 2 arguments, this call has " << argc << "." << std::endl;
                }
            }

            for (size_t i = 0; i < 3; ++i) {
                a_context.stringRegisters[a_context.stringRegister--] = "";
            }

            a_context.stringRegister += 1;
        }

        a_context.mode.pop_back();
    }
};
#pragma endregion

struct Test {
public:
    std::string name = "";
    RegisterType returnType = RegisterType::ERROR;
    std::string code = "";
};

int main(int argc, char** argv) {
    //std::string testString = "A, bcde, efef, hpphuweaf, aew";
    //std::vector<std::string> split = std::vector<std::string>();
    //size_t size = SplitString(testString, ",", split);
    //std::cout << "Size: " << size << std::endl;

    std::vector<Test> tests = {
        {
            "If Test",
            RegisterType::ERROR,
            "{\n\tif (player.HasPerk(ARCANE_EYES)) {\n\t\tconsole.Print(\"There is a mana fountain in the area<comma> but you can't benefit from it.\\n\");\n\t} else {\n\t\tconsole.Print(\"Nothing of interest can be found in the room.\\n\");\n\t} end;\n}"
        },
        {
            "If Not Test",
            RegisterType::ERROR,
            "{\n\tif (!player.HasPerk(ARCANE_EYES)) {\n\t\tconsole.Print(\"There is a mana fountain in the area<comma> but you can't benefit from it.\\n\");\n\t} else {\n\t\tconsole.Print(\"Nothing of interest can be found in the room.\\n\");\n\t} end;\n}"
        },
        {
            "Var Test",
            RegisterType::ERROR,
            "{\n\tmanaGain<uint32> = math.Min(player.maxMana - player.curMana, 25);\n\tconsole.Print(\"you have regained {uint32@manaGain@} mana.\\n\");\n}"
        },
        {
            "Return Test",
            RegisterType::INT,
            "{\n\tmanaGain<uint32> = math.Min(player.maxMana - player.curMana, 25);\n\tconsole.Print(\"you have regained {uint32@manaGain@} mana.\\n\");\n\treturn uint32@manaGain@;\n}"
        },
    };

    GameState gameState = GameState(std::mt19937(std::random_device()()));

    Interpreter interp = Interpreter();
    for (size_t i = 0; i < tests.size(); ++i) {
        std::cout << "Test #" << i << ": " << tests[i].name << "\n" << tests[i].code << "\n\nRunning: " << std::endl;
        int32_t a = interp.ParseStatement<int32_t>(gameState, tests[i].returnType, tests[i].code);
        std::cout << std::endl;

        std::cout << "Returned: " << a << std::endl;
    }

    return 0;
}