#include "quantization.h"
#include "palette.h"
#include "dither.h"

using namespace std;
using namespace Eigen;

void floyd_steinberg_dither(PixelMatrix &originalRgbMatrix, const Palette colorPalette, const unsigned width)
{
    cout << "Dithering... ";

    double red_error, green_error, blue_error;

    unsigned limit = originalRgbMatrix.rows();

    for (unsigned pixel = 0; pixel < limit; ++pixel)
    {
        Pixel closestColor = find_closest_pixel_value(originalRgbMatrix.row(pixel), colorPalette);

        red_error = originalRgbMatrix.row(pixel)(0) - closestColor(0);
        green_error = originalRgbMatrix.row(pixel)(1) - closestColor(1);
        blue_error = originalRgbMatrix.row(pixel)(2) - closestColor(2);

        originalRgbMatrix.row(pixel) = closestColor;

        // Push quantization error onto neighboring pixels
        if (pixel + 1 < limit) // right neighbor
        {
            originalRgbMatrix.row(pixel + 1)(0) += red_error * 7 / 16;
            originalRgbMatrix.row(pixel + 1)(1) += green_error * 7 / 16;
            originalRgbMatrix.row(pixel + 1)(2) += blue_error * 7 / 16;
        }

        if (pixel + width < limit) // bottom neighbor
        {
            originalRgbMatrix.row(pixel + width)(0) += red_error * 5 / 16;
            originalRgbMatrix.row(pixel + width)(1) += green_error * 5 / 16;
            originalRgbMatrix.row(pixel + width)(2) += blue_error * 5 / 16;
        }

        if (pixel + width - 1 < limit) // bottom-left neighbor
        {
            originalRgbMatrix.row(pixel + width - 1)(0) += red_error * 3 / 16;
            originalRgbMatrix.row(pixel + width - 1)(1) += green_error * 3 / 16;
            originalRgbMatrix.row(pixel + width - 1)(2) += blue_error * 3 / 16;
        }

        if (pixel + width + 1 < limit) // bottom-right neighbor
        {
            originalRgbMatrix.row(pixel + width + 1)(0) += red_error * 1 / 16;
            originalRgbMatrix.row(pixel + width + 1)(1) += green_error * 1 / 16;
            originalRgbMatrix.row(pixel + width + 1)(2) += blue_error * 1 / 16;
        }
    }

    cout << "done." << endl;
}