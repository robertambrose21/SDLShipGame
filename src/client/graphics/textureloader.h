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

    struct sdl_deleter {
        void operator()(SDL_Texture *p) const {
            SDL_DestroyTexture(p);
        }
    };

    inline static const std::string NoID = "NO_ID_INTERNAL";

private:
    std::unique_ptr<SDL_Texture, sdl_deleter> texture;
    std::string id;
    Colour colour;
    uint8_t alpha;

public:
    Texture();
    Texture(std::unique_ptr<SDL_Texture, sdl_deleter> texture);
    Texture(std::unique_ptr<SDL_Texture, sdl_deleter> texture, const std::string& id);

    std::string getId(void) const;
    void draw(
        SDL_Renderer* renderer,
        const SDL_Rect* srcRect = NULL,
        const SDL_Rect* dstRect = NULL,
        double rotation = 0.0
    );

    void draw(
        SDL_Renderer* renderer,
        const Colour& colour,
        uint8_t alpha,
        const SDL_Rect* srcRect = NULL,
        const SDL_Rect* dstRect = NULL,
        double rotation = 0.0
    );

    void setColour(const Colour& colour);
    Colour getColour(void) const;
    void setAlpha(uint8_t alpha);
    uint8_t getAlpha(void) const;

    SDL_Texture* getSDLTexture(void);
};

class TextureLoader {
private:
    struct sdl_deleter {
        void operator()(SDL_Texture *p) const { /*SDL_DestroyTexture(p);*/ }
        void operator()(SDL_Surface *p) const { /*SDL_FreeSurface(p);*/ }
        void operator()(SDL_RWops *p) const   { /*SDL_FreeRW(p);*/ }
    };

    typedef struct _textureData {
        uint32_t id;
        std::string name;
        std::string path;
    } TextureData;

    std::map<uint32_t, TextureData> availableTextures;
    std::map<std::string, std::unique_ptr<Texture>> loadedTextures;
    SDL_Renderer* renderer;

public:
    TextureLoader();
    TextureLoader(SDL_Renderer* renderer);

    Texture* loadTexture(uint32_t id);
    Texture* loadTexture(const std::string& path);

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
