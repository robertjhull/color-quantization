#pragma once

#include "shared.h"

std::vector<unsigned char> import_png_image(const char *filename, unsigned &width, unsigned &height);
int write_rgb_image(const char *filename, std::vector<unsigned char> image, unsigned width, unsigned height);
PixelMatrix convert_to_matrix(std::vector<unsigned char> rgbImage);
std::vector<unsigned char> convert_to_vector(PixelMatrix &pixelMatrix);