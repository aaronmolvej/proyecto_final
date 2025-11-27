#include "MenuScreen.h"
#include "MD5.h"
#include "USBDetector.h"
#include "Config.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <SDL2/SDL_mixer.h>
#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

bool ends_with(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

std::string fixWindowsPath(const std::string& path) {
    std::string fixed = path;
    std::replace(fixed.begin(), fixed.end(), '/', '\\');
    return fixed;
}

void forceSnes9xFullscreen(const std::string& emulatorPath) {
#ifdef _WIN32
    size_t lastSlash = emulatorPath.find_last_of("\\/");
    std::string emulatorDir = emulatorPath.substr(0, lastSlash);
    std::vector<std::string> configPaths = {
        emulatorDir + "\\snes9x.conf",
        emulatorDir + "\\snes9x.ini",
        emulatorDir + "\\snes9x-x64.conf"
    };
    
    for (const auto& configPath : configPaths) {
        if (fs::exists(configPath)) {
            std::ifstream inFile(configPath);
            std::stringstream buffer;
            buffer << inFile.rdbuf();
            std::string content = buffer.str();
            inFile.close();
            
            bool modified = false;
            if (content.find("FullScreen=") != std::string::npos) {
                size_t pos = 0;
                while ((pos = content.find("FullScreen=", pos)) != std::string::npos) {
                    size_t endPos = content.find('\n', pos);
                    if (endPos == std::string::npos) endPos = content.length();
                    content.replace(pos, endPos - pos, "FullScreen=1");
                    modified = true;
                    pos += 12;
                }
            }
            
            if (content.find("Fullscreen=") != std::string::npos) {
                size_t pos = 0;
                while ((pos = content.find("Fullscreen=", pos)) != std::string::npos) {
                    size_t endPos = content.find('\n', pos);
                    if (endPos == std::string::npos) endPos = content.length();
                    content.replace(pos, endPos - pos, "Fullscreen=1");
                    modified = true;
                    pos += 12;
                }
            }
            
            if (!modified) {
                content += "\n[Display]\n";
                content += "FullScreen=1\n";
                content += "Fullscreen=1\n";
            }
            
            std::ofstream outFile(configPath);
            outFile << content;
            outFile.close();
            return;
        }
    }
    
    std::string newConfigPath = emulatorDir + "\\snes9x.conf";
    std::ofstream newFile(newConfigPath);
    newFile << "[Display]\n";
    newFile << "FullScreen=1\n";
    newFile << "Fullscreen=1\n";
    newFile.close();
#else
    std::string homeDir = getenv("HOME") ? getenv("HOME") : "/home/pi";
    std::vector<std::string> configPaths = {
        homeDir + "/.snes9x/snes9x.conf",
        homeDir + "/.config/snes9x/snes9x.conf"
    };
    
    for (const auto& configPath : configPaths) {
        if (fs::exists(configPath)) {
            std::ifstream inFile(configPath);
            std::stringstream buffer;
            buffer << inFile.rdbuf();
            std::string content = buffer.str();
            inFile.close();
            
            bool modified = false;
            if (content.find("FullScreen=") != std::string::npos) {
                size_t pos = 0;
                while ((pos = content.find("FullScreen=", pos)) != std::string::npos) {
                    size_t endPos = content.find('\n', pos);
                    if (endPos == std::string::npos) endPos = content.length();
                    content.replace(pos, endPos - pos, "FullScreen=1");
                    modified = true;
                    pos += 12;
                }
            }
            
            if (!modified) {
                content += "\n[Display]\nFullScreen=1\n";
            }
            
            std::ofstream outFile(configPath);
            outFile << content;
            outFile.close();
            return;
        }
    }
#endif
}

void launchEmulator(const std::string& emulatorPath, const std::string& romPathRel) {
#ifdef _WIN32
    std::string romPathAbs = fixWindowsPath(fs::absolute(romPathRel).string());
    std::string emulatorFixed = fixWindowsPath(emulatorPath);
#else
    std::string romPathAbs = fs::absolute(romPathRel).string();
    std::string emulatorFixed = emulatorPath;
#endif

    if (!fs::exists(emulatorFixed)) {
        std::cerr << "Error: No se encontró el emulador" << std::endl;
        std::cerr << "Ruta esperada: " << emulatorFixed << std::endl;
        std::cerr << "Ruta absoluta: " << fs::absolute(emulatorFixed) << std::endl;
        return;
    }
    
    if (!fs::exists(romPathAbs)) {
        std::cerr << "No existe la rom: " << romPathAbs << std::endl;
        return;
    }

    if (Mix_PlayingMusic()) {
        Mix_PauseMusic();
        std::cout << "Música del menú pausada..." << std::endl;
    }

    std::cout << "\n== Emulador Ejecutado ==" << std::endl;
    std::cout << "Emulador: " << emulatorFixed << std::endl;
    std::cout << "ROM: " << romPathAbs << std::endl;

#ifdef _WIN32
    std::string commandLine;
    std::string workingDir;

    size_t lastSlash = emulatorFixed.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        workingDir = emulatorFixed.substr(0, lastSlash);
    }

    if (emulatorFixed.find("fceux") != std::string::npos) {
        commandLine = "\"" + emulatorFixed + "\" --fullscreen 1 \"" + romPathAbs + "\"";
        std::cout << "Modo: FCEUX Fullscreen" << std::endl;
    }
    else if (emulatorFixed.find("snes9x") != std::string::npos) {
        forceSnes9xFullscreen(emulatorFixed);
        commandLine = "\"" + emulatorFixed + "\" \"" + romPathAbs + "\"";
        std::cout << "Modo: Snes9x Fullscreen" << std::endl;
    }
    else if (emulatorFixed.find("mGBA") != std::string::npos ||
             emulatorFixed.find("mgba") != std::string::npos) {
        commandLine = "\"" + emulatorFixed + "\" -f \"" + romPathAbs + "\"";
        std::cout << "Modo: mGBA Fullscreen" << std::endl;
    }
    else {
        commandLine = "\"" + emulatorFixed + "\" \"" + romPathAbs + "\"";
    }

    std::cout << "Comando: " << commandLine << std::endl;
    std::cout << "Working Dir: " << workingDir << std::endl;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWMAXIMIZED;
    ZeroMemory(&pi, sizeof(pi));

    std::vector<char> cmdLine(commandLine.begin(), commandLine.end());
    cmdLine.push_back('\0');

    if (CreateProcessA(
            NULL,
            cmdLine.data(),
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            workingDir.empty() ? NULL : workingDir.c_str(),
            &si,
            &pi)
    ) {
        std::cout << " Emulador lanzado (PID: " << pi.dwProcessId << ")" << std::endl;
        std::cout << " Esperando cierre del emulador..." << std::endl;
        std::cout << " (Presiona ESC en el emulador para salir)" << std::endl;

        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        std::cout << "Emulador cerrado, regresando a la interfaz" << std::endl;
    } else {
        DWORD error = GetLastError();
        std::cerr << "Error al lanzar emulador. Código: " << error << std::endl;

        LPVOID lpMsgBuf;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error,
            0,
            (LPSTR)&lpMsgBuf,
            0,
            NULL
        );
        std::cerr << "Descripción: " << (char*)lpMsgBuf << std::endl;
        LocalFree(lpMsgBuf);
    }

