#include <ArsTimoris/Visual/NPC/NPCImageStep.h>
#include <ArsTimoris/Visual/RenderPassData.h>
#include <ArsTimoris/Assets/TextureAsset.h>

namespace ArsTimoris::Visual::NPC {
    NPCImageStep::NPCImageStep(std::shared_ptr<Assets::TextureAsset> a_image, uint8_t a_alpha, SDL_BlendMode a_blendmode) {
        this->image = a_image;
        this->alpha = a_alpha;
        this->blendmode = a_blendmode;
    }

    void NPCImageStep::Render(SDL_Renderer* a_renderer, SDL_FRect a_rect, float a_time) {
        uint8_t oldAlpha;
        SDL_BlendMode oldBlendmode;
        SDL_GetTextureAlphaMod(image->texture, &oldAlpha);
        SDL_GetTextureBlendMode(image->texture, &oldBlendmode);
        RenderPassData renderPassData = {{0, 0, image->w, image->h}, a_rect, alpha};
        for (std::function<void(RenderPassData*, float)> func : modifiers) {
            func(&renderPassData, a_time);
        }
        SDL_SetTextureAlphaMod(image->texture, renderPassData.alpha);
        SDL_SetTextureBlendMode(image->texture, blendmode);
        SDL_RenderTexture(a_renderer, image->texture, &renderPassData.src, &renderPassData.rect);
        SDL_SetTextureAlphaMod(image->texture, oldAlpha);
        SDL_SetTextureBlendMode(image->texture, oldBlendmode);
    }

    NPCImageStep::~NPCImageStep() {
        image.reset();
    }
}