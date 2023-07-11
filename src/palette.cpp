#include "palette.h"

Palette get_reduced_palette(const vector<PixelSubset> &subsets)
{
    cout << "Reducing palette...";
    Palette reducedPalette(subsets.size(), 3);

    for (unsigned i = 0; i < subsets.size(); ++i)
    {
        reducedPalette.row(i) = subsets[i].data.colwise().mean();
    }

    cout << " done." << endl;
    log_color_hex_values(reducedPalette);

    return reducedPalette;
}

Pixel find_closest_pixel_value(const Pixel &targetColor, const Palette &colorPalette)
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

    for (unsigned i = 0; i < colorPalette.rows(); ++i)
    {
        double distance = calculateDistance(targetColor, colorPalette.row(i));
        if (distance < minDistance)
        {
            minDistance = distance;
            closestIndex = i;
        }
    }

    return colorPalette.row(closestIndex);
}

bool color_has_been_used(const Pixel &targetColor, const vector<Pixel> colors_used)
{
    auto iter = std::find_if(colors_used.begin(), colors_used.end(), [&targetColor](const Pixel &color)
                             { return color.isApprox(targetColor); });

    return iter != colors_used.end();
}

void map_to_palette(PixelMatrix &originalImage, const Palette palette)
{
    vector<Pixel> colors_used;
    Pixel color = find_closest_pixel_value(originalImage.row(0), palette);
    colors_used.emplace_back(color);

    for (unsigned pixel = 0; pixel < originalImage.rows(); ++pixel)
    {
        color = find_closest_pixel_value(originalImage.row(pixel), palette);

        if (!color_has_been_used(color, colors_used))
            colors_used.emplace_back(color);

        originalImage.row(pixel) = color;
    }

    std::cout << colors_used.size() << " colors used out of palette of " << palette.rows() << endl;
}