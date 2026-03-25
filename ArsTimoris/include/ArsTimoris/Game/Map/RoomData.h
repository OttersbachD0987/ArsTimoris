#pragma once
#ifndef ARS_TIMORIS__GAME__MAP__ROOM_DATA_H
#define ARS_TIMORIS__GAME__MAP__ROOM_DATA_H
#include <string>
#include <functional>
#include <cstdint>
#include <string_view>

struct GameState;

#include <ArsTimoris/Game/Map/ConnectionTest.h>

struct RoomData {
public:
    std::string roomName;
    std::string roomDescription;
    std::function<uint32_t(GameState&, const RoomData&)> weight = nullptr;
    bool backlink;
    uint32_t minExisting;
    uint32_t maxExisting;
    uint32_t minNew;
    uint32_t maxNew;
    ConnectionTest toConnection;
    ConnectionTest fromConnection;

    RoomData(std::string_view a_name, std::string_view a_description, const std::function<uint32_t(GameState&, const RoomData&)>& a_weight, bool a_backlink, uint32_t a_minExisting, uint32_t a_maxExisting, uint32_t a_minNew, uint32_t a_maxNew);
    RoomData(std::string_view a_name, std::string_view a_description, const std::function<uint32_t(GameState&, const RoomData&)>& a_weight, bool a_backlink, uint32_t a_minExisting, uint32_t a_maxExisting, uint32_t a_minNew, uint32_t a_maxNew, ConnectionTest a_to, ConnectionTest a_from);
};
#endif