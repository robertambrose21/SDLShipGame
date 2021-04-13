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
{ }

Texture TextureLoader::getTexture(std::string path) {
    if(loadedTextures.contains(path)) {
        return loadedTextures[path];
    }

    auto surface = std::unique_ptr<SDL_Surface>(IMG_Load(path.c_str()));

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

    loadedTextures[path] = Texture(std::move(texture), path);

    return loadedTextures[path];
}