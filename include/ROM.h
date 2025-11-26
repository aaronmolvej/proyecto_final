#ifndef ROM_H
#define ROM_H

#include <string>
#include <SDL.h>
#include "Config.h"

class ROM {
private:
    std::string displayName;
    std::string filename;
    std::string filepath;
    std::string md5Hash;
    Console console;
    bool favorite;
    int timesPlayed;
    SDL_Texture* boxartTexture;
    
    void extractDisplayName();
    void loadBoxart(SDL_Renderer* renderer);
    
public:
    ROM(const std::string& filename, const std::string& filepath, 
        Console console, SDL_Renderer* renderer);
    ~ROM();
    
    std::string getDisplayName() const;
    std::string getFilename() const;
    std::string getFilepath() const;
    std::string getMD5Hash() const;
    Console getConsole() const;
    std::string getConsoleString() const;
    
    bool isFavorite() const;
    void setFavorite(bool fav);
    
    int getTimesPlayed() const;
    void incrementTimesPlayed();
    
    SDL_Texture* getBoxartTexture() const;
    void setBoxartTexture(SDL_Texture* texture);
};

#endif
