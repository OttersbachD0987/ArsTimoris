#pragma once
#ifndef ARS_TIMORIS__UI__TEXT__FONT_ATLAS_H
#define ARS_TIMORIS__UI__TEXT__FONT_ATLAS_H
#include <string>
#include <string_view>
#include <cstdint>

struct SDL_Surface;
struct SDL_Color;

namespace ArsTimoris::UI::Text {
    struct FontAtlas {
        std::string name;

        uint32_t glyphWidth;
        uint32_t glyphHeight;
        float atlasWidth;
        float atlasHeight;
        SDL_Surface* atlas;

        FontAtlas(std::string_view a_name, uint32_t a_gw, uint32_t a_gh);

        SDL_Surface* RenderWrapped(std::string a_text, float a_size, SDL_Color a_color, int32_t a_wrapWidth);

        ~FontAtlas();
    };
}
#endif