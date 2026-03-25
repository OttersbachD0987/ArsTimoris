#include <ArsTimoris/Game/Inventory/ItemData.h>

ItemData::ItemData(std::string_view a_name, std::string_view a_description, uint32_t a_maxStack) {
    this->name = a_name;
    this->description = a_description;
    this->tags = std::unordered_map<std::string, int32_t>();
    this->usage = ItemUsage();
    this->maxStack = a_maxStack;
}

ItemData::ItemData(std::string_view a_name, std::string_view a_description, const ItemUsage& a_usage, uint32_t a_maxStack) {
    this->name = a_name;
    this->description = a_description;
    this->tags = std::unordered_map<std::string, int32_t>();
    this->usage = a_usage;
    this->maxStack = a_maxStack;
}

ItemData::ItemData(std::string_view a_name, std::string_view a_description, const std::unordered_map<std::string, int32_t>& a_tags, int32_t a_maxStack) {
    this->name = a_name;
    this->description = a_description;
    this->tags = a_tags;
    this->usage = ItemUsage();
    this->maxStack = a_maxStack;
}

ItemData::ItemData(std::string_view a_name, std::string_view a_description, const std::unordered_map<std::string, int32_t>& a_tags, const ItemUsage& a_usage, uint32_t a_maxStack) {
    this->name = a_name;
    this->description = a_description;
    this->tags = a_tags;
    this->usage = a_usage;
    this->maxStack = a_maxStack;
}