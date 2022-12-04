#include "textureloader.h"

Texture::Texture(std::shared_ptr<SDL_Texture> texture, std::string id) :
    texture(texture),
    id(id)
{ }

std::string Texture::getId(void) const {
    return id;
}

void Texture::draw(
    std::shared_ptr<SDL_Renderer> renderer,
    const SDL_Rect* srcRect,
    const SDL_Rect* dstRect
) {
    SDL_RenderCopy(renderer.get(), texture.get(), srcRect, dstRect);
}

TextureLoader::TextureLoader(std::shared_ptr<SDL_Renderer> renderer) :
    renderer(renderer)
{
    std::ifstream f("../assets/data/textures/textures.json");
    json data = json::parse(f);

    auto texturesData = data["textures"].get<std::vector<json>>();
    game_assert(!texturesData.empty());

    for(auto textureData : texturesData) {
        auto id = textureData["id"].get<uint8_t>();
        auto name = textureData["name"].get<std::string>();
        auto path = "../assets/data/" + textureData["path"].get<std::string>();

        availableTextures[id] = { id, name, path };
    }

    game_assert(availableTextures.size() == texturesData.size());
}

std::shared_ptr<Texture> TextureLoader::loadTexture(const uint8_t& id) {
    return loadTexture(availableTextures[id].path);
}

std::shared_ptr<Texture> TextureLoader::loadTexture(const std::string& path) {
    if(loadedTextures.contains(path)) {
        return loadedTextures[path];
    }

    auto surface = std::unique_ptr<SDL_Surface, sdl_deleter>(
        IMG_Load(path.c_str()),
        sdl_deleter()
    );

    if(surface == nullptr) {
        throw TextureLoaderException("Unable to load image: \'" + path + "\': " + SDL_GetError());
    }

    auto texture = std::unique_ptr<SDL_Texture, sdl_deleter>(
        SDL_CreateTextureFromSurface(renderer.get(), surface.get()),
        sdl_deleter()
    );

    if(texture == nullptr) {
        throw TextureLoaderException("Unable to create texture from \'" + path + "\': " + SDL_GetError());
    }

    loadedTextures[path] = std::make_shared<Texture>(std::move(texture), path);

    SDL_FreeSurface(surface.get());

    return loadedTextures[path];
}