#pragma once
#include <SDL.h>

#include <filesystem>

#ifndef PROJECT_NAME
#pragma error "Missing defines from CMake"
#define PROJECT_NAME ""
#define PROJECT_VERSION ""
#define PROJECT_ORG ""
#define PROJECT_DESCRIPTION ""
#endif

namespace fs = std::filesystem;

static fs::path path_from_sdl(char *sdl_path) {
	fs::path path(sdl_path);
	SDL_free(sdl_path);
	return path;
}

const fs::path DEFAULT_BIOS_PATH = "bios.bin";
const fs::path DEFAULT_SOUND_BIOS_PATH = "soundbios.bin";
const fs::path CONTROLLER_DB_PATH = "gamecontrollerdb.txt";
const fs::path INI_PATH = "loopymse.ini";
const fs::path RESOURCE_PATH = path_from_sdl(SDL_GetBasePath());
const fs::path PREFS_PATH = 
#ifdef _WIN32
// Don't use %APPDATA% on windows, keep everything beside .exe
path_from_sdl(SDL_GetBasePath());
#else
path_from_sdl(SDL_GetPrefPath(PROJECT_ORG, PROJECT_NAME));
#endif
