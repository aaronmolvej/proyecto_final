#ifndef SCREEN_H
#define SCREEN_H

#include <SDL.h>
#include "TextRenderer.h"
#include "InputManager.h"

class Screen {
public:
    virtual ~Screen() {}
    
    virtual void handleInput(InputManager& input) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer* renderer, TextRenderer* textRenderer) = 0;
    virtual void onEnter() = 0;
};

#endif
