#include "imgwriter.h"

#include <SDL_image.h>
#include <log/log.h>

#include <algorithm>
#include <cstring>
#include <ctime>
#include <fstream>
#include <vector>

namespace SDL::ImageWriter
{

constexpr int JPG_QUALITY = 90;
constexpr double PRINT_ASPECT_CORRECTION_PRESCALE = 4;

int parse_image_type(std::string type, int default_)
{
	std::transform(type.begin(), type.end(), type.begin(), [](unsigned char c) { return std::tolower(c); });
	if (type == "bmp" || type == ".bmp" || type == "bitmap")
	{
		return IMAGE_TYPE_BMP;
	}
	if (type == "jpg" || type == "jpeg" || type == ".jpg" || type == ".jpeg")
	{
		return IMAGE_TYPE_JPG;
	}
	if (type == "png" || type == ".png")
	{
		return IMAGE_TYPE_PNG;
	}
	return default_;
}

fs::path image_extension(int image_type)
{
	switch (image_type)
	{
	case IMAGE_TYPE_BMP:
		return {".bmp"};
	case IMAGE_TYPE_JPG:
		return {".jpg"};
	case IMAGE_TYPE_PNG:
		return {".png"};
	default:
		return {""};
	}
}

fs::path make_unique_name(std::string prefix, std::string suffix)
{
	static unsigned int unique_number = 1;

	std::time_t timestamp = std::time(nullptr);
	char timestamp_buffer[20];
	strftime(timestamp_buffer, sizeof(timestamp_buffer), "%Y%m%d_%H%M%S", std::localtime(&timestamp));

	return prefix + timestamp_buffer + "_" + std::to_string(unique_number++) + suffix;
}

bool save_surface(int image_type, fs::path path, SDL_Surface* surf)
{
	int status = -1;
	switch (image_type)
	{
	case IMAGE_TYPE_PNG:
		status = IMG_SavePNG(surf, path.string().c_str());
		break;
	case IMAGE_TYPE_JPG:
		status = IMG_SaveJPG(surf, path.string().c_str(), JPG_QUALITY);
		break;
	case IMAGE_TYPE_BMP:
	{
		// alpha channel in bitmaps don't behave well with e.g. Preview.app
		SDL_Surface* rgb = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB24, 0);
		status = SDL_SaveBMP(rgb, path.string().c_str());
		SDL_FreeSurface(rgb);
		break;
	}
	}
	return 0 == status;
}

bool save_image_32bpp(
	int image_type, fs::path path, uint32_t width, uint32_t height, uint32_t data[], bool _transparent,
	double correct_aspect
)
{
	SDL_Surface* surf =
		SDL_CreateRGBSurfaceWithFormatFrom(data, (int)width, (int)height, 32, (int)width * 4, SDL_PIXELFORMAT_ARGB8888);

	if (correct_aspect > 0)
	{
		double scaled_height = height * PRINT_ASPECT_CORRECTION_PRESCALE;
		double scaled_width = scaled_height * correct_aspect;
		SDL_Surface* scaled =
			SDL_CreateRGBSurfaceWithFormat(0, (int)scaled_width, (int)scaled_height, 32, SDL_PIXELFORMAT_ARGB8888);
		// No AA, sharper. Alternatively SDL_SoftStretchLinear for bilinear
		SDL_BlitScaled(surf, nullptr, scaled, nullptr);

		Log::debug("Rescaling image from %dx%d -> %dx%d", surf->w, surf->h, scaled->w, scaled->h);

		SDL_FreeSurface(surf);
		surf = scaled;
	}

	bool success = save_surface(image_type, path, surf);

	// Should not free the pixel data since we created it with a *From
	SDL_FreeSurface(surf);

	return success;
}

static inline uint32_t color_16bpp_to_argb(uint16_t c)
{
	uint8_t r = ((c >> 10) & 31) * 255 / 31;
	uint8_t g = ((c >> 5) & 31) * 255 / 31;
	uint8_t b = (c & 31) * 255 / 31;
	uint8_t a = (c >> 15) * 255;
	return (a << 24) | (r << 16) | (g << 8) | b;
}

bool save_image_16bpp(
	int image_type, fs::path path, uint32_t width, uint32_t height, uint16_t data[], bool transparent,
	double correct_aspect
)
{
	unsigned int num_pixels = width * height;
	uint32_t data_argb[num_pixels];

	uint16_t alpha_set = transparent ? 0 : 0x8000;

	for (int i = 0; i < num_pixels; i++)
	{
		data_argb[i] = color_16bpp_to_argb(data[i] | alpha_set);
	}

	return save_image_32bpp(image_type, path, width, height, data_argb, transparent, correct_aspect);
}

bool save_image_8bpp(
	int image_type, fs::path path, uint32_t width, uint32_t height, uint8_t data[], uint32_t num_colors,
	uint16_t palette[], bool transparent, double correct_aspect
)
{
	unsigned int num_pixels = width * height;
	uint16_t data_16bpp[num_pixels];

	for (int i = 0; i < num_pixels; i++)
	{
		uint8_t pixel = data[i];
		if (pixel >= num_colors) pixel = num_colors - 1;
		data_16bpp[i] = palette[pixel];
	}

	return save_image_16bpp(image_type, path, width, height, data_16bpp, transparent, correct_aspect);
}

}  // namespace SDL::ImageWriter
