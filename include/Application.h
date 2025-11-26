#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include "Screen.h"
#include "InputManager.h"
#include "TextRenderer.h"
#include "ROMManager.h"

enum class AppState {
    INTRO,
    MENU
};

class Application {
public:
    Application();
    ~Application();

    bool init();
    void run();
    void cleanup();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    InputManager inputManager;
    TextRenderer textRenderer;
    ROMManager romManager;

    Screen* currentScreen;
    
    Uint32 frameStart;
    int frameTime;

    AppState currentState;
    SDL_Texture* logoTexture;
    Mix_Music* introMusic;
    Mix_Music* menuMusic;
    
    float introTimer;
    const float INTRO_DURATION = 5.0f;
    bool introMusicStarted;

    void handleEvents();
    void update();
    void render();
    
    bool loadIntroAssets();
    void renderIntroScreen();
    void transitionToMenu();
    void playMenuMusic();
};

#endif
