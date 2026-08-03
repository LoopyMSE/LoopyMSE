#pragma once

#include <SDL.h>

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace SDL::ImageWriter
{

const int IMAGE_TYPE_BMP = 1;
const int IMAGE_TYPE_PNG = 2;
const int IMAGE_TYPE_JPG = 3;
const int IMAGE_TYPE_DEFAULT = IMAGE_TYPE_PNG;

const double LOOPY_SEAL_ASPECT = 8.0 / 7.0;
const double LOOPY_SCREEN_ASPECT = 4.0 / 3.0;

int parse_image_type(std::string type, int _default);

fs::path image_extension(int image_type);
fs::path make_unique_name(std::string prefix = "", std::string suffix = "");

bool save_surface(int image_type, fs::path path, SDL_Surface* surf);
bool save_image_32bpp(
	int image_type, fs::path path, uint32_t width, uint32_t height, uint32_t data[], bool transparent = false,
	double correct_aspect = 0
);
bool save_image_16bpp(
	int image_type, fs::path path, uint32_t width, uint32_t height, uint16_t data[], bool transparent = false,
	double correct_aspect = 0
);
bool save_image_8bpp(
	int image_type, fs::path path, uint32_t width, uint32_t height, uint8_t data[], uint32_t num_colors,
	uint16_t palette[], bool transparent = false, double correct_aspect = 0
);

}  // namespace SDL::ImageWriter