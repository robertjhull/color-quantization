#pragma once

#include "shared.h"

void floyd_steinberg_dither(MatrixRgb &originalMatrix, const std::vector<Pixel> colorPalette, const unsigned width);