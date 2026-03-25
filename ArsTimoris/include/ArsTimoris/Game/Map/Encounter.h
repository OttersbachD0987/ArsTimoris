#pragma once
#ifndef ARS_TIMORIS__GAME__MAP__ENCOUNTER_H
#define ARS_TIMORIS__GAME__MAP__ENCOUNTER_H
#include <string>
#include <vector>
#include <filesystem>

struct Encounter {
public:
    std::string weight = "";
    std::string event = "";
};

std::vector<Encounter> LoadEncountersData(std::filesystem::path a_path);
#endif