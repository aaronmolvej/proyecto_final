#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <SDL.h>

class InputManager {
public:
    InputManager();
    ~InputManager();

    void initGamepad();
    void close();

    void update(SDL_Event& event);
    void clear();

    bool isKeyPressed(SDL_Keycode key) const;
    bool isKeyDown(SDL_Keycode key) const;
    
    bool isMouseButtonPressed(Uint8 button) const;
    int getMouseX() const { return mouseX; }
    int getMouseY() const { return mouseY; }
    
    bool isGamepadConnected() const { return gamepad != nullptr; }
    bool isGamepadButtonPressed(SDL_GameControllerButton button) const;

    bool isUpPressed() const;
    bool isDownPressed() const;
    bool isLeftPressed() const;
    bool isRightPressed() const;
    bool isConfirmPressed() const;
    bool isCancelPressed() const;

private:
    const Uint8* keyboardState;
    Uint8 previousKeyboardState[SDL_NUM_SCANCODES];
    
    int mouseX, mouseY;
    Uint32 mouseState;
    Uint32 previousMouseState;
    
    SDL_GameController* gamepad;
    Uint8 gamepadButtons[SDL_CONTROLLER_BUTTON_MAX];
    Uint8 previousGamepadButtons[SDL_CONTROLLER_BUTTON_MAX];

};

#endif
