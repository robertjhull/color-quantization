#include "pch/cqt_pch.h"

#include "quantization.h"
#include "palette.h"
#include "dither.h"

void floyd_steinberg_dither(MatrixRgb &originalMatrix, const std::vector<Pixel> colorPalette, const unsigned width)
{
    std::cout << "Dithering... ";

    double red_error, green_error, blue_error;

    unsigned limit = originalMatrix.rows();

    for (unsigned pixel = 0; pixel < limit; ++pixel)
    {
        Pixel closestColor = find_closest_pixel_value(originalMatrix.row(pixel), colorPalette);

        red_error = originalMatrix.row(pixel)(0) - closestColor(0);
        green_error = originalMatrix.row(pixel)(1) - closestColor(1);
        blue_error = originalMatrix.row(pixel)(2) - closestColor(2);

        originalMatrix.row(pixel) = closestColor;

        // Push quantization error onto neighboring pixels
        if (pixel + 1 < limit) // right neighbor
        {
            originalMatrix.row(pixel + 1)(0) += red_error * 7 / 16;
            originalMatrix.row(pixel + 1)(1) += green_error * 7 / 16;
            originalMatrix.row(pixel + 1)(2) += blue_error * 7 / 16;
        }

        if (pixel + width < limit) // bottom neighbor
        {
            originalMatrix.row(pixel + width)(0) += red_error * 5 / 16;
            originalMatrix.row(pixel + width)(1) += green_error * 5 / 16;
            originalMatrix.row(pixel + width)(2) += blue_error * 5 / 16;
        }

        if (pixel + width - 1 < limit) // bottom-left neighbor
        {
            originalMatrix.row(pixel + width - 1)(0) += red_error * 3 / 16;
            originalMatrix.row(pixel + width - 1)(1) += green_error * 3 / 16;
            originalMatrix.row(pixel + width - 1)(2) += blue_error * 3 / 16;
        }

        if (pixel + width + 1 < limit) // bottom-right neighbor
        {
            originalMatrix.row(pixel + width + 1)(0) += red_error * 1 / 16;
            originalMatrix.row(pixel + width + 1)(1) += green_error * 1 / 16;
            originalMatrix.row(pixel + width + 1)(2) += blue_error * 1 / 16;
        }
    }

    std::cout << "done." << std::endl;
}