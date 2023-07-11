#include "shared.h"
#include "image.h"

std::vector<unsigned char> import_png_image(const char *filename, unsigned &width, unsigned &height)
{
    std::vector<unsigned char> png;
    std::vector<unsigned char> image; // the raw pixels
    lodepng::State state;             // optionally customize this

    state.info_raw.colortype = LCT_RGB; // RGB format (no alpha channel)

    unsigned error = lodepng::load_file(png, filename); // load image with given filename
    if (!error)
        error = lodepng::decode(image, width, height, state, png);

    if (error)
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    // the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA
    // State state contains extra information about the PNG such as text chunks

    return image;
}

int write_rgb_image(const char *filename, std::vector<unsigned char> image, unsigned width, unsigned height)
{
    assert(image.size() == width * height * 3 && "Image dimensions do not match!");
    unsigned error = lodepng::encode(filename, image, width, height, LCT_RGB);

    if (error)
    {
        std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return 1;
    }

    return 0;
}

// Flatten the image by concatenating the RGB values of each pixel row-wise, resulting in
// a matrix where each row represents a pixel and each column represents a color channel.
PixelMatrix convert_to_matrix(std::vector<unsigned char> rgbImage)
{
    PixelMatrix rgbMatrix;
    unsigned n = rgbImage.size();

    rgbMatrix.resize(n / 3, 3);

    for (unsigned y = 0; y < n / 3; ++y)
    {
        for (unsigned x = 0; x < 3; ++x)
        {
            rgbMatrix(y, x) = rgbImage[(y * 3) + x];
        }
    }

    return rgbMatrix;
}

std::vector<unsigned char> convert_to_vector(PixelMatrix &pixelMatrix)
{
    std::vector<unsigned char> image;

    for (int i = 0; i < pixelMatrix.rows(); ++i)
    {
        for (int j = 0; j < pixelMatrix.cols(); ++j)
        {
            image.push_back(static_cast<unsigned char>(pixelMatrix(i, j)));
        }
    }

    return image;
}