#include <ArsTimoris/Assets/Assets.h>
#include <iostream>

namespace ArsTimoris::Assets {
    TextureAsset::TextureAsset(std::string a_id, std::string a_name) {
        this->id = a_id;
        this->name = a_name;
    }

    void ArsTimoris::Assets::TextureAsset::Load(SDL_Renderer* a_renderer, std::string a_path) {
        this->texture = IMG_LoadTexture(a_renderer, a_path.c_str());
        if (this->texture == nullptr) {
            std::cout << "Error creating texture: " << SDL_GetError() << std::endl;
        }

        SDL_SetTextureScaleMode(this->texture, SDL_SCALEMODE_NEAREST);
        //SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);

        if (!SDL_GetTextureSize(this->texture, &this->w, &this->h)) {
            std::cout << "Error getting size: " << SDL_GetError() << std::endl;
        }
    }

    void ArsTimoris::Assets::TextureAsset::Unload(void) {
        SDL_DestroyTexture(this->texture);
    }
}