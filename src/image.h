#pragma once

#include "shared.h"

MatrixRgb to_matrix(std::vector<unsigned char> rgbImage);
MatrixRgb import_png_as_matrix(const char *filename, unsigned &width, unsigned &height);
std::vector<unsigned char> to_char_vector(MatrixRgb &matrixRgb);
int write_image_to_file(const char *filename, MatrixRgb &matrixRgb, unsigned width, unsigned height);