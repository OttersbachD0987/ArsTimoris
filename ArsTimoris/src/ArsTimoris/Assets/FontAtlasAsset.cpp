#include <ArsTimoris/Assets/FontAtlasAsset.h>
#include <ArsTimoris/UI/Text/FontAtlas.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

namespace ArsTimoris::Assets {
    FontAtlasAsset::FontAtlasAsset(std::string a_id, std::string a_name) {
        this->id = a_id;
        this->name = a_name;
    }

    void FontAtlasAsset::Load(SDL_Renderer* a_renderer, std::string a_path) {
        this->fontAtlas = std::make_shared<ArsTimoris::UI::Text::FontAtlas>(name, 0, 0);
        this->fontAtlas->atlas =  IMG_Load(a_path.c_str());
        if (this->fontAtlas->atlas == nullptr) {
            std::cout << a_path << std::endl;
            std::cout << "Error loading font atlas image: " << SDL_GetError() << std::endl;
        }

        this->fontAtlas->atlasWidth = this->fontAtlas->atlas->w;
        this->fontAtlas->atlasHeight = this->fontAtlas->atlas->h;

        /*
         1 - 1.0000
         2 - 0.5000
         4 - 0.2500
         8 - 0.1250
        16 - 0.0625
        */

        this->fontAtlas->glyphWidth = this->fontAtlas->atlasWidth * 0.0625f;
        this->fontAtlas->glyphHeight = this->fontAtlas->atlasHeight * 0.0625f;
    }

    void FontAtlasAsset::Unload(void) {
        this->fontAtlas.reset();
    }
}