#include <ArsTimoris/UI/Text/TextObject.h>

namespace ArsTimoris::UI::Text { 
    TextObject::TextObject(std::shared_ptr<FontAtlas>& a_font, float a_pt, TextSegment a_text) {
        this->font = a_font;
        this->pt = a_pt;
        this->text = a_text;
    }
}