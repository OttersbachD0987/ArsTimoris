#include <ArsTimoris/Assets/LazyFontAsset.h>
#include <print>

namespace ArsTimoris::Assets {
    LazyFontAsset::LazyFontAsset(std::string a_id, std::string a_name) {
        this->id = a_id;
        this->name = a_name;
        this->sizes = std::unordered_map<float, std::shared_ptr<FontAsset>>();
    }

    std::shared_ptr<FontAsset> LazyFontAsset::GetFontSize(float a_size) {
        if (sizes.contains(a_size)) {
            return sizes.at(a_size);
        }
        std::println("LazyFont Font: {} {} {}", path, name, a_size);
        std::shared_ptr<FontAsset> fontAsset = std::make_shared<FontAsset>(path, name, a_size);
        fontAsset->Load(path);
        sizes.emplace(std::piecewise_construct,
            std::forward_as_tuple(a_size), 
            std::forward_as_tuple(std::move(fontAsset)));
        return sizes.at(a_size);
    }

    void LazyFontAsset::Load(std::string a_path) {
        this->path = a_path;
    }

    void LazyFontAsset::Unload(void) {
        for (std::pair<float, std::shared_ptr<FontAsset>> fontPair : sizes) {
            fontPair.second->Unload();
        }
    }
}