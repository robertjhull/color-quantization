#pragma once
#include "shared.h"

Palette get_reduced_palette(const vector<PixelSubset> &subsets);
Pixel find_closest_pixel_value(const Pixel &targetColor, const Palette &colorPalette);
bool color_has_been_used(const Pixel &targetColor, const vector<Pixel> colors_used);
void map_to_palette(PixelMatrix &originalImage, const Palette palette);