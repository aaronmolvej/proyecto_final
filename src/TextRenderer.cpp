#include "TextRenderer.h"
#include <iostream>

TextRenderer::TextRenderer() : defaultFont(nullptr) {
}

TextRenderer::~TextRenderer() {
    for (auto& pair : fonts) {
        if (pair.second) {
            TTF_CloseFont(pair.second);
        }
    }
    fonts.clear();
    defaultFont = nullptr;
}

bool TextRenderer::loadFont(const std::string& path, int size) {
    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (!font) {
        std::cerr << "Error loading font: " << TTF_GetError() << std::endl;
        return false;
    }
    
    fonts[size] = font;
    
    if (!defaultFont) {
        defaultFont = font;
    }
    
    std::cout << "Fuente cargada: " << path << " (tamaño " << size << ")" << std::endl;
    return true;
}

void TextRenderer::renderText(SDL_Renderer* renderer, const std::string& text,
                              int x, int y, SDL_Color color, bool centered) {
    if (!defaultFont) {
        std::cerr << "No hay fuente cargada" << std::endl;
        return;
    }
    
    SDL_Surface* surface = TTF_RenderText_Blended(defaultFont, text.c_str(), color);
    if (!surface) {
        std::cerr << "Error al renderizar texto: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Error al crear textura de texto: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect destRect;
    destRect.w = surface->w;
    destRect.h = surface->h;
    destRect.x = centered ? x - destRect.w / 2 : x;
    destRect.y = y;
    
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void TextRenderer::renderText(SDL_Renderer* renderer, const std::string& text,
                              int x, int y, SDL_Color color, int size, bool centered) {
    TTF_Font* font = nullptr;
    
    if (fonts.find(size) != fonts.end()) {
        font = fonts[size];
    } else {
        font = defaultFont;
    }
    
    if (!font) {
        std::cerr << "No hay fuente disponible para tamaño " << size << std::endl;
        return;
    }
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Error al renderizar texto: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Error al crear textura de texto: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect destRect;
    destRect.w = surface->w;
    destRect.h = surface->h;
    destRect.x = centered ? x - destRect.w / 2 : x;
    destRect.y = y;
    
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}
