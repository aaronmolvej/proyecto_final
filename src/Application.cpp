#include "Application.h"
#include "MenuScreen.h"
#include "Config.h"
#include <iostream>
#include <string>

Application::Application()
    : window(nullptr), renderer(nullptr), running(false),
      currentScreen(nullptr), frameStart(0), frameTime(0),
      currentState(AppState::INTRO), logoTexture(nullptr),
      introMusic(nullptr), menuMusic(nullptr),
      introTimer(0.0f), introMusicStarted(false)
{}

Application::~Application() {
    cleanup();
}

bool Application::init() {
    std::cout << "iniciando Consola" << std::endl;
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Error SDL_Init: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "SDL inicializado" << std::endl;

    int mappingsAdded = SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
    if (mappingsAdded == -1) {
        std::cout << "Advertencia: No se encontro gamecontrollerdb.txt" << std::endl;
    } else {
        std::cout << "Se cargaron " << mappingsAdded << " mapeos de controles." << std::endl;
    }

    std::cout << "Buscando controles conectados..." << std::endl;
    inputManager.initGamepad();

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "Error IMG_Init: " << IMG_GetError() << std::endl;
        return false;
    }
    std::cout << "SDL_image inicializado" << std::endl;

    if (TTF_Init() == -1) {
        std::cerr << "Error TTF_Init: " << TTF_GetError() << std::endl;
        return false;
    }
    std::cout << "SDL_ttf inicializado" << std::endl;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Error Mix_OpenAudio: " << Mix_GetError() << std::endl;
        return false;
    }
    std::cout << "SDL_mixer inicializado" << std::endl;

    window = SDL_CreateWindow(
        "Consola Retro - ProyectoFinalEmbebidos",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    if (!window) {
        std::cerr << "Error al crear ventana: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "Ventana creada en pantalla completa" << std::endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Error al crear renderer: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "Renderer creado" << std::endl;

    if (!textRenderer.loadFont(FONT_PATH + "font.ttf", 16)) {
        std::cerr << "Advertencia: No se pudo cargar la fuente" << std::endl;
    } else {
        std::cout << "Fuente cargada" << std::endl;
    }

    if (!loadIntroAssets()) {
        std::cerr << "Advertencia: No se pudieron cargar todos los assets de intro" << std::endl;
        currentState = AppState::MENU;
    }

    std::cout << "Escaneando ROMs" << std::endl;
    romManager.scanROMs(renderer);
    
    if (currentState == AppState::MENU) {
        currentScreen = new MenuScreen(&romManager);
        currentScreen->onEnter();
        playMenuMusic();
    }

    running = true;
    std::cout << "Consola Retro en FullScreen" << std::endl;
    std::cout << "Controles: Flechas, Enter, F, C, ESC" << std::endl;
    
    return true;
}

bool Application::loadIntroAssets() {
    std::string logoPath = "media/logo/logo.jpg";
    SDL_Surface* logoSurface = IMG_Load(logoPath.c_str());
    
    if (!logoSurface) {
        std::cerr << "No se pudo cargar logo: " << IMG_GetError() << std::endl;
        return false;
    }
    
    logoTexture = SDL_CreateTextureFromSurface(renderer, logoSurface);
    SDL_FreeSurface(logoSurface);
    
    if (!logoTexture) {
        std::cerr << "No se pudo crear textura del logo: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "Logo cargado correctamente" << std::endl;

    std::string introMusicPath = "media/intro/intro.mp3";
    introMusic = Mix_LoadMUS(introMusicPath.c_str());
    
    if (!introMusic) {
        std::cerr << "No se pudo cargar música de intro: " << Mix_GetError() << std::endl;
    } else {
        std::cout << "Música de intro cargada" << std::endl;
        Mix_PlayMusic(introMusic, 0);
        introMusicStarted = true;
    }

    std::string menuMusicPath = "media/menu/menu.mp3";
    menuMusic = Mix_LoadMUS(menuMusicPath.c_str());
    
    if (!menuMusic) {
        std::cerr << "No se pudo cargar música del menú: " << Mix_GetError() << std::endl;
    } else {
        std::cout << "Música del menú cargada" << std::endl;
    }

    return (logoTexture != nullptr);
}

void Application::renderIntroScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (logoTexture) {
        int logoW = 0, logoH = 0;
        SDL_QueryTexture(logoTexture, nullptr, nullptr, &logoW, &logoH);

        int windowW = 0, windowH = 0;
        SDL_GetRendererOutputSize(renderer, &windowW, &windowH);

        float scale = std::min(
            1.0f, 
            std::min((float)windowW / logoW, (float)windowH / logoH)
        );
        int destW = static_cast<int>(logoW * scale);
        int destH = static_cast<int>(logoH * scale);

        int x = (windowW - destW) / 2;
        int y = (windowH - destH) / 2;

        SDL_Rect destRect = { x, y, destW, destH };
        SDL_RenderCopy(renderer, logoTexture, nullptr, &destRect);
    }

    SDL_RenderPresent(renderer);
}



void Application::transitionToMenu() {
    std::cout << "Transición al menú principal" << std::endl;
    
    currentState = AppState::MENU;
    
    if (!currentScreen) {
        currentScreen = new MenuScreen(&romManager);
        currentScreen->onEnter();
    }    
    playMenuMusic();
}

void Application::playMenuMusic() {
    if (menuMusic) {
        Mix_PlayMusic(menuMusic, -1);
        std::cout << "Música del menú en loop" << std::endl;
    }
}

void Application::run() {
    while (running) {
        frameStart = SDL_GetTicks();
        handleEvents();
        update();
        render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
}

void Application::handleEvents() {
    SDL_Event event;
    inputManager.clear();

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cout << "[DEBUG] Salida por evento SDL_QUIT" << std::endl;
            running = false;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }

        if (currentState == AppState::INTRO) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN || 
                    event.key.keysym.sym == SDLK_SPACE) {
                    transitionToMenu();
                }
            }
        }

        inputManager.update(event);
    }

    if (currentScreen && currentState == AppState::MENU) {
        currentScreen->handleInput(inputManager);

        if (inputManager.isCancelPressed()) {
             std::cout << "[DEBUG] Botón CANCELAR detectado por InputManager" << std::endl;
        }
    }
}

void Application::update() {
    float deltaTime = frameTime / 1000.0f;

    if (currentState == AppState::INTRO) {
        introTimer += deltaTime;
        
        if (introTimer >= INTRO_DURATION || !Mix_PlayingMusic()) {
            transitionToMenu();
        }
    } 
    else if (currentState == AppState::MENU && currentScreen) {
        currentScreen->update(deltaTime);
    }
}

void Application::render() {
    if (currentState == AppState::INTRO) {
        renderIntroScreen();
    } 
    else if (currentState == AppState::MENU) {
        SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
        SDL_RenderClear(renderer);

        if (currentScreen) {
            currentScreen->render(renderer, &textRenderer);
        }

        SDL_RenderPresent(renderer);
    }
}

void Application::cleanup() {
    std::cout << "Cerrando Consola Retro" << std::endl;
    std::cout << "Guardando datos" << std::endl;
    
    romManager.saveData();

    inputManager.close();

    if (currentScreen) {
        delete currentScreen;
        currentScreen = nullptr;
    }

    if (logoTexture) {
        SDL_DestroyTexture(logoTexture);
        logoTexture = nullptr;
    }

    if (introMusic) {
        Mix_FreeMusic(introMusic);
        introMusic = nullptr;
    }

    if (menuMusic) {
        Mix_FreeMusic(menuMusic);
        menuMusic = nullptr;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    std::cout << "Aplicación cerrada" << std::endl;
}
