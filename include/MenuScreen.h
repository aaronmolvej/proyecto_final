#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include "Screen.h"
#include "ROMManager.h"
#include "Button.h"
#include "InputManager.h"
#include "TextRenderer.h"
#include "USBDetector.h"
#include "Config.h"
#include <vector>


class MenuScreen : public Screen {
public:
    MenuScreen(ROMManager* romManager);
    ~MenuScreen();

    void onEnter() override;
    void handleInput(InputManager& input) override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, TextRenderer* textRenderer) override;

    void checkAndCopyUSBROMs(SDL_Renderer* renderer);
    bool isUSBConnected();

private:
    ROMManager* romManager;
    Tab currentTab;
    Console selectedConsole;
    int selectedROMIndex;
    int selectedFileIndex;

    std::vector<Button*> tabButtons;
    std::vector<Button*> consoleButtons;
    std::vector<ROM*> displayedROMs;
    std::vector<std::string> availableForAdd;

    Button* btnFavorite;
    Button* btnPlay;
    Button* btnContinue;
    Button* btnAddROM;
    Button* btnDeleteROM;
    Button* btnRescan;

    USBDetector usbDetector;
    bool usbCheckInProgress;
    float usbCheckTimer;
    const float USB_CHECK_INTERVAL = 2.0f;

    std::string statusMessage;
    float statusMessageTimer;

    void updateDisplayedROMs();
    void updateButtonStates();
    void renderTabs(SDL_Renderer* renderer, TextRenderer* textRenderer);
    void renderConsoleButtons(SDL_Renderer* renderer, TextRenderer* textRenderer);
    void renderROMs(SDL_Renderer* renderer, TextRenderer* textRenderer);
    void renderButtons(SDL_Renderer* renderer, TextRenderer* textRenderer);
    void renderTopBar(SDL_Renderer* renderer, TextRenderer* textRenderer);
    void renderROMDetailNew(SDL_Renderer* renderer, TextRenderer* textRenderer);
    void renderROMListVertical(SDL_Renderer* renderer, TextRenderer* textRenderer);

    ROM* getSelectedROM();
    void addROMFromPath(const std::string& path);
    void deleteSelectedROM();
    void rescanROMs();
    void renderStatusMessage(SDL_Renderer* renderer, TextRenderer* textRenderer);
    void setStatusMessage(const std::string& msg, float duration = 3.0f);
    void copyROMFromUSBToLocal(SDL_Renderer* renderer, const std::string& sourcePath);
};

#endif
