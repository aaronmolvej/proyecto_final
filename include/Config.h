#ifndef CONFIG_H
#define CONFIG_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;
const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;

const SDL_Color BG_COLOR = {30, 30, 50, 255};
const SDL_Color PRIMARY_COLOR = {100, 150, 255, 255};
const SDL_Color TEXT_COLOR = {255, 255, 255, 255};
const SDL_Color TEXT_SECONDARY_COLOR = {180, 180, 180, 255};
const SDL_Color BUTTON_COLOR = {70, 70, 100, 255};
const SDL_Color BUTTON_HOVER_COLOR = {90, 90, 120, 255};
const SDL_Color SELECTED_COLOR = {120, 180, 255, 255};


const std::string FONT_PATH = "assets/fonts/";
const std::string IMAGE_PATH = "assets/images/";
const std::string SOUND_PATH = "assets/sounds/";

const float USB_CHECK_INTERVAL = 5.0f;

enum class Console {
    ALL,
    NES,
    SNES,
    GBA
};

enum class Tab {
    BIBLIOTECA,
    FAVORITOS,
    ROMS
};


const std::string ROM_PATH_USB = "/media/";
const std::string ROM_PATH_NES = "/media/nes/"; 
const std::string ROM_PATH_SNES = "/media/snes/";
const std::string ROM_PATH_GBA = "/media/gba/";

const std::string ROM_PATH = "roms/";
const std::string LOCAL_ROM_PATH = "/home/pi/proyecto_final/roms/";
const std::string LOCAL_ROM_PATH_NES = "/home/pi/proyecto_final/roms/nes/";
const std::string LOCAL_ROM_PATH_SNES = "/home/pi/proyecto_final/roms/snes/";
const std::string LOCAL_ROM_PATH_GBA = "/home/pi/proyecto_final/roms/gba/";

const std::string BOXART_PATH = "media/boxart/";

const std::string EMULATOR_FCEUX = "/usr/games/mednafen";
const std::string EMULATOR_SNES9X = "/usr/games/mednafen";
const std::string EMULATOR_MGBA = "/usr/games/mednafen";

#endif
