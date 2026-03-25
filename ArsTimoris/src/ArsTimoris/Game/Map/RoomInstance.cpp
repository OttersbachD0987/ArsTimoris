#include <ArsTimoris/Game/Map/RoomInstance.h>

RoomInstance::RoomInstance(size_t a_roomID, size_t a_roomIndex) {
    this->roomID = a_roomID;
    this->roomIndex = a_roomIndex;
    this->connections = std::vector<ConnectionInstance>();
    this->inhabitants = std::vector<NPCData>();
    this->roomActions = std::vector<RoomAction>();
    this->flags = std::unordered_map<std::string, int32_t>();
    this->data = std::unordered_map<std::string, void*>();
}

size_t RoomInstance::LivingInhabitants(void) {
    size_t toReturn = 0;
    for (const NPCData& npc : inhabitants) {
        if (npc.curHP > 0) {
            ++toReturn;
        }
    }
    return toReturn;
}