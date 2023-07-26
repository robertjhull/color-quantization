#pragma once
#include "shared.h"

std::vector<Pixel> get_reduced_palette(const std::vector<PixelSubset> &subsets);
Pixel find_closest_pixel_value(const Pixel &targetColor, const std::vector<Pixel> &colorPalette);
void map_to_palette(MatrixRgb &originalImage, std::vector<Pixel> &palette);