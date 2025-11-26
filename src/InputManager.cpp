#include "InputManager.h"
#include <cstring>

InputManager::InputManager() 
    : gamepad(nullptr), mouseX(0), mouseY(0), 
      mouseState(0), previousMouseState(0) {
    memset(previousKeyboardState, 0, sizeof(previousKeyboardState));
    memset(gamepadButtons, 0, sizeof(gamepadButtons));
    memset(previousGamepadButtons, 0, sizeof(previousGamepadButtons));
    keyboardState = SDL_GetKeyboardState(nullptr);
   // initGamepad();
}

InputManager::~InputManager() {
    close();
}

void InputManager::initGamepad() {
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            gamepad = SDL_GameControllerOpen(i);
            if (gamepad) {
                break;
            }
        }
    }
}

void InputManager::update(SDL_Event& event) {
    mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    if (gamepad) {
        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
            gamepadButtons[i] = SDL_GameControllerGetButton(
                gamepad, static_cast<SDL_GameControllerButton>(i));
        }
    }
    if (event.type == SDL_CONTROLLERDEVICEADDED && !gamepad) {
        initGamepad();
    } else if (event.type == SDL_CONTROLLERDEVICEREMOVED && gamepad) {
        SDL_GameControllerClose(gamepad);
        gamepad = nullptr;
    }
}

void InputManager::clear() {
    const Uint8* currentState = SDL_GetKeyboardState(nullptr);
    memcpy(previousKeyboardState, currentState, SDL_NUM_SCANCODES);
    previousMouseState = mouseState;
    memcpy(previousGamepadButtons, gamepadButtons, sizeof(previousGamepadButtons));
}

bool InputManager::isKeyPressed(SDL_Keycode key) const {
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
    const Uint8* currentState = SDL_GetKeyboardState(nullptr);
    return currentState[scancode] && !previousKeyboardState[scancode];
}

bool InputManager::isKeyDown(SDL_Keycode key) const {
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
    const Uint8* currentState = SDL_GetKeyboardState(nullptr);
    return currentState[scancode];
}

bool InputManager::isMouseButtonPressed(Uint8 button) const {
    Uint32 mask = SDL_BUTTON(button);
    return (mouseState & mask) && !(previousMouseState & mask);
}

bool InputManager::isGamepadButtonPressed(SDL_GameControllerButton button) const {
    if (!gamepad) return false;
    return gamepadButtons[button] && !previousGamepadButtons[button];
}

bool InputManager::isUpPressed() const {
    return isKeyPressed(SDLK_UP) || 
           isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_DPAD_UP);
}

bool InputManager::isDownPressed() const {
    return isKeyPressed(SDLK_DOWN) || 
           isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
}

bool InputManager::isLeftPressed() const {
    return isKeyPressed(SDLK_LEFT) || 
           isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
}

bool InputManager::isRightPressed() const {
    return isKeyPressed(SDLK_RIGHT) || 
           isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
}

bool InputManager::isConfirmPressed() const {
    return isKeyPressed(SDLK_RETURN) || 
           isKeyPressed(SDLK_SPACE) ||
           isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_A);
}

bool InputManager::isCancelPressed() const {
    return isKeyPressed(SDLK_ESCAPE) || 
           isKeyPressed(SDLK_BACKSPACE) ||
           isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_B);
}

void InputManager::close() {
    if (gamepad) {
        SDL_GameControllerClose(gamepad);
        gamepad = nullptr;
    }
}
