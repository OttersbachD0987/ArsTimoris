#include <ArsTimoris/Game/Map/RoomData.h>


RoomData::RoomData(std::string_view a_name, std::string_view a_description, const std::function<uint32_t(GameState&, const RoomData&)>& a_weight, bool a_backlink, uint32_t a_minExisting, uint32_t a_maxExisting, uint32_t a_minNew, uint32_t a_maxNew) {
    this->roomName = a_name;
    this->roomDescription = a_description;
    this->weight = a_weight;
    this->backlink = a_backlink;
    this->minExisting = a_minExisting;
    this->maxExisting = a_maxExisting;
    this->minNew = a_minNew;
    this->maxNew = a_maxNew;
    this->toConnection = ConnectionTest();
    this->fromConnection = ConnectionTest();
}

RoomData::RoomData(std::string_view a_name, std::string_view a_description, const std::function<uint32_t(GameState&, const RoomData&)>& a_weight, bool a_backlink, uint32_t a_minExisting, uint32_t a_maxExisting, uint32_t a_minNew, uint32_t a_maxNew, ConnectionTest a_to, ConnectionTest a_from) {
    this->roomName = a_name;
    this->roomDescription = a_description;
    this->weight = a_weight;
    this->backlink = a_backlink;
    this->minExisting = a_minExisting;
    this->maxExisting = a_maxExisting;
    this->minNew = a_minNew;
    this->maxNew = a_maxNew;
    this->toConnection = a_to;
    this->fromConnection = a_from;
}