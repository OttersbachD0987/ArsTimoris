#pragma once
#ifndef ARS_TIMORIS__UI__TEXT__TEXT_OBJECT_H
#define ARS_TIMORIS__UI__TEXT__TEXT_OBJECT_H
#include <memory>
#include <ArsTimoris/UI/Text/TextSegment.h>

namespace ArsTimoris::UI::Text {
    struct FontAtlas;

    struct TextObject {
        std::shared_ptr<FontAtlas> font;
        float pt;
        TextSegment text;

        TextObject(std::shared_ptr<FontAtlas>& a_font, float a_pt, TextSegment a_text);
    };
}
#endif