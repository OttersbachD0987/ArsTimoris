#include <ArsTimoris/Util/Random.hpp>

std::uniform_real_distribution<float> PERCENT_DISTRIBUTION = std::uniform_real_distribution<float>(0, 1);

std::uniform_int_distribution<uint32_t> DIE_TWO_DISTRIBUTION     = std::uniform_int_distribution<uint32_t>(1, 2);
std::uniform_int_distribution<uint32_t> DIE_THREE_DISTRIBUTION   = std::uniform_int_distribution<uint32_t>(1, 3);
std::uniform_int_distribution<uint32_t> DIE_FOUR_DISTRIBUTION    = std::uniform_int_distribution<uint32_t>(1, 4);
std::uniform_int_distribution<uint32_t> DIE_SIX_DISTRIBUTION     = std::uniform_int_distribution<uint32_t>(1, 6);
std::uniform_int_distribution<uint32_t> DIE_EIGHT_DISTRIBUTION   = std::uniform_int_distribution<uint32_t>(1, 8);
std::uniform_int_distribution<uint32_t> DIE_TEN_DISTRIBUTION     = std::uniform_int_distribution<uint32_t>(1, 10);
std::uniform_int_distribution<uint32_t> DIE_TWELVE_DISTRIBUTION  = std::uniform_int_distribution<uint32_t>(1, 12);
std::uniform_int_distribution<uint32_t> DIE_SIXTEEN_DISTRIBUTION = std::uniform_int_distribution<uint32_t>(1, 16);
std::uniform_int_distribution<uint32_t> DIE_TWENTY_DISTRIBUTION  = std::uniform_int_distribution<uint32_t>(1, 20);
std::uniform_int_distribution<uint32_t> DIE_HUNDRED_DISTRIBUTION = std::uniform_int_distribution<uint32_t>(1, 100);

std::unordered_map<uint32_t, std::uniform_int_distribution<uint32_t>&> DICE = {
    {  2, DIE_TWO_DISTRIBUTION    },
    {  3, DIE_THREE_DISTRIBUTION  },
    {  4, DIE_FOUR_DISTRIBUTION   },
    {  6, DIE_SIX_DISTRIBUTION    },
    {  8, DIE_EIGHT_DISTRIBUTION  },
    { 10, DIE_TEN_DISTRIBUTION    },
    { 12, DIE_TWELVE_DISTRIBUTION },
    { 16, DIE_SIXTEEN_DISTRIBUTION},
    { 20, DIE_TWENTY_DISTRIBUTION },
    {100, DIE_HUNDRED_DISTRIBUTION},
};