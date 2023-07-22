#include "pch/cqt_pch.h"

#include "palette.h"

std::vector<Pixel> get_reduced_palette(const std::vector<PixelSubset> &subsets)
{
    std::cout << "\nReducing palette...";

    std::vector<Pixel> palette;

    for (unsigned i = 0; i < subsets.size(); ++i)
    {
        Pixel color = subsets[i].data.colwise().mean();
        palette.emplace_back(color);
    }

    std::cout << " done." << std::endl;

    return palette;
}

Pixel find_closest_pixel_value(const Pixel &targetColor, const std::vector<Pixel> &colorPalette)
{
    int closestIndex = 0;
    double minDistance = MAX_DOUBLE;

    // Lambda for calculating Euclidean distance between two RGB values.
    auto calculateDistance = [](const Pixel &color1, const Pixel &color2)
    {
        double dr = color1[0] - color2[0];
        double dg = color1[1] - color2[1];
        double db = color1[2] - color2[2];
        return sqrt((dr * dr) + (dg * dg) + (db * db));
    };

    for (unsigned i = 0; i < colorPalette.size(); ++i)
    {
        double distance = calculateDistance(targetColor, colorPalette[i]);
        if (distance < minDistance)
        {
            minDistance = distance;
            closestIndex = i;
        }
    }

    return colorPalette[closestIndex];
}

bool color_has_been_used(const Pixel &targetColor, const std::vector<Pixel> colors_used)
{
    auto iter = std::find_if(colors_used.begin(), colors_used.end(), [&targetColor](const Pixel &color)
                             { return color.isApprox(targetColor); });

    return iter != colors_used.end();
}

void map_to_palette(MatrixRgb &originalImage, std::vector<Pixel> &palette)
{
    for (Eigen::Index pixel = 0; pixel < originalImage.rows(); ++pixel)
    {
        Pixel newColor = find_closest_pixel_value(originalImage.row(pixel), palette);
        originalImage.row(pixel) = newColor;
    }
}

// TODO: figure out the problem of mapping between palettes and using all colors