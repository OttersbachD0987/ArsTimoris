#pragma once
#ifndef ARS_TIMORIS__DATA_COMPONENTS__DATA_CONTAINER_TPP
#define ARS_TIMORIS__DATA_COMPONENTS__DATA_CONTAINER_TPP
#include <ArsTimoris/DataComponents/DataContainer.h>

namespace DataComponents {
    template<typename T>
    T DataContainer::Get(const std::string& a_key) {
        return std::any_cast<T>(data.at(a_key).data);
    }
}
#endif