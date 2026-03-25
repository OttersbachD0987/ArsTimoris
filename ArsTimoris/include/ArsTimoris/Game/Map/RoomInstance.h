#pragma once
#ifndef ARS_TIMORIS__GAME__MAP__ROOM_INSTANCE_H
#define ARS_TIMORIS__GAME__MAP__ROOM_INSTANCE_H
struct RoomInstance;

#include <ArsTimoris/Game/Interaction/RoomAction.h>
#include <ArsTimoris/Game/Entity/NPCData.h>
#include <ArsTimoris/Game/Map/ConnectionInstance.h>
#include <ArsTimoris/Game/Map/RoomData.h>

struct RoomInstance {
public:
    bool initialized = false;
    size_t roomID;
    size_t roomIndex;
    std::vector<ConnectionInstance> connections;
    std::vector<NPCData> inhabitants;
    std::vector<RoomAction> roomActions;
    std::unordered_map<std::string, int32_t> flags;
    std::unordered_map<std::string, void*> data;

    RoomInstance(size_t a_roomID, size_t a_roomIndex);

    size_t LivingInhabitants(void);
};
#endif