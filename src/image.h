#pragma once

#include "shared.h"

RgbMatrix to_matrix(std::vector<unsigned char> rgbImage);
RgbMatrix import_png_as_matrix(const char *filename, unsigned &width, unsigned &height);
std::vector<unsigned char> to_char_vector(RgbMatrix &rgbMatrix);
int write_image_to_file(const char *filename, RgbMatrix &rgbMatrix, unsigned width, unsigned height);