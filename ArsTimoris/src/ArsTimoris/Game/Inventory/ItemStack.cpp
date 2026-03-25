#include <ArsTimoris/Game/Inventory/ItemStack.h>

ItemStack::ItemStack(void) {
    this->itemID = 0;
    this->stackSize = 0;
    this->metadata = std::unordered_map<std::string, int32_t>();
}

ItemStack::ItemStack(size_t a_itemID, uint32_t a_stackSize) {
    this->itemID = a_itemID;
    this->stackSize = a_stackSize;
    this->metadata = std::unordered_map<std::string, int32_t>();
}

ItemStack::ItemStack(size_t a_itemID, uint32_t a_stackSize, const std::unordered_map<std::string, int32_t>& a_metadata) {
    this->itemID = a_itemID;
    this->stackSize = a_stackSize;
    this->metadata = a_metadata;
}

int32_t ItemStack::GetMetadata(const std::string& a_key, int32_t a_default) {
    return metadata.contains(a_key) ? metadata.at(a_key) : a_default;
}

ItemStack ItemStack::FromString(std::string_view a_serialized) {
    ItemStack itemStack;
    std::string collector = "";
    std::string stringStorage = "";
    int32_t intStorage = 0;
    ItemStackDeserializationMode mode = ItemStackDeserializationMode::COLLECTING_ID;
    for (char character : a_serialized) {
        switch (mode) {
            case ItemStackDeserializationMode::COLLECTING_ID:
                switch (character) {
                    case 'x':
                        intStorage = std::stoi(collector);
                        collector = "";
                        mode = ItemStackDeserializationMode::COLLECTING_STACK;
                        break;
                    default:
                        collector += character;
                }
                break;
            case ItemStackDeserializationMode::COLLECTING_STACK:
                switch (character) {
                    case '(':
                        itemStack = ItemStack(intStorage, std::stoi(collector));
                        collector = "";
                        mode = ItemStackDeserializationMode::COLLECTING_METAKEY;
                        break;
                    case ';':
                        itemStack = ItemStack(intStorage, std::stoi(collector));
                        collector = "";
                        mode = ItemStackDeserializationMode::FINISHED;
                        break;
                    default:
                        collector += character;
                }
                break;
            case ItemStackDeserializationMode::COLLECTING_METAKEY:
                switch (character) {
                    case ':':
                        stringStorage = collector;
                        collector = "";
                        mode = ItemStackDeserializationMode::COLLECTING_METAVALUE;
                        break;
                    default:
                        collector += character;
                }
                break;
            case ItemStackDeserializationMode::COLLECTING_METAVALUE:
                switch (character) {
                    case ';':
                        itemStack.metadata.emplace(stringStorage, std::stoi(collector));
                        collector = "";
                        mode = ItemStackDeserializationMode::COLLECTING_METAKEY;
                        break;
                    case ')':
                        itemStack.metadata.emplace(stringStorage, std::stoi(collector));
                        collector = "";
                        mode = ItemStackDeserializationMode::FINISHED;
                        break;
                    default:
                        collector += character;
                }
                break;
        }
    }

    return itemStack;
}