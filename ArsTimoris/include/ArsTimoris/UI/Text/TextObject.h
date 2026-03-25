#pragma once
#ifndef ARS_TIMORIS__UI__TEXT__TEXT_OBJECT_H
#define ARS_TIMORIS__UI__TEXT__TEXT_OBJECT_H
#include <memory>
#include <string>
#include <string_view>

namespace ArsTimoris::UI::Text {
    struct FontAtlas;

    struct TextObject {
        std::shared_ptr<FontAtlas> font;
        float pt;
        std::string text;

        TextObject(std::shared_ptr<FontAtlas>& a_font, float a_pt, std::string_view a_text);
    };
}
#endif