#include "Button.h"
#include "TextRenderer.h"
#include "Config.h"

Button::Button(int x, int y, int width, int height, const std::string& text)
    : text(text), hovered(false), selected(false), enabled(true), onClick(nullptr)
{
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;
}

Button::~Button() {}

void Button::update(int mouseX, int mouseY) {
    hovered = (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
               mouseY >= rect.y && mouseY <= rect.y + rect.h);
}

bool Button::isClicked(int mouseX, int mouseY) {
    return (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
            mouseY >= rect.y && mouseY <= rect.y + rect.h);
}

void Button::click() {
    if (enabled && onClick) {
        onClick();
    }
}

void Button::render(SDL_Renderer* renderer, TextRenderer* textRenderer) {
    SDL_Color bgColor;
    
    if (!enabled) {
        bgColor = {50, 50, 70, 255};
    } else if (selected) {
        bgColor = SELECTED_COLOR;
    } else if (hovered) {
        bgColor = BUTTON_HOVER_COLOR;
    } else {
        bgColor = BUTTON_COLOR;
    }
    
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &rect);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    SDL_RenderDrawRect(renderer, &rect);
    
    int textY = rect.y + (rect.h / 2) - 8;
    
    SDL_Color textColor = enabled ? TEXT_COLOR : TEXT_SECONDARY_COLOR;
    textRenderer->renderText(renderer, text, rect.x + rect.w / 2, textY, textColor, true);
}
