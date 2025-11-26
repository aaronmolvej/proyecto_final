#include "ROM.h"
#include "MD5.h"
#include "Config.h"
#include <SDL_image.h>
#include <iostream>

ROM::ROM(const std::string& filename, const std::string& filepath, Console console, SDL_Renderer* renderer)
    : filename(filename), filepath(filepath), console(console),
      boxartTexture(nullptr), favorite(false), timesPlayed(0), md5Hash("") {
    
    extractDisplayName();
    
    md5Hash = MD5::hashFile(filepath);
    if (!md5Hash.empty()) {
        std::cout << "MD5: " << md5Hash.substr(0, 8) << " para " << displayName << std::endl;
    }
    
    if (renderer) {
        loadBoxart(renderer);
    }
}

ROM::~ROM() {
    if (boxartTexture) {
        SDL_DestroyTexture(boxartTexture);
        boxartTexture = nullptr;
    }
}

void ROM::extractDisplayName() {
    displayName = filename;
    
    size_t dotPos = displayName.find_last_of('.');
    if (dotPos != std::string::npos) {
        displayName = displayName.substr(0, dotPos);
    }
    
    size_t parenPos = displayName.find('(');
    if (parenPos != std::string::npos) {
        displayName = displayName.substr(0, parenPos);
    }
    
    while (!displayName.empty() && displayName.back() == ' ') {
        displayName.pop_back();
    }
}

void ROM::loadBoxart(SDL_Renderer* renderer) {
    if (boxartTexture) return;
    
    std::string baseName = filename.substr(0, filename.find_last_of('.'));
    std::string consoleStr = getConsoleString();
    SDL_Surface* surface = nullptr;
    std::string boxartPath;
    
    boxartPath = "media/boxart/" + consoleStr + "/" + baseName + ".jpg";
    surface = IMG_Load(boxartPath.c_str());
    
    if (!surface) {
        boxartPath = "media/boxart/" + consoleStr + "/" + baseName + ".png";
        surface = IMG_Load(boxartPath.c_str());
    }
    
    if (!surface) {
        boxartPath = BOXART_PATH + consoleStr + "/" + baseName + ".jpg";
        surface = IMG_Load(boxartPath.c_str());
    }
    
    if (!surface) {
        boxartPath = BOXART_PATH + consoleStr + "/" + baseName + ".png";
        surface = IMG_Load(boxartPath.c_str());
    }
    
    if (!surface) {
        boxartPath = IMAGE_PATH + "placeholder.png";
        surface = IMG_Load(boxartPath.c_str());
    }
    
    if (surface) {
        boxartTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

std::string ROM::getDisplayName() const { return displayName; }
std::string ROM::getFilename() const { return filename; }
std::string ROM::getFilepath() const { return filepath; }
std::string ROM::getMD5Hash() const { return md5Hash; }
Console ROM::getConsole() const { return console; }

std::string ROM::getConsoleString() const {
    switch (console) {
        case Console::NES: return "NES";
        case Console::SNES: return "SNES";
        case Console::GBA: return "GBA";
        default: return "Unknown";
    }
}

bool ROM::isFavorite() const { return favorite; }
void ROM::setFavorite(bool fav) { favorite = fav; }

int ROM::getTimesPlayed() const { return timesPlayed; }
void ROM::incrementTimesPlayed() { timesPlayed++; }

SDL_Texture* ROM::getBoxartTexture() const { return boxartTexture; }

void ROM::setBoxartTexture(SDL_Texture* texture) {
    if (boxartTexture) {
        SDL_DestroyTexture(boxartTexture);
    }
    boxartTexture = texture;
}
