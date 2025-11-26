#ifndef ROMMANAGER_H
#define ROMMANAGER_H

#include "ROM.h"
#include <vector>
#include <string>
#include <SDL.h>

class ROMManager {
private:
    std::vector<ROM*> allROMs;
    
    void scanConsole(SDL_Renderer* renderer, Console console);
    void loadROMsFromDirectory(SDL_Renderer* renderer, 
                               const std::string& path,
                               Console console);
    void loadBoxart(SDL_Renderer* renderer, ROM* rom);
    
public:
    ROMManager();
    ~ROMManager();
    
    void scanROMs(SDL_Renderer* renderer);
    
    std::vector<ROM*> getROMs(Console console) const;
    std::vector<ROM*> getFavorites() const;
    std::vector<ROM*> getAllROMs() const;
    
    ROM* getROMByIndex(int index, Console console);
    
    int getTotalROMs(Console console) const;
    int getFavoritesCount() const;
    
    void saveData();
    void loadData();
    
    bool addROMFromFile(SDL_Renderer* renderer, const std::string& filepath);
    void removeROM(ROM* rom);
    void rescanAll(SDL_Renderer* renderer);
    
    Console detectConsoleFromExtension(const std::string& filename) const;
    bool isROMFile(const std::string& filename) const;
};

#endif
