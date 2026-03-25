#pragma once
#ifndef ARS_TIMORIS__GAME__CLASSES__CLASS_INSTANCE_H
#define ARS_TIMORIS__GAME__CLASSES__CLASS_INSTANCE_H
#include <string>
#include <cstdint>
#include <string_view>

struct ClassInstance {
public:
    std::string classType;
    uint32_t level;

    ClassInstance(void);
    ClassInstance(std::string_view a_class, uint32_t a_level);
};
#endif