#else
    std::string command;

    if (emulatorFixed.find("mgba") != std::string::npos) {
        command = emulatorFixed + " -f \"" + romPathAbs + "\"";
    }
    else {
        command = emulatorFixed + " \"" + romPathAbs + "\"";
    }

    std::cout << "[System] Preparando lanzamiento: " << command << std::endl;

    std::ofstream cmdFile("/tmp/run_game.sh");
    if (cmdFile.is_open()) {
        cmdFile << "#!/bin/bash\n";
        cmdFile << command << "\n"; 
        cmdFile.close();
        
        int chmodRes = std::system("chmod +x /tmp/run_game.sh");
        (void)chmodRes; 
    }

    std::cout << "Cerrando interfaz..." << std::endl;
    exit(0); 
#endif

    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
        std::cout << "Música del menú reanudada" << std::endl;
    }
}

MenuScreen::MenuScreen(ROMManager* romManager)
    : romManager(romManager), currentTab(Tab::BIBLIOTECA),
      selectedConsole(Console::ALL), selectedROMIndex(0),
      selectedFileIndex(0),
      usbCheckInProgress(false), usbCheckTimer(0.0f),
      statusMessage(""), statusMessageTimer(0.0f),
      btnFavorite(nullptr), btnPlay(nullptr), btnContinue(nullptr),
      btnAddROM(nullptr), btnDeleteROM(nullptr), btnRescan(nullptr) {
    
    int tabWidth = 200;
    int tabHeight = 55;
    int tabY = 20;
    int startX = (SCREEN_WIDTH - (tabWidth * 3 + 40)) / 2;
    
    tabButtons.push_back(new Button(startX, tabY, tabWidth, tabHeight, "Biblioteca"));
    tabButtons.push_back(new Button(startX + tabWidth + 20, tabY, tabWidth, tabHeight, "Favoritos"));
    tabButtons.push_back(new Button(startX + (tabWidth + 20) * 2, tabY, tabWidth, tabHeight, "ROMS"));
    
    tabButtons[0]->setOnClick([this]() { currentTab = Tab::BIBLIOTECA; updateDisplayedROMs(); for (size_t i = 0; i < tabButtons.size(); i++) tabButtons[i]->setSelected(i == 0); });
    tabButtons[1]->setOnClick([this]() { currentTab = Tab::FAVORITOS; updateDisplayedROMs(); for (size_t i = 0; i < tabButtons.size(); i++) tabButtons[i]->setSelected(i == 1); });
    tabButtons[2]->setOnClick([this]() { currentTab = Tab::ROMS; updateDisplayedROMs(); for (size_t i = 0; i < tabButtons.size(); i++) tabButtons[i]->setSelected(i == 2); });
    
    int consoleY = 90;
    int consoleWidth = 100;
    int consoleStartX = 100;
    
    consoleButtons.push_back(new Button(consoleStartX, consoleY, consoleWidth, 40, "Todas"));
    consoleButtons.push_back(new Button(consoleStartX + 110, consoleY, consoleWidth, 40, "NES"));
    consoleButtons.push_back(new Button(consoleStartX + 220, consoleY, consoleWidth, 40, "SNES"));
    consoleButtons.push_back(new Button(consoleStartX + 330, consoleY, consoleWidth, 40, "GBA"));
    
    for (int i = 0; i < 4; i++) {
        consoleButtons[i]->setOnClick([this, i]() {
            if (i == 0) selectedConsole = Console::ALL;
            else if (i == 1) selectedConsole = Console::NES;
            else if (i == 2) selectedConsole = Console::SNES;
            else selectedConsole = Console::GBA;
            updateDisplayedROMs();
            for (size_t j = 0; j < consoleButtons.size(); j++)
                consoleButtons[j]->setSelected(j == static_cast<size_t>(i));
        });
    }
    
    int actionX = 30;
    int actionY = 630;
    int actionW = 140;
    int actionH = 38;
    
    btnFavorite = new Button(actionX, actionY, actionW, actionH, "Favorito");
    btnPlay = new Button(actionX + actionW + 10, actionY, actionW, actionH, "Jugar");
    btnContinue = new Button(actionX + (actionW + 10) * 2, actionY, actionW, actionH, "Continuar");
    
    btnFavorite->setOnClick([this]() {
        ROM* rom = getSelectedROM();
        if (rom) {
            rom->setFavorite(!rom->isFavorite());
            updateButtonStates();
            this->romManager->saveData();
        }
    });
    
    btnPlay->setOnClick([this]() {
        ROM* rom = getSelectedROM();
        if (rom) {
            rom->incrementTimesPlayed();
            updateButtonStates();
            std::string emulatorPath;
            switch (rom->getConsole()) {
                case Console::NES:
                    emulatorPath = EMULATOR_FCEUX;
                    break;
                case Console::SNES:
                    emulatorPath = EMULATOR_SNES9X;
                    break;
                case Console::GBA:
                    emulatorPath = EMULATOR_MGBA;
                    break;
                default:
                    return;
            }
            launchEmulator(emulatorPath, rom->getFilepath());
        }
    });
    
    btnContinue->setOnClick([this]() {
        if (btnPlay->isEnabled()) {
            btnPlay->click();
        }
    });
    
    int actionX2 = SCREEN_WIDTH / 2 - 110;
    int actionY2 = 210; 
    int buttonSpacing = 120; 
    btnAddROM = new Button(actionX2, actionY2,                220, 65, "Agregar ROM");
    btnDeleteROM = new Button(actionX2, actionY2 + buttonSpacing,     220, 65, "Eliminar ROM");
    btnRescan = new Button(actionX2, actionY2 + buttonSpacing * 2, 220, 65, "Re-escaneo");
        
    btnAddROM->setOnClick([this]() {
        if (!availableForAdd.empty() && selectedFileIndex >= 0 && selectedFileIndex < (int)availableForAdd.size()) {
            addROMFromPath(availableForAdd[selectedFileIndex]);
        }
    });
    
    btnDeleteROM->setOnClick([this]() { deleteSelectedROM(); });
    btnRescan->setOnClick([this]() { rescanROMs(); });
}

MenuScreen::~MenuScreen() {
    for (auto btn : tabButtons) delete btn;
    for (auto btn : consoleButtons) delete btn;
    delete btnFavorite; delete btnPlay; delete btnContinue;
    delete btnAddROM; delete btnDeleteROM; delete btnRescan;
}

void MenuScreen::onEnter() {
    updateDisplayedROMs();
    tabButtons[0]->setSelected(true);
    consoleButtons[0]->setSelected(true);
    updateButtonStates();
}

void MenuScreen::updateDisplayedROMs() {
    if (currentTab == Tab::ROMS) {
        displayedROMs = romManager->getROMs(Console::ALL);
        availableForAdd.clear();
        
        std::vector<ROM*> existingROMs = romManager->getAllROMs();
        
        std::vector<std::string> localPaths = {
            "roms/nes/",
            "roms/snes/",
            "roms/gba/"
        };
        
        for (const auto& localPath : localPaths) {
            if (fs::exists(localPath)) {
                try {
                    for (auto& entry : fs::directory_iterator(localPath)) {
                        if (fs::is_regular_file(entry.status())) {
                            std::string filepath = entry.path().string();
                            std::string filename = entry.path().filename().string();
                            
                            if (ends_with(filename, ".nes") || ends_with(filename, ".sfc") ||
                                ends_with(filename, ".smc") || ends_with(filename, ".gba")) {
                                
                                std::string fileMD5 = MD5::hashFile(filepath);
                                bool alreadyAdded = false;
                                
                                for (ROM* rom : existingROMs) {
                                    if (rom->getMD5Hash() == fileMD5) {
                                        alreadyAdded = true;
                                        break;
                                    }
                                }
                                
                                if (!alreadyAdded && !fileMD5.empty()) {
                                    availableForAdd.push_back(filepath);
                                }
                            }
                        }
                    }
                } catch (const fs::filesystem_error& e) {
                    std::cerr << "Error escaneando " << localPath << ": " << e.what() << std::endl;
                }
            }
        }
        
        if (usbDetector.detectUSBDrives()) {
            std::vector<std::string> usbPaths = usbDetector.getUSBPaths();
            
            for (const std::string& usbPath : usbPaths) {
                std::vector<std::string> usbDirs;
                if (fs::exists(usbPath + "nes")) usbDirs.push_back(usbPath + "nes");
                if (fs::exists(usbPath + "snes")) usbDirs.push_back(usbPath + "snes");
                if (fs::exists(usbPath + "gba")) usbDirs.push_back(usbPath + "gba");
                
                usbDirs.push_back(usbPath);
                
                for (const std::string& usbDir : usbDirs) {
                    if (!fs::exists(usbDir) || !fs::is_directory(usbDir)) continue;
                    
                    try {
                        for (auto& entry : fs::directory_iterator(usbDir)) {
                            if (fs::is_regular_file(entry.status())) {
                                std::string filepath = entry.path().string();
                                std::string filename = entry.path().filename().string();
                                std::string extension = entry.path().extension().string();
                                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                                
                                if (extension == ".nes" || extension == ".sfc" || 
                                    extension == ".smc" || extension == ".gba" ||
                                    extension == ".gb" || extension == ".gbc") {
                                    
                                    std::string fileMD5 = MD5::hashFile(filepath);
                                    bool alreadyAdded = false;
                                    
                                    for (ROM* rom : existingROMs) {
                                        if (rom->getMD5Hash() == fileMD5) {
                                            alreadyAdded = true;
                                            break;
                                        }
                                    }
                                    
                                    if (!alreadyAdded && !fileMD5.empty()) {
                                        availableForAdd.push_back(filepath);
                                        std::cout << "Disponible para agregar: " << filename << std::endl;
                                    }
                                }
                            }
                        }
                    } catch (const fs::filesystem_error& e) {
                        std::cerr << "Error escaneando " << usbDir << ": " << e.what() << std::endl;
                    }
                }
            }
        }
        
        std::cout << "Archivos disponibles para agregar: " << availableForAdd.size() << std::endl;
        selectedFileIndex = 0;
    }
    else if (currentTab == Tab::BIBLIOTECA) {
        displayedROMs = romManager->getROMs(selectedConsole);
    }
    else if (currentTab == Tab::FAVORITOS) {
        displayedROMs = romManager->getFavorites();
    }
    
    if (selectedROMIndex >= (int)displayedROMs.size())
        selectedROMIndex = displayedROMs.empty() ? 0 : (int)displayedROMs.size() - 1;
    updateButtonStates();
}

void MenuScreen::updateButtonStates() {
    if (currentTab == Tab::ROMS) return;
    ROM* rom = getSelectedROM();
    if (!rom) {
        btnFavorite->setEnabled(false);
        btnFavorite->setText("Favorito");
        btnPlay->setEnabled(false);
        btnContinue->setEnabled(false);
        return;
    }
    
    btnFavorite->setEnabled(true);
    btnFavorite->setText(rom->isFavorite() ? "* Favorito" : "Favorito");
    btnPlay->setEnabled(true);
    if (rom->getTimesPlayed() > 0)
        btnContinue->setEnabled(true);
    else
        btnContinue->setEnabled(false);
    btnContinue->setText(rom->getTimesPlayed() > 0 ? "Continuar" : "Sin partidas");
}

void MenuScreen::handleInput(InputManager& input) {
    if (input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_LEFTSTICK) && 
        input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_RIGHTSTICK)) {
        
        std::cout << "Combo L3+R3 detectado: Apagando..." << std::endl;
        int res = std::system("sudo poweroff");
        (void)res;
        exit(0);
    }
    if (input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_LEFTSHOULDER)) {
         if (currentTab == Tab::FAVORITOS) tabButtons[0]->click();
         else if (currentTab == Tab::ROMS) tabButtons[1]->click();
    }
    if (input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) {
         if (currentTab == Tab::BIBLIOTECA) tabButtons[1]->click();
         else if (currentTab == Tab::FAVORITOS) tabButtons[2]->click();
    }

    if (currentTab != Tab::ROMS) {
        if (input.isRightPressed() || input.isDownPressed()) { 
            selectedROMIndex = (selectedROMIndex + 1) % (int)displayedROMs.size(); 
            updateButtonStates();
        }
        if (input.isLeftPressed() || input.isUpPressed()) { 
            selectedROMIndex--; 
            if (selectedROMIndex < 0) selectedROMIndex = (int)displayedROMs.size() - 1; 
            updateButtonStates(); 
        }

        if (input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_BACK)) {
            int nextFilter = (static_cast<int>(selectedConsole) + 1) % 4;
            consoleButtons[nextFilter]->click();
        }

        if (input.isKeyPressed(SDLK_f) || input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_Y)) 
            btnFavorite->click();

        if (input.isConfirmPressed() || input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_START)) 
            btnPlay->click();

        if (input.isKeyPressed(SDLK_c) || input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_X)) 
            btnContinue->click();
    }

    if (currentTab == Tab::ROMS) {
        if (input.isUpPressed() && !availableForAdd.empty())
            selectedFileIndex = (selectedFileIndex - 1 + availableForAdd.size()) % availableForAdd.size();

        if (input.isDownPressed() && !availableForAdd.empty())
            selectedFileIndex = (selectedFileIndex + 1) % availableForAdd.size();

        if ((input.isConfirmPressed() || input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_A)) && !availableForAdd.empty())
            addROMFromPath(availableForAdd[selectedFileIndex]);

        if (input.isKeyPressed(SDLK_r) || input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_BACK)) {
            if (!usbCheckInProgress) {
                checkAndCopyUSBROMs(nullptr);
            }
        }
        
        if (input.isKeyPressed(SDLK_DELETE) || input.isGamepadButtonPressed(SDL_CONTROLLER_BUTTON_Y))
            btnDeleteROM->click();
    }

    int mx = input.getMouseX();
    int my = input.getMouseY();
    
    for(auto btn : tabButtons) { 
        btn->update(mx,my); 
        if (input.isMouseButtonPressed(SDL_BUTTON_LEFT) && btn->isClicked(mx, my)) btn->click(); 
    }
    for(auto btn : consoleButtons) { 
        btn->update(mx,my); 
        if (input.isMouseButtonPressed(SDL_BUTTON_LEFT) && btn->isClicked(mx, my)) btn->click(); 
    }
    
    if (currentTab != Tab::ROMS) {
        btnFavorite->update(mx,my); 
        btnPlay->update(mx,my); 
        btnContinue->update(mx,my);
        
        if (input.isMouseButtonPressed(SDL_BUTTON_LEFT)){
            if (btnFavorite->isClicked(mx,my)) btnFavorite->click();
            if (btnPlay->isClicked(mx,my)) btnPlay->click();
            if (btnContinue->isClicked(mx,my)) btnContinue->click();
        }
    }

    if (currentTab == Tab::ROMS) {
        btnAddROM->update(mx,my);
        btnDeleteROM->update(mx,my);
        btnRescan->update(mx,my);
        
        if (input.isMouseButtonPressed(SDL_BUTTON_LEFT)){
            if (btnAddROM->isClicked(mx,my)) btnAddROM->click();
            if (btnDeleteROM->isClicked(mx,my)) btnDeleteROM->click();
            if (btnRescan->isClicked(mx,my)) btnRescan->click();
        }
    }
}

