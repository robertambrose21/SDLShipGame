#pragma once

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <memory>
#include <map>
#include <exception>

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
        void operator()(SDL_Window *p) const { }
        void operator()(SDL_Renderer *p) const { }
        void operator()(SDL_Texture *p) const { }
    };

    std::map<std::string, std::shared_ptr<Texture> > loadedTextures;
    std::shared_ptr<SDL_Renderer> renderer;

public:
    TextureLoader(std::shared_ptr<SDL_Renderer> renderer);

    std::shared_ptr<Texture> getTexture(std::string path);

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
