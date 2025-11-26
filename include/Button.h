#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>
#include <string>
#include <functional>

class TextRenderer;

class Button {
public:
    Button(int x, int y, int width, int height, const std::string& text);
    ~Button();
    
    bool isEnabled() const { return enabled; }
    void update(int mouseX, int mouseY);
    bool isClicked(int mouseX, int mouseY);
    void click();
    
    void render(SDL_Renderer* renderer, TextRenderer* textRenderer);
    
    void setOnClick(std::function<void()> callback) { onClick = callback; }
    void setText(const std::string& newText) { text = newText; }
    void setSelected(bool sel) { selected = sel; }
    void setEnabled(bool en) { enabled = en; }
    
    void setRect(int x, int y, int width, int height) {
        rect.x = x;
        rect.y = y;
        rect.w = width;
        rect.h = height;
    }
    
    SDL_Rect rect;
    
private:
    std::string text;
    bool hovered;
    bool selected;
    bool enabled;
    std::function<void()> onClick;
};

#endif