void MenuScreen::update(float deltaTime) {
    usbCheckTimer += deltaTime;
    if (usbCheckTimer >= USB_CHECK_INTERVAL) {
        usbCheckTimer = 0.0f;
        if (currentTab == Tab::ROMS && !usbCheckInProgress && isUSBConnected()) {
            checkAndCopyUSBROMs(nullptr);
        }
    }
    
    if (statusMessageTimer > 0) {
        statusMessageTimer -= deltaTime;
        if (statusMessageTimer < 0) {
            statusMessageTimer = 0;
            statusMessage = "";
        }
    }
}

void MenuScreen::render(SDL_Renderer* renderer, TextRenderer* textRenderer) {
    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    SDL_RenderClear(renderer);
    
    renderTopBar(renderer, textRenderer);
    
    if (currentTab == Tab::ROMS) {
        btnAddROM->render(renderer, textRenderer);
        btnDeleteROM->render(renderer, textRenderer);
        btnRescan->render(renderer, textRenderer);
        
        if (!availableForAdd.empty()) {
            int x = 100, y = 500;
            textRenderer->renderText(renderer, "Archivos para agregar:", x, y, TEXT_SECONDARY_COLOR);
            for (size_t i = 0; i < availableForAdd.size() && i < 3; i++) {
                SDL_Color color = (i == (size_t)selectedFileIndex) ? SELECTED_COLOR : TEXT_COLOR;
                textRenderer->renderText(renderer, availableForAdd[i], x + 20, y + 30 + 30 * i, color);
            }
            int offsetX = 180;
            int ctrlY = SCREEN_HEIGHT - 35;
            textRenderer->renderText(renderer, "[Arriba/Abajo]", 50, ctrlY, TEXT_SECONDARY_COLOR);
            textRenderer->renderText(renderer, "Seleccionar", 200, ctrlY, TEXT_COLOR);
            textRenderer->renderText(renderer, "|", 350, ctrlY, TEXT_SECONDARY_COLOR);
            textRenderer->renderText(renderer, "[Enter]", 380, ctrlY, TEXT_SECONDARY_COLOR);
            textRenderer->renderText(renderer, "Agregar ROM", 470, ctrlY, TEXT_COLOR);
        }
        else {
        int msgX = SCREEN_WIDTH / 2;
        int msgY = 520;
        textRenderer->renderText(renderer, "No hay ROMs detectados en USB", msgX, msgY, TEXT_SECONDARY_COLOR, true);
        textRenderer->renderText(renderer, "Presiona [R] para re-escanear", msgX, msgY + 40, TEXT_COLOR, true);

        }
    }
    else {
        renderROMDetailNew(renderer, textRenderer);
        renderROMListVertical(renderer, textRenderer);
    }
    
    renderStatusMessage(renderer, textRenderer);
}

void MenuScreen::renderTopBar(SDL_Renderer* renderer, TextRenderer* textRenderer) {
    for (auto btn : tabButtons) btn->render(renderer, textRenderer);
    for (auto btn : consoleButtons) btn->render(renderer, textRenderer);
    SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
    SDL_RenderDrawLine(renderer, 0, 145, SCREEN_WIDTH, 145);
}

void MenuScreen::renderROMDetailNew(SDL_Renderer* renderer, TextRenderer* textRenderer) {
    ROM* rom = getSelectedROM();
    int boxartSize = 450;
    int boxartX = 30;
    int boxartY = 165;
    
    if (currentTab != Tab::ROMS && rom && rom->getBoxartTexture()) {
        SDL_Rect boxartRect = {boxartX, boxartY, boxartSize, boxartSize};
        SDL_RenderCopy(renderer, rom->getBoxartTexture(), nullptr, &boxartRect);
    } else {
        SDL_Rect placeholderRect = {boxartX, boxartY, boxartSize, boxartSize};
        SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
        SDL_RenderFillRect(renderer, &placeholderRect);
        SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
        SDL_RenderDrawRect(renderer, &placeholderRect);
        
        if (currentTab == Tab::ROMS) {
            textRenderer->renderText(renderer, "Agregar ROM", boxartX + boxartSize / 2, boxartY + boxartSize / 2, TEXT_SECONDARY_COLOR, true);
        } else {
            textRenderer->renderText(renderer, "Sin caratula", boxartX + boxartSize / 2, boxartY + boxartSize / 2, TEXT_SECONDARY_COLOR, true);
        }
    }
    
    if (rom && currentTab != Tab::ROMS) {
        int infoX = boxartX + 5;
        int infoY = boxartY + boxartSize + 15;
        std::string name = rom->getDisplayName();
        if (name.length() > 30) {
            name = name.substr(0, 28) + "...";
        }
        
        textRenderer->renderText(renderer, name, infoX, infoY, TEXT_COLOR);
        textRenderer->renderText(renderer, "Consola: " + rom->getConsoleString(), infoX, infoY + 28, TEXT_SECONDARY_COLOR);
        textRenderer->renderText(renderer, "Jugado: " + std::to_string(rom->getTimesPlayed()), infoX, infoY + 50, TEXT_SECONDARY_COLOR);
    }
    
    if (currentTab != Tab::ROMS) {
        int btnX = boxartX;
        int btnY = 680;
        int btnW = 150;
        int btnH = 40;
        
        btnFavorite->setRect(btnX, btnY, btnW, btnH);
        btnPlay->setRect(btnX + btnW + 5, btnY, btnW, btnH);
        btnContinue->setRect(btnX + (btnW + 5) * 2, btnY, btnW, btnH);
        
        btnFavorite->render(renderer, textRenderer);
        btnPlay->render(renderer, textRenderer);
        btnContinue->render(renderer, textRenderer);
    }
}

void MenuScreen::renderROMListVertical(SDL_Renderer* renderer, TextRenderer* textRenderer) {
    if (displayedROMs.empty()) {
        textRenderer->renderText(renderer, "No hay juegos disponibles", 600, 300, TEXT_SECONDARY_COLOR);
        return;
    }
    
    int listX = 520;
    int listY = 165;
    int itemHeight = 55;
    int itemWidth = 820;
    int maxVisible = 11;
    
    textRenderer->renderText(renderer, "ROMs (" + std::to_string(displayedROMs.size()) + ")", listX, listY - 30, TEXT_COLOR);
    
    int scrollOffset = 0;
    if (selectedROMIndex >= maxVisible / 2) {
        scrollOffset = selectedROMIndex - maxVisible / 2;
        if (scrollOffset + maxVisible > (int)displayedROMs.size()) {
            scrollOffset = (int)displayedROMs.size() - maxVisible;
        }
    }
    if (scrollOffset < 0) scrollOffset = 0;
    
    for (int i = 0; i < maxVisible && (i + scrollOffset) < (int)displayedROMs.size(); i++) {
        int actualIndex = i + scrollOffset;
        ROM* rom = displayedROMs[actualIndex];
        int itemY = listY + i * itemHeight;
        
        SDL_Rect itemRect = {listX, itemY, itemWidth, itemHeight - 5};
        if (actualIndex == selectedROMIndex) {
            SDL_SetRenderDrawColor(renderer, SELECTED_COLOR.r, SELECTED_COLOR.g, SELECTED_COLOR.b, SELECTED_COLOR.a);
        } else {
            SDL_SetRenderDrawColor(renderer, BUTTON_COLOR.r, BUTTON_COLOR.g, BUTTON_COLOR.b, BUTTON_COLOR.a);
        }
        
        SDL_RenderFillRect(renderer, &itemRect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
        SDL_RenderDrawRect(renderer, &itemRect);
        
        std::string name = rom->getDisplayName();
        if (rom->isFavorite()) name = "*" + name;
        textRenderer->renderText(renderer, name, listX + 10, itemY + 18, TEXT_COLOR);
        textRenderer->renderText(renderer, rom->getConsoleString(), listX + itemWidth - 100, itemY + 18, TEXT_SECONDARY_COLOR);
    }
    int offsetX = 150;
    int controlY = SCREEN_HEIGHT - -150;

    textRenderer->renderText(renderer, "[Flechas]", 40  + offsetX, controlY, TEXT_SECONDARY_COLOR);
    textRenderer->renderText(renderer, "Navegar", 120 + offsetX, controlY, TEXT_COLOR);
    textRenderer->renderText(renderer, "|", 240   + offsetX, controlY, TEXT_SECONDARY_COLOR);
    textRenderer->renderText(renderer, "[Enter]", 280 + offsetX, controlY, TEXT_SECONDARY_COLOR);
    textRenderer->renderText(renderer, "Jugar", 370 + offsetX, controlY, TEXT_COLOR);
    textRenderer->renderText(renderer, "|", 450   + offsetX, controlY, TEXT_SECONDARY_COLOR);
    textRenderer->renderText(renderer, "[F]", 470 + offsetX, controlY, TEXT_SECONDARY_COLOR);
    textRenderer->renderText(renderer, "Favorito", 520 + offsetX, controlY, TEXT_COLOR);
    textRenderer->renderText(renderer, "|", 620   + offsetX, controlY, TEXT_SECONDARY_COLOR);
    textRenderer->renderText(renderer, "[C]", 640 + offsetX, controlY, TEXT_SECONDARY_COLOR);
    textRenderer->renderText(renderer, "Continuar", 680 + offsetX, controlY, TEXT_COLOR);
    textRenderer->renderText(renderer, "|", 800   + offsetX, controlY, TEXT_SECONDARY_COLOR);
    textRenderer->renderText(renderer, "[R]", 820 + offsetX, controlY, TEXT_SECONDARY_COLOR);
    textRenderer->renderText(renderer, "USB", 860 + offsetX, controlY, TEXT_COLOR);
}

ROM* MenuScreen::getSelectedROM() {
    if (selectedROMIndex >= 0 && selectedROMIndex < (int)displayedROMs.size()) {
        return displayedROMs[selectedROMIndex];
    }
    return nullptr;
}

void MenuScreen::addROMFromPath(const std::string& path) {
    if (romManager->addROMFromFile(nullptr, path)) {
        updateDisplayedROMs();
    }
}

void MenuScreen::deleteSelectedROM() {
    ROM* rom = getSelectedROM();
    if (rom) {
        romManager->removeROM(rom);
        updateDisplayedROMs();
    }
}

void MenuScreen::rescanROMs() {
    checkAndCopyUSBROMs(nullptr);
    updateDisplayedROMs();
}

bool MenuScreen::isUSBConnected() {
    return usbDetector.detectUSBDrives();
}

void MenuScreen::setStatusMessage(const std::string& msg, float duration) {
    statusMessage = msg;
    statusMessageTimer = duration;
}

void MenuScreen::renderStatusMessage(SDL_Renderer* renderer, TextRenderer* textRenderer) {
    if (statusMessageTimer > 0 && !statusMessage.empty()) {
        int msgX = SCREEN_WIDTH / 2;
        int msgY = SCREEN_HEIGHT - 50;
        
        SDL_Rect bgRect = {msgX - 200, msgY - 20, 400, 40};
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_RenderFillRect(renderer, &bgRect);
        
        textRenderer->renderText(renderer, statusMessage, msgX, msgY, SELECTED_COLOR, true);
    }
}

void MenuScreen::copyROMFromUSBToLocal(SDL_Renderer* renderer, const std::string& sourcePath) {
    try {
        std::string sourceHash = MD5::hashFile(sourcePath);
        if (sourceHash.empty()) return;

        for (ROM* existingROM : romManager->getAllROMs()) {
            if (existingROM->getMD5Hash() == sourceHash) {
                setStatusMessage("ROM duplicada", 2.0f);
                return;
            }
        }

        Console targetConsole = romManager->detectConsoleFromExtension(sourcePath);
        if (targetConsole == Console::ALL) return;

        std::string destinationDir;
        switch (targetConsole) {
            case Console::NES: destinationDir = LOCAL_ROM_PATH_NES; break;
            case Console::SNES: destinationDir = LOCAL_ROM_PATH_SNES; break;
            case Console::GBA: destinationDir = LOCAL_ROM_PATH_GBA; break;
            default: return;
        }

        if (!fs::exists(destinationDir)) {
            fs::create_directories(destinationDir);
        }

        std::string filename = fs::path(sourcePath).filename().string();
        std::string destinationPath = destinationDir + filename;

        if (fs::exists(destinationPath)) {
            std::string destHash = MD5::hashFile(destinationPath);
            if (destHash == sourceHash) {
                setStatusMessage("ROM ya existe", 2.0f);
                return;
            } else {
                std::string baseName = fs::path(filename).stem().string();
                std::string extension = fs::path(filename).extension().string();
                int suffix = 1;
                do {
                    destinationPath = destinationDir + baseName + "_" + std::to_string(suffix) + extension;
                    suffix++;
                } while (fs::exists(destinationPath));
            }
        }

        fs::copy_file(sourcePath, destinationPath, fs::copy_options::overwrite_existing);

        if (romManager->addROMFromFile(renderer, destinationPath)) {
            setStatusMessage("ROM copiada: " + filename, 3.0f);
        }
    } catch (const fs::filesystem_error& e) {
        setStatusMessage("Error al copiar ROM", 2.0f);
    }
}

void MenuScreen::checkAndCopyUSBROMs(SDL_Renderer* renderer) {
    if (usbCheckInProgress) return;

    usbCheckInProgress = true;
    setStatusMessage("Buscando ROMs en USB", 2.0f);

    if (!usbDetector.detectUSBDrives()) {
        usbCheckInProgress = false;
        return;
    }

    std::vector<std::string> usbPaths = usbDetector.getUSBPaths();
    int copiedCount = 0;

    for (const std::string& usbPath : usbPaths) {
        std::vector<std::string> romDirs;
        
        if (fs::exists(usbPath + "nes")) romDirs.push_back(usbPath + "nes");
        if (fs::exists(usbPath + "snes")) romDirs.push_back(usbPath + "snes");
        if (fs::exists(usbPath + "gba")) romDirs.push_back(usbPath + "gba");
        
        romDirs.push_back(usbPath);

        for (const std::string& romDir : romDirs) {
            if (!fs::exists(romDir) || !fs::is_directory(romDir)) continue;

            std::cout << "Escaneando: " << romDir << std::endl;

            try {
                for (const auto& entry : fs::directory_iterator(romDir)) {
                    if (!fs::is_regular_file(entry.status())) continue;
                    
                    std::string filepath = entry.path().string();
                    std::string extension = entry.path().extension().string();
                    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                    if (extension == ".nes" || extension == ".snes" || 
                        extension == ".smc" || extension == ".sfc" ||
                        extension == ".gba" || extension == ".gb" || extension == ".gbc") {
                        
                        std::cout << "  Encontrado: " << entry.path().filename().string() << std::endl;
                        copyROMFromUSBToLocal(renderer, filepath);
                        copiedCount++;
                    }
                }
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error al escanear " << romDir << ": " << e.what() << std::endl;
            }
        }
    }

    if (copiedCount > 0) {
        setStatusMessage("Procesadas " + std::to_string(copiedCount) + " ROMs", 3.0f);
        updateDisplayedROMs();
    } else {
        setStatusMessage("No se encontraron ROMs nuevas", 2.0f);
    }

    usbCheckInProgress = false;
}
