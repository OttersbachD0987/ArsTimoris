#pragma once
#ifndef ARS_TIMORIS__UI__TEXT__FONT_ATLAS_H
#define ARS_TIMORIS__UI__TEXT__FONT_ATLAS_H
#include <string>
#include <cstdint>

struct SDL_Texture;

namespace ArsTimoris::UI::Text {
    struct FontAtlas {
        std::string name;

        uint32_t glyphWidth;
        uint32_t glyphHeight;
        uint32_t atlasWidth;
        uint32_t atlasHeight;
        SDL_Texture* atlas;
    };
}
#endif