#include <ArsTimoris/UI/Text/FontAtlas.h>
#include <SDL3/SDL.h>
#include <vector>
#include <print>

static enum class FontRenderEscapeCode {
    COLOR,
    SHLFT
};

namespace ArsTimoris::UI::Text { 
    FontAtlas::FontAtlas(std::string_view a_name, uint32_t a_gw, uint32_t a_gh) {
        this->name = a_name;
        this->glyphWidth = a_gw;
        this->glyphHeight = a_gh;
    }

    SDL_Surface* FontAtlas::RenderWrapped(std::string a_text, float a_size, SDL_Color a_color, int32_t a_wrapWidth) {
        if (a_wrapWidth <= 0) {
            a_wrapWidth = std::numeric_limits<int32_t>::max();
        }   
        //std::println("Rendering: {}", a_text);
        int32_t paddedGlyphWidth = glyphWidth + 2;
        int32_t paddedGlyphHeight = glyphHeight + 2;
        int32_t glyphsPerLine = (int32_t)floorf(((float)a_wrapWidth + 2.0f * a_size) / ((float)paddedGlyphWidth * a_size));
        int32_t x = 0;
        int32_t foundWidth = 0;
        int32_t curLine = 0;
        std::vector<int32_t> lineLengths = {0};
        bool escaped = false;
        bool collecting = false;
        for (char c : a_text) {
            switch (c) {
                case '\\': {
                    if (escaped && ++x >= glyphsPerLine) {
                        lineLengths[curLine++] = --x;
                        if (curLine >= lineLengths.size()) {
                            lineLengths.push_back(0);
                        }
                        foundWidth = (++x > foundWidth) ? x : foundWidth;
                        x = 0;
                    }
                    escaped = !escaped;
                    break;
                }
                case ':':
                case ',': {
                    if (!collecting && ++x >= glyphsPerLine) {
                        lineLengths[curLine++] = --x;
                        if (curLine >= lineLengths.size()) {
                            lineLengths.push_back(0);
                        }
                        foundWidth = (++x > foundWidth) ? x : foundWidth;
                        x = 0;
                    }
                    escaped = false;
                    break;
                }
                case '[': {
                    if (escaped) {
                        collecting = true;
                        escaped = false;
                    } else if (++x >= glyphsPerLine) {
                        lineLengths[curLine++] = --x;
                        if (curLine >= lineLengths.size()) {
                            lineLengths.push_back(0);
                        }
                        foundWidth = (++x > foundWidth) ? x : foundWidth;
                        x = 0;
                    }
                    break;
                }
                case ']': {
                    if (collecting) {
                        collecting = false;
                    } else if (++x >= glyphsPerLine) {
                        lineLengths[curLine++] = --x;
                        if (curLine >= lineLengths.size()) {
                            lineLengths.push_back(0);
                        }
                        foundWidth = (++x > foundWidth) ? x : foundWidth;
                        x = 0;
                    }
                    escaped = false;
                    break;
                }
                case '\b': {
                    if (--x < 0) {
                        x = lineLengths[--curLine];
                    }
                    escaped = false;
                    break;
                }
                case 'b': {
                    if (!collecting) {
                        if (escaped) {
                            if (--x < 0) {
                                x = lineLengths[--curLine];
                            }
                        } else if (++x >= glyphsPerLine) {
                            lineLengths[curLine++] = --x;
                            if (curLine >= lineLengths.size()) {
                                lineLengths.push_back(0);
                            }
                            foundWidth = (++x > foundWidth) ? x : foundWidth;
                            x = 0;
                        }   
                    }
                    escaped = false;
                    break;
                }
                case '\n': {
                    lineLengths[curLine++] = x;
                    if (curLine >= lineLengths.size()) {
                        lineLengths.push_back(0);
                    }
                    foundWidth = (++x > foundWidth) ? x : foundWidth;
                    x = 0;
                    escaped = false;
                    break;
                }
                case 'n': {
                    if (!collecting && (escaped || ++x >= glyphsPerLine)) {
                        lineLengths[curLine++] = escaped ? x : --x;
                        if (curLine >= lineLengths.size()) {
                            lineLengths.push_back(0);
                        }
                        foundWidth = (++x > foundWidth) ? x : foundWidth;
                        x = 0;
                    }
                    escaped = false;
                    break;
                }
                default: {
                    if (!collecting && ++x >= glyphsPerLine) {
                        lineLengths[curLine++] = --x;
                        if (curLine >= lineLengths.size()) {
                            lineLengths.push_back(0);
                        }
                        foundWidth = (++x > foundWidth) ? x : foundWidth;
                        x = 0;
                    }
                    escaped = false;
                    break;
                }
            }
        }
        foundWidth = (x > foundWidth) ? x : foundWidth;
        //std::println("GPL : FW = {} : {}", glyphsPerLine, foundWidth);
        //std::println("String ({}) is bounded by {} lines of {} characters.", a_text, lines, foundWidth);
        int32_t y = x = 0;
        escaped = collecting = false;
        int32_t lines = lineLengths.size();
        int32_t width = paddedGlyphWidth * foundWidth - 2;
        int32_t height = paddedGlyphHeight * lines - 2;
        std::string accum = "";
        SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
        int32_t base = 10;
        int32_t ci = 0;
        FontRenderEscapeCode escapeCode = FontRenderEscapeCode::COLOR;
        SDL_SetSurfaceColorMod(atlas, color.r, color.g, color.b);
        SDL_SetSurfaceAlphaMod(atlas, color.a);
        //std::println("GPL: {} Size: ({}, {})", glyphsPerLine, width, height);
        //std::println("Arg: {} {} {} {}", a_text, a_text.size(), a_size, a_wrapWidth);
        //std::println("Val: ({}, {}) ({}, {})", glyphWidth, glyphHeight, atlasWidth, atlasHeight);
        //std::println("Cal: ({}, {}) {} {} ({}, {})", paddedGlyphWidth, paddedGlyphHeight, glyphsPerLine, lines, width, height);
        glyphsPerLine = foundWidth;
        SDL_Surface* surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
        SDL_Rect src = {0, 0, (int)glyphWidth, (int)glyphHeight};
        SDL_Rect dst = {0, 0, (int)glyphWidth, (int)glyphHeight};
        for (size_t i = 0; i < a_text.size(); ++i) {
            switch (a_text[i]) {
                case '\\': {
                    if (escaped) {
                        src.x = ((uint8_t)a_text[i] % 16) * glyphWidth;
                        src.y = ((uint8_t)a_text[i] / 16) * glyphHeight;
                        dst.x = x * paddedGlyphWidth;
                        dst.y = y * paddedGlyphHeight;
                        //std::println("{:0>4}) [{} | {:>3}] pos ({:>3}, {:>3}) src ({:>3}, {:>3}) dst ({:>4}, {:>4})", i, a_text[i], (uint8_t)a_text[i], x, y, src.x, src.y, dst.x, dst.y);
                        SDL_BlitSurfaceUnchecked(atlas, &src, surface, &dst);
                        if (++x >= glyphsPerLine) {
                            lineLengths[y++] = --x;
                            x = 0;
                        }
                    }
                    escaped = !escaped;
                    break;
                }
                case ':': {
                    if (collecting) {
                        if (accum == "FCB") {
                            escapeCode = FontRenderEscapeCode::COLOR;
                            base = 2;
                        } else if (accum == "FCO") {
                            escapeCode = FontRenderEscapeCode::COLOR;
                            base = 8;
                        } else if (accum == "FCD") {
                            escapeCode = FontRenderEscapeCode::COLOR;
                            base = 10;
                        } else if (accum == "FCH") {
                            escapeCode = FontRenderEscapeCode::COLOR;
                            base = 16;
                        }
                        accum = "";
                    } else {
                        src.x = ((uint8_t)a_text[i] % 16) * glyphWidth;
                        src.y = ((uint8_t)a_text[i] / 16) * glyphHeight;
                        dst.x = x * paddedGlyphWidth;
                        dst.y = y * paddedGlyphHeight;
                        //std::println("{:0>4}) [{} | {:>3}] pos ({:>3}, {:>3}) src ({:>3}, {:>3}) dst ({:>4}, {:>4})", i, a_text[i], (uint8_t)a_text[i], x, y, src.x, src.y, dst.x, dst.y);
                        SDL_BlitSurfaceUnchecked(atlas, &src, surface, &dst);
                        if (++x >= glyphsPerLine) {
                            lineLengths[y++] = --x;
                            x = 0;
                        }
                    }
                    escaped = false;
                    break;
                }
                case ',': {
                    if (collecting) {
                        switch (escapeCode) {
                            case FontRenderEscapeCode::COLOR: {
                                switch (ci++) {
                                    case 0: {
                                        color.r = std::stoi(accum, (size_t*)nullptr, base);
                                        accum = "";
                                        break;
                                    }
                                    case 1: {
                                        color.g = std::stoi(accum, (size_t*)nullptr, base);
                                        accum = "";
                                        break;
                                    }
                                    case 2: {
                                        color.b = std::stoi(accum, (size_t*)nullptr, base);
                                        accum = "";
                                        break;
                                    }
                                    case 3: {
                                        color.a = std::stoi(accum, (size_t*)nullptr, base);
                                        accum = "";
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    } else {
                        src.x = ((uint8_t)a_text[i] % 16) * glyphWidth;
                        src.y = ((uint8_t)a_text[i] / 16) * glyphHeight;
                        dst.x = x * paddedGlyphWidth;
                        dst.y = y * paddedGlyphHeight;
                        //std::println("{:0>4}) [{} | {:>3}] pos ({:>3}, {:>3}) src ({:>3}, {:>3}) dst ({:>4}, {:>4})", i, a_text[i], (uint8_t)a_text[i], x, y, src.x, src.y, dst.x, dst.y);
                        SDL_BlitSurfaceUnchecked(atlas, &src, surface, &dst);
                        if (++x >= glyphsPerLine) {
                            lineLengths[y++] = --x;
                            x = 0;
                        }
                    }
                    escaped = false;
                    break;
                }
                case '[': {
                    if (escaped) {
                        collecting = true;
                        escaped = false;
                    } else {
                        src.x = ((uint8_t)a_text[i] % 16) * glyphWidth;
                        src.y = ((uint8_t)a_text[i] / 16) * glyphHeight;
                        dst.x = x * paddedGlyphWidth;
                        dst.y = y * paddedGlyphHeight;
                        //std::println("{:0>4}) [{} | {:>3}] pos ({:>3}, {:>3}) src ({:>3}, {:>3}) dst ({:>4}, {:>4})", i, a_text[i], (uint8_t)a_text[i], x, y, src.x, src.y, dst.x, dst.y);
                        SDL_BlitSurfaceUnchecked(atlas, &src, surface, &dst);
                        if (++x >= glyphsPerLine) {
                            lineLengths[y++] = --x;
                            x = 0;
                        }
                    }
                    break;
                }
                case ']': {
                    if (collecting) {
                        switch (escapeCode) {
                            case FontRenderEscapeCode::COLOR: {
                                switch (ci) {
                                    case 0: {
                                        color.r = std::stoi(accum, (size_t*)nullptr, base);
                                        accum = "";
                                        break;
                                    }
                                    case 1: {
                                        color.g = std::stoi(accum, (size_t*)nullptr, base);
                                        accum = "";
                                        break;
                                    }
                                    case 2: {
                                        color.b = std::stoi(accum, (size_t*)nullptr, base);
                                        accum = "";
                                        break;
                                    }
                                    case 3: {
                                        color.a = std::stoi(accum, (size_t*)nullptr, base);
                                        accum = "";
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        ci = 0;
                        collecting = false;
                        switch (escapeCode) {
                            case FontRenderEscapeCode::COLOR: {
                                //std::println("Finished Color Code: ({}, {}, {}, {})", color.r, color.g, color.b, color.a);
                                SDL_SetSurfaceColorMod(atlas, color.r, color.g, color.b);
                                SDL_SetSurfaceAlphaMod(atlas, color.a);
                                break;
                            }
                        }
                    } else {
                        src.x = ((uint8_t)a_text[i] % 16) * glyphWidth;
                        src.y = ((uint8_t)a_text[i] / 16) * glyphHeight;
                        dst.x = x * paddedGlyphWidth;
                        dst.y = y * paddedGlyphHeight;
                        //std::println("{:0>4}) [{} | {:>3}] pos ({:>3}, {:>3}) src ({:>3}, {:>3}) dst ({:>4}, {:>4})", i, a_text[i], (uint8_t)a_text[i], x, y, src.x, src.y, dst.x, dst.y);
                        SDL_BlitSurfaceUnchecked(atlas, &src, surface, &dst);
                        if (++x >= glyphsPerLine) {
                            lineLengths[y++] = --x;
                            x = 0;
                        }
                    }
                    escaped = false;
                    break;
                }
                case '\b': {
                    if (--x < 0) {
                        x = lineLengths[--y];
                        //std::println("New pos: ({}, {})", x, y);
                    }
                    escaped = false;
                    break;
                }
                case 'b': {
                    if (collecting) {
                        accum += a_text[i];
                    } else if (escaped) {
                        if (--x < 0) {
                            x = lineLengths[--y];
                            //std::println("New pos: ({}, {})", x, y);
                        }
                    } else {
                        src.x = ((uint8_t)a_text[i] % 16) * glyphWidth;
                        src.y = ((uint8_t)a_text[i] / 16) * glyphHeight;
                        dst.x = x * paddedGlyphWidth;
                        dst.y = y * paddedGlyphHeight;
                        //std::println("{:0>4}) [{} | {:>3}] pos ({:>3}, {:>3}) src ({:>3}, {:>3}) dst ({:>4}, {:>4})", i, a_text[i], (uint8_t)a_text[i], x, y, src.x, src.y, dst.x, dst.y);
                        SDL_BlitSurfaceUnchecked(atlas, &src, surface, &dst);
                        if (++x >= glyphsPerLine) {
                            lineLengths[y++] = --x;
                            x = 0;
                        }
                    }
                    escaped = false;
                    break;
                }
                case '\n': {
                    lineLengths[y++] = x;
                    x = 0;
                    escaped = false;
                    break;
                }
                case 'n': {
                    if (collecting) {
                        accum += a_text[i];
                    } else if (escaped) {
                        lineLengths[y++] = x;
                        x = 0;
                    } else {
                        src.x = ((uint8_t)a_text[i] % 16) * glyphWidth;
                        src.y = ((uint8_t)a_text[i] / 16) * glyphHeight;
                        dst.x = x * paddedGlyphWidth;
                        dst.y = y * paddedGlyphHeight;
                        //std::println("{:0>4}) [{} | {:>3}] pos ({:>3}, {:>3}) src ({:>3}, {:>3}) dst ({:>4}, {:>4})", i, a_text[i], (uint8_t)a_text[i], x, y, src.x, src.y, dst.x, dst.y);
                        SDL_BlitSurfaceUnchecked(atlas, &src, surface, &dst);
                        if (++x >= glyphsPerLine) {
                            lineLengths[y++] = --x;
                            x = 0;
                        }
                    }
                    escaped = false;
                    break;
                }
                default: {
                    if (collecting) {
                        accum += a_text[i];
                    } else {
                        src.x = ((uint8_t)a_text[i] % 16) * glyphWidth;
                        src.y = ((uint8_t)a_text[i] / 16) * glyphHeight;
                        dst.x = x * paddedGlyphWidth;
                        dst.y = y * paddedGlyphHeight;
                        //std::println("{:0>4}) [{} | {:>3}] pos ({:>3}, {:>3}) src ({:>3}, {:>3}) dst ({:>4}, {:>4})", i, a_text[i], (uint8_t)a_text[i], x, y, src.x, src.y, dst.x, dst.y);
                        SDL_BlitSurfaceUnchecked(atlas, &src, surface, &dst);
                        if (++x >= glyphsPerLine) {
                            lineLengths[y++] = --x;
                            x = 0;
                        }
                    }
                    escaped = false;
                    break;
                }
            }
        }
        //std::println("{}", surface->w);
        //std::println("{}", surface->h);
        return surface;
    }

    FontAtlas::~FontAtlas() {
        if (atlas != nullptr) {
            SDL_DestroySurface(atlas);
            atlasWidth = 0;
            atlasHeight = 0;
            glyphWidth = 0;
            glyphHeight = 0;
        }
    }
}