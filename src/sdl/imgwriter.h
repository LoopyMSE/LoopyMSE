#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace SDL::ImageWriter
{

constexpr static int IMAGE_TYPE_BMP = 1;
constexpr static int IMAGE_TYPE_DEFAULT = IMAGE_TYPE_BMP;

constexpr double LOOPY_SEAL_PAR = 42.0 / 32.0;
constexpr double LOOPY_SCREEN_PAR = 4.0 / 3.0;

int parse_image_type(std::string type, int default_);

fs::path image_extension(int image_type);
fs::path make_unique_name(std::string prefix = "", std::string suffix = "");

bool save_image_16bpp(
	int image_type, fs::path path, uint32_t width, uint32_t height, uint16_t data[], bool transparent = false,
	double correct_aspect = 0
);
bool save_image_8bpp(
	int image_type, fs::path path, uint32_t width, uint32_t height, uint8_t data[], uint32_t num_colors,
	uint16_t palette[], bool transparent = false, double correct_aspect = 0
);

}  // namespace SDL::ImageWriter