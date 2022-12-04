#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <map>
#include <exception>
#include <fstream>
#include <iostream>

#include "core/json.hpp"
#include "core/util/gameassert.h"

using json = nlohmann::json;

class Texture {
private:
    struct sdl_deleter {
        void operator()(SDL_Texture *p) const { }
    };

    std::shared_ptr<SDL_Texture> texture;
    std::string id;

public:
    Texture() { }
    Texture(std::shared_ptr<SDL_Texture> texture, std::string id);

    std::string getId(void) const;
    void draw(
        std::shared_ptr<SDL_Renderer> renderer,
        const SDL_Rect* srcRect = NULL,
        const SDL_Rect* dstRect = NULL
    );
};

class TextureLoader {
private:
    struct sdl_deleter {
        void operator()(SDL_Window *p) const    { }
        void operator()(SDL_Renderer *p) const  { }
        void operator()(SDL_Texture *p) const   { }
        void operator()(SDL_Surface *p) const   { }
        void operator()(SDL_RWops *p) const     { }
    };

    typedef struct _textureData {
        uint8_t id;
        std::string name;
        std::string path;
    } TextureData;

    std::map<uint8_t, TextureData> availableTextures;
    std::map<std::string, std::shared_ptr<Texture>> loadedTextures;
    std::shared_ptr<SDL_Renderer> renderer;

public:
    TextureLoader(std::shared_ptr<SDL_Renderer> renderer);

    std::shared_ptr<Texture> loadTexture(const uint8_t& id);
    std::shared_ptr<Texture> loadTexture(const std::string& path);

    class TextureLoaderException : public std::exception {
    private:
        std::string message;

    public:
        virtual const char* what() const throw() {
            return std::string("TextureLoaderException occurred: " + message).c_str();
        }

        TextureLoaderException(const std::string& message) :
            message(message)
        { }
    };
};
