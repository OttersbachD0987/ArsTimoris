#include <ArsTimoris/UI/Text/FontAtlas.h>

namespace ArsTimoris::UI::Text { 
    FontAtlas::FontAtlas(std::string_view a_name, uint32_t a_gw, uint32_t a_gh) {
        this->name = a_name;
        this->glyphWidth = a_gw;
        this->glyphHeight = a_gh;
    }

    FontAtlas::~FontAtlas() {

    }
}