#include "textureloader.h"

Texture::Texture() :
    Texture(nullptr)
{ }

Texture::Texture(std::unique_ptr<SDL_Texture, sdl_deleter> texture) :
    Texture(std::move(texture), NoID)
{ }

Texture::Texture(std::unique_ptr<SDL_Texture, sdl_deleter> texture, const std::string& id) :
    texture(std::move(texture)),
    id(id),
    colour({ 0xFF, 0xFF, 0xFF }),
    alpha(0xFF)
{ }

std::string Texture::getId(void) const {
    return id;
}

void Texture::draw(
    SDL_Renderer* renderer,
    const SDL_Rect* srcRect,
    const SDL_Rect* dstRect
) {
    draw(renderer, this->colour, this->alpha, srcRect, dstRect);
}

void Texture::draw(
    SDL_Renderer* renderer,
    const Colour& colour,
    uint8_t alpha,
    const SDL_Rect* srcRect,
    const SDL_Rect* dstRect
) {
    SDL_SetTextureAlphaMod(texture.get(), alpha);
    SDL_SetTextureColorMod(texture.get(), colour.r, colour.g, colour.b);
    SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(renderer, texture.get(), srcRect, dstRect);
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

SDL_Texture* Texture::getSDLTexture(void) {
    return texture.get();
}

TextureLoader::TextureLoader()
{ }

TextureLoader::TextureLoader(SDL_Renderer* renderer) :
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

Texture* TextureLoader::loadTexture(uint32_t id) {
    if(!availableTextures.contains(id)) {
        throw TextureLoaderException("Unable to create texture from id \'" + std::to_string(id) 
            + "\': id is not associated with any available textures");
    }

    return loadTexture(availableTextures[id].path);
}

Texture* TextureLoader::loadTexture(const std::string& path) {
    if(loadedTextures.contains(path)) {
        return loadedTextures[path].get();
    }

    auto surface = IMG_Load(path.c_str());

    if(surface == nullptr) {
        throw TextureLoaderException("Unable to load image: \'" + path + "\': " + SDL_GetError());
    }

    auto texture = std::unique_ptr<SDL_Texture, Texture::sdl_deleter>(
        SDL_CreateTextureFromSurface(renderer, surface),
        Texture::sdl_deleter()
    );

    if(texture == nullptr) {
        throw TextureLoaderException("Unable to create texture from \'" + path + "\': " + SDL_GetError());
    }

    loadedTextures[path] = std::make_unique<Texture>(std::move(texture), path);

    SDL_FreeSurface(surface);

    return loadedTextures[path].get();
}