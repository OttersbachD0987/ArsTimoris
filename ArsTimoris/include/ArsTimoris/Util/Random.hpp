#pragma once
#ifndef ARS_TIMORIS__UTIL__DICE_HPP
#define ARS_TIMORIS__UTIL__DICE_HPP
#include <cstdint>
#include <random>
#include <unordered_map>

extern std::uniform_real_distribution<float> PERCENT_DISTRIBUTION;

extern std::uniform_int_distribution<uint32_t> DIE_TWO_DISTRIBUTION;
extern std::uniform_int_distribution<uint32_t> DIE_THREE_DISTRIBUTION;
extern std::uniform_int_distribution<uint32_t> DIE_FOUR_DISTRIBUTION;
extern std::uniform_int_distribution<uint32_t> DIE_SIX_DISTRIBUTION;
extern std::uniform_int_distribution<uint32_t> DIE_EIGHT_DISTRIBUTION;
extern std::uniform_int_distribution<uint32_t> DIE_TEN_DISTRIBUTION;
extern std::uniform_int_distribution<uint32_t> DIE_TWELVE_DISTRIBUTION;
extern std::uniform_int_distribution<uint32_t> DIE_SIXTEEN_DISTRIBUTION;
extern std::uniform_int_distribution<uint32_t> DIE_TWENTY_DISTRIBUTION;
extern std::uniform_int_distribution<uint32_t> DIE_HUNDRED_DISTRIBUTION;

extern std::unordered_map<uint32_t, std::uniform_int_distribution<uint32_t>&> DICE;
#endif