#include <ArsTimoris/DataComponents/DataContainer.h>

namespace DataComponents {
    DataHolder::DataHolder(std::string_view a_key, const DataType& a_dataType, const std::any a_data, size_t a_size) {
        this->key = a_key;
        this->type = a_dataType;
        this->data = a_data;
        this->size = a_size;
    }
    
    void DataHolder::SetData(std::any a_data, size_t a_size) {
        this->data = a_data;
        this->size = a_size;
    }

    DataHolder::~DataHolder() {
        this->data.reset();
    }
    
    DataContainer::DataContainer(void) {
        this->data = std::unordered_map<std::string, DataHolder>();
    }

    void DataContainer::Set(const std::string& a_key, DataType a_dataType, const std::any a_data, size_t a_size) {
        data.emplace(a_key, DataHolder(a_key, a_dataType, a_data, a_size));
    }

    void DataContainer::Del(const std::string& a_key) {
        data.erase(a_key);
    }

    DataContainer ParseDataFile(std::filesystem::path a_path) {
        std::ifstream reader = std::ifstream(a_path.string());
        DataContainer container = DataContainer();
        ReaderMode mode = ReaderMode::COLLECTING_NAME;
        bool inString = false;
        bool escaped = false;
        char character = 0;
        std::string collectorName = "";
        std::string collectorType = "";
        std::string collectorData = "";
        DataType dataType = DataType::INVALID;
        while ((character = reader.get()) != -1) {
            //std::cout << character;
            switch (mode) {
                case ReaderMode::COLLECTING_NAME:
                    switch (character) {
                        case ' ':
                        case '\t':
                        case '\v':
                        case '\n':
                            continue;
                        case '<':
                            mode = ReaderMode::COLLECTING_TYPE;
                            break;
                        default:
                            collectorName += character;
                            break;
                    }
                    break;
                case ReaderMode::COLLECTING_TYPE:
                    switch (character) {
                        case ' ':
                        case '\t':
                        case '\v':
                        case '\n':
                            continue;
                        case '>':
                            if (collectorType == "uint8") {
                                dataType = DataType::UINT8;
                            } else if (collectorType == "uint16") {
                                dataType = DataType::UINT16;
                            } else if (collectorType == "uint32") {
                                dataType = DataType::UINT32;
                            } else if (collectorType == "uint64") {
                                dataType = DataType::UINT64;
                            } else if (collectorType == "int8") {
                                dataType = DataType::INT8;
                            } else if (collectorType == "int16") {
                                dataType = DataType::INT16;
                            } else if (collectorType == "int32") {
                                dataType = DataType::INT32;
                            } else if (collectorType == "int64") {
                                dataType = DataType::INT64;
                            } else if (collectorType == "bool") {
                                dataType = DataType::BOOL;
                            } else if (collectorType == "string") {
                                dataType = DataType::STRING;
                            } else if (collectorType == "float") {
                                dataType = DataType::FLOAT;
                            }  else {
                                dataType = DataType::INVALID;
                                //std::cout << "Error, invalid type \"" << collectorType << "\"." << std::endl;
                            }
                            mode = ReaderMode::COLLECTING_DELM;
                            break;
                        default:
                            collectorType += character;
                            break;
                    }
                    break;
                case ReaderMode::COLLECTING_DELM:
                    switch (character) {
                        case ' ':
                        case '\t':
                        case '\v':
                        case '\n':
                            continue;
                        case ':':
                            mode = ReaderMode::COLLECTING_DATA;
                            break;
                        default:
                            //std::cout << "Invalid char between delimeter \"" << character << "\"" << std::endl;
                            break;
                    }
                    break;
                case ReaderMode::COLLECTING_DATA:
                    switch (character) {
                        case ' ':
                        case '\t':
                        case '\v':
                        case '\n':
                            if (inString) {
                                collectorData += character;
                            }
                            escaped = false;
                            continue;
                        case '\\':
                            if (escaped) {
                                collectorData += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                            break;
                        case ';':
                            if (escaped || inString) {
                                collectorData += character;
                            } else {
                                mode = ReaderMode::COLLECTING_NAME;
                                switch (dataType) {
                                    case DataType::UINT8: {
                                        uint8_t data = std::stoul(collectorData);
                                        container.Set(collectorName, dataType, data, sizeof(uint8_t));
                                        break;
                                    }
                                    case DataType::UINT16: {
                                        uint16_t data = std::stoul(collectorData);
                                        container.Set(collectorName, dataType, data, sizeof(uint16_t));
                                        break;
                                    }
                                    case DataType::UINT32: {
                                        uint32_t data = std::stoul(collectorData);
                                        container.Set(collectorName, dataType, data, sizeof(uint32_t));
                                        break;
                                    }
                                    case DataType::UINT64: {
                                        uint64_t data = std::stoull(collectorData);
                                        container.Set(collectorName, dataType, data, sizeof(uint64_t));
                                        break;
                                    }
                                    case DataType::INT8: {
                                        int8_t data = std::stoi(collectorData);
                                        container.Set(collectorName, dataType, data, sizeof(int8_t));
                                        break;
                                    }
                                    case DataType::INT16: {
                                        int16_t data = std::stoi(collectorData);
                                        container.Set(collectorName, dataType, data, sizeof(int16_t));
                                        break;
                                    }
                                    case DataType::INT32: {
                                        int32_t data = std::stoi(collectorData);
                                        container.Set(collectorName, dataType, data, sizeof(int32_t));
                                        break;
                                    }
                                    case DataType::INT64: {
                                        int64_t data = std::stoll(collectorData);
                                        container.Set(collectorName, dataType, data, sizeof(int64_t));
                                        break;
                                    }
                                    case DataType::BOOL: {
                                        std::transform(collectorData.begin(), collectorData.end(), collectorData.begin(), [](unsigned char a_character) { return std::tolower(a_character);});
                                        container.Set(collectorName, dataType, collectorData == "true", sizeof(bool));
                                        break;
                                    }
                                    case DataType::STRING: {
                                        container.Set(collectorName, dataType, (collectorData + '\0').c_str(), collectorData.size() + 1);
                                        break;
                                    }
                                    case DataType::FLOAT: {
                                        float data = std::stof(collectorData);
                                        container.Set(collectorName, dataType, data, sizeof(float));
                                        break;
                                    }
                                }
                                collectorName = "";
                                collectorType = "";
                                collectorData = "";
                            }
                            escaped = false;
                            break;
                        case '\"':
                            if (escaped) {
                                collectorData += '\"';
                            } else {
                                inString = !inString;
                            }
                            escaped = false;
                            break;
                        default:
                            if (escaped) {
                                switch (character) {
                                    case 't':
                                        collectorData += '\t';
                                    case 'v':
                                        collectorData += '\v';
                                    case 'n':
                                        collectorData += '\n';
                                    default:
                                        collectorData += character;
                                }
                            } else {
                                collectorData += character;
                            }
                            escaped = false;
                            break;
                    }
                    break;
            }
        }

        reader.close();

        return container;
    }
}