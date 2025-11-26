#include "ROMManager.h"
#include "Config.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <SDL_image.h>


namespace fs = std::filesystem;

ROMManager::ROMManager() {
}

ROMManager::~ROMManager() {
    for (ROM* rom : allROMs) {
        delete rom;
    }
    allROMs.clear();
}

void ROMManager::scanROMs(SDL_Renderer* renderer) {
    std::cout << "Escaneando ROMs" << std::endl;
    
    std::cout << "Buscando en USB E:\\" << std::endl;
    bool foundUSB = false;
    if (fs::exists(ROM_PATH_USB)) {
        loadROMsFromDirectory(renderer, ROM_PATH_NES, Console::NES);
        loadROMsFromDirectory(renderer, ROM_PATH_SNES, Console::SNES);
        loadROMsFromDirectory(renderer, ROM_PATH_GBA, Console::GBA);
        foundUSB = true;
    }
    
    std::cout << "Buscando ROMs localmente..." << std::endl;
    scanConsole(renderer, Console::NES);
    scanConsole(renderer, Console::SNES);
    scanConsole(renderer, Console::GBA);
    
    std::cout << "Total de ROMs encontradas: " << allROMs.size() << std::endl;
    loadData();
}

void ROMManager::scanConsole(SDL_Renderer* renderer, Console console) {
    std::string consoleName;
    switch (console) {
        case Console::NES: consoleName = "nes"; break;
        case Console::SNES: consoleName = "snes"; break;
        case Console::GBA: consoleName = "gba"; break;
        default: return;
    }
    
    std::string path = ROM_PATH + consoleName + "/";
    
    if (!fs::exists(path)) {
        std::cout << "No existe directorio: " << path << std::endl;
        return;
    }
    
    std::cout << "Escaneando: " << path << std::endl;
    loadROMsFromDirectory(renderer, path, console);
}

void ROMManager::loadROMsFromDirectory(SDL_Renderer* renderer, 
                                       const std::string& path,
                                       Console console) {
    if (!fs::exists(path)) return;
    
    for (const auto& entry : fs::directory_iterator(path)) {
        if (fs::is_regular_file(entry.status())) {
            std::string filepath = entry.path().string();
            std::string filename = entry.path().filename().string();
            
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            
            if (ext == ".nes" || ext == ".snes" || ext == ".smc" || 
                ext == ".sfc" || ext == ".gba" || ext == ".gb" || ext == ".gbc") {
                
                ROM* rom = new ROM(filename, filepath, console, renderer);
                loadBoxart(renderer, rom);
                
                allROMs.push_back(rom);
                std::cout << "  + " << filename << std::endl;
            }
        }
    }
}

void ROMManager::loadBoxart(SDL_Renderer* renderer, ROM* rom) {
    if (!renderer || !rom) return;
    
    std::string boxartName = rom->getDisplayName();
    std::vector<std::string> possiblePaths = {
        BOXART_PATH + boxartName + ".png",
        BOXART_PATH + boxartName + ".jpg",
        "assets/boxart/" + boxartName + ".png",
        "assets/boxart/" + boxartName + ".jpg"
    };
    
    for (const std::string& boxartPath : possiblePaths) {
        if (fs::exists(boxartPath)) {
            SDL_Surface* surface = IMG_Load(boxartPath.c_str());
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
                rom->setBoxartTexture(texture);
                std::cout << "Carátula: " << boxartPath << std::endl;
                return;
            } else {
                std::cerr << "Error IMG_Load: " << IMG_GetError() << std::endl;
            }
        }
    }
    
    std::cout << "Sin carátula para: " << boxartName << std::endl;
}

std::vector<ROM*> ROMManager::getROMs(Console console) const {
    if (console == Console::ALL) {
        return allROMs;
    }
    
    std::vector<ROM*> filtered;
    for (ROM* rom : allROMs) {
        if (rom->getConsole() == console) {
            filtered.push_back(rom);
        }
    }
    return filtered;
}

std::vector<ROM*> ROMManager::getFavorites() const {
    std::vector<ROM*> favorites;
    for (ROM* rom : allROMs) {
        if (rom->isFavorite()) {
            favorites.push_back(rom);
        }
    }
    return favorites;
}

std::vector<ROM*> ROMManager::getAllROMs() const {
    return allROMs;
}

ROM* ROMManager::getROMByIndex(int index, Console console) {
    std::vector<ROM*> roms = getROMs(console);
    if (index >= 0 && index < (int)roms.size()) {
        return roms[index];
    }
    return nullptr;
}

int ROMManager::getTotalROMs(Console console) const {
    if (console == Console::ALL) {
        return allROMs.size();
    }
    
    int count = 0;
    for (ROM* rom : allROMs) {
        if (rom->getConsole() == console) {
            count++;
        }
    }
    return count;
}

int ROMManager::getFavoritesCount() const {
    int count = 0;
    for (ROM* rom : allROMs) {
        if (rom->isFavorite()) {
            count++;
        }
    }
    return count;
}

void ROMManager::saveData() {
    std::ofstream file("data/roms.dat");
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir archivo de datos para guardar" << std::endl;
        return;
    }
    
    for (ROM* rom : allROMs) {
        file << rom->getFilepath() << "|"
             << rom->isFavorite() << "|"
             << rom->getTimesPlayed() << "\n";
    }
    
    file.close();
    std::cout << "Datos guardados correctamente" << std::endl;
}

void ROMManager::loadData() {
    std::ifstream file("data/roms.dat");
    if (!file.is_open()) {
        std::cout << "No hay datos previos para cargar" << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string filepath, favStr, playsStr;
        
        if (std::getline(ss, filepath, '|') &&
            std::getline(ss, favStr, '|') &&
            std::getline(ss, playsStr)) {
            
            for (ROM* rom : allROMs) {
                if (rom->getFilepath() == filepath) {
                    rom->setFavorite(favStr == "1");
                    int plays = std::stoi(playsStr);
                    for (int i = 0; i < plays; i++) {
                        rom->incrementTimesPlayed();
                    }
                    break;
                }
            }
        }
    }
    
    file.close();
    std::cout << "Datos cargados correctamente" << std::endl;
}

bool ROMManager::addROMFromFile(SDL_Renderer* renderer, const std::string& filepath) {
    if (!fs::exists(filepath)) {
        std::cerr << "El archivo no existe: " << filepath << std::endl;
        return false;
    }
    
    for (ROM* rom : allROMs) {
        if (rom->getFilepath() == filepath) {
            std::cout << "La ROM ya está en la biblioteca" << std::endl;
            return false;
        }
    }
    
    Console console = detectConsoleFromExtension(filepath);
    if (console == Console::ALL) {
        std::cerr << "No se pudo detectar la consola" << std::endl;
        return false;
    }
    
    std::string filename = fs::path(filepath).filename().string();
    ROM* rom = new ROM(filename, filepath, console, renderer);
    
    loadBoxart(renderer, rom);
    
    allROMs.push_back(rom);
    
    std::cout << "ROM agregada: " << filename << std::endl;
    saveData();
    return true;
}

void ROMManager::removeROM(ROM* rom) {
    auto it = std::find(allROMs.begin(), allROMs.end(), rom);
    if (it != allROMs.end()) {
        delete *it;
        allROMs.erase(it);
        saveData();
    }
}

void ROMManager::rescanAll(SDL_Renderer* renderer) {
    for (ROM* rom : allROMs) {
        delete rom;
    }
    allROMs.clear();
    
    scanROMs(renderer);
}

Console ROMManager::detectConsoleFromExtension(const std::string& filename) const {
    std::string ext = fs::path(filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".nes") return Console::NES;
    if (ext == ".snes" || ext == ".smc" || ext == ".sfc") return Console::SNES;
    if (ext == ".gba" || ext == ".gb" || ext == ".gbc") return Console::GBA;
    
    return Console::ALL;
}

bool ROMManager::isROMFile(const std::string& filename) const {
    std::string ext = fs::path(filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return (ext == ".nes" || ext == ".snes" || ext == ".smc" || 
            ext == ".sfc" || ext == ".gba" || ext == ".gb" || ext == ".gbc");
}
