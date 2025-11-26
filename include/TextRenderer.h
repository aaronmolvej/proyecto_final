#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <map>

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    
    bool loadFont(const std::string& path, int size);
    
    void renderText(SDL_Renderer* renderer, const std::string& text,
                   int x, int y, SDL_Color color, bool centered = false);
    
    void renderText(SDL_Renderer* renderer, const std::string& text,
                   int x, int y, SDL_Color color, int size, bool centered);

private:
    std::map<int, TTF_Font*> fonts;
    TTF_Font* defaultFont;
};

#endif
