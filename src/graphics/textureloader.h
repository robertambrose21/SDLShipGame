#pragma once

#include "sdlimport.h"
#include <memory>
#include <map>
#include <exception>
#include <fstream>
#include <iostream>

#include "core/json.hpp"
#include "core/util/gameassert.h"

using json = nlohmann::json;

class Texture {
public:
    typedef struct _colour {
        uint8_t r, g, b;

        _colour(uint8_t r, uint8_t g, uint8_t b) :
            r(r),
            g(g),
            b(b)
        { }
    } Colour;

private:
    struct sdl_deleter {
        void operator()(SDL_Texture *p) const { }
    };

    std::shared_ptr<SDL_Texture> texture;
    std::string id;
    Colour colour;
    uint8_t alpha;

public:
    Texture();
    Texture(const std::shared_ptr<SDL_Texture>& texture, const std::string& id);

    std::string getId(void) const;
    void draw(
        const std::shared_ptr<SDL_Renderer>& renderer,
        const SDL_Rect* srcRect = NULL,
        const SDL_Rect* dstRect = NULL
    );

    void draw(
        const std::shared_ptr<SDL_Renderer>& renderer,
        const Colour& colour,
        uint8_t alpha,
        const SDL_Rect* srcRect = NULL,
        const SDL_Rect* dstRect = NULL
    );

    void setColour(const Colour& colour);
    Colour getColour(void) const;
    void setAlpha(uint8_t alpha);
    uint8_t getAlpha(void) const;
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
        uint32_t id;
        std::string name;
        std::string path;
    } TextureData;

    std::map<uint32_t, TextureData> availableTextures;
    std::map<std::string, std::shared_ptr<Texture>> loadedTextures;
    std::shared_ptr<SDL_Renderer> renderer;

public:
    TextureLoader(const std::shared_ptr<SDL_Renderer>& renderer);

    std::shared_ptr<Texture> loadTexture(uint32_t id);
    std::shared_ptr<Texture> loadTexture(const std::string& path);

    class TextureLoaderException : public std::exception {
    private:
        std::string message;

    public:
        virtual const char* what() const throw() {
            return message.c_str();
        }

        TextureLoaderException(const std::string& message) :
            message(std::string("TextureLoaderException occurred: " + message))
        { }
    };
};
