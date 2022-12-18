#include "textureloader.h"

Texture::Texture(const std::shared_ptr<SDL_Texture>& texture, const std::string& id) :
    texture(texture),
    id(id),
    colour({ 0xFF, 0xFF, 0xFF }),
    alpha(0xFF)
{ }

std::string Texture::getId(void) const {
    return id;
}

void Texture::draw(
    const std::shared_ptr<SDL_Renderer>& renderer,
    const SDL_Rect* srcRect,
    const SDL_Rect* dstRect
) {
    draw(renderer, this->colour, this->alpha, srcRect, dstRect);
}

void Texture::draw(
    const std::shared_ptr<SDL_Renderer>& renderer,
    const Colour& colour,
    uint8_t alpha,
    const SDL_Rect* srcRect,
    const SDL_Rect* dstRect
) {
    SDL_SetTextureAlphaMod(texture.get(), alpha);
    SDL_SetTextureColorMod(texture.get(), colour.r, colour.g, colour.b);
    SDL_RenderCopy(renderer.get(), texture.get(), srcRect, dstRect);
}

void Texture::setColour(const Colour& colour) {
    this->colour = colour;
}

Texture::Colour Texture::getColour(void) const {
    return colour;
}

void Texture::setAlpha(uint8_t alpha) {
    this->alpha = alpha;
}

uint8_t Texture::getAlpha(void) const {
    return alpha;
}

TextureLoader::TextureLoader(const std::shared_ptr<SDL_Renderer>& renderer) :
    renderer(renderer)
{
    std::ifstream f("../assets/data/textures/textures.json");
    json data = json::parse(f);

    auto const& texturesData = data["textures"].get<std::vector<json>>();
    game_assert(!texturesData.empty());

    for(auto const& textureData : texturesData) {
        auto id = textureData["id"].get<uint32_t>();
        auto name = textureData["name"].get<std::string>();
        auto path = "../assets/data/" + textureData["path"].get<std::string>();

        availableTextures[id] = { id, name, path };
    }

    game_assert(availableTextures.size() == texturesData.size());
}

std::shared_ptr<Texture> TextureLoader::loadTexture(uint32_t id) {
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