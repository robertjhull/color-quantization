#include "pch/cqt_pch.h"
#include "shared.h"
#include "lodepng.h"
#include "image.h"

// Flatten the image by concatenating the RGB values of each pixel row-wise, resulting in
// a matrix where each row represents a pixel and each column represents a color channel.
RgbMatrix to_matrix(std::vector<unsigned char> rgbImage)
{
    const int rows = rgbImage.size() / 3;
    const int cols = 3;

    Eigen::Map<MatrixXuc> result(rgbImage.data(), rows, cols);

    return result.cast<double>();
}

RgbMatrix import_png_as_matrix(const char *filename, unsigned &width, unsigned &height)
{
    std::vector<unsigned char> png;
    std::vector<unsigned char> image; // the raw pixels
    lodepng::State state;             // optionally customize this

    state.info_raw.colortype = LCT_RGB; // RGB format (no alpha channel)

    unsigned error = lodepng::load_file(png, filename); // load image with given filename

    if (!error)
    {
        // Decode pixels into the vector `image`, 3 bytes per pixel, ordered RGBRGB...
        error = lodepng::decode(image, width, height, state, png);
    }

    if (error)
    {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }

    return to_matrix(image);
}

// TODO: now working, clean up
std::vector<unsigned char> to_char_vector(RgbMatrix &rgbMatrix)
{
    // Map the data from Eigen matrix to a std::vector
    MatrixXuc mappedMatrix = rgbMatrix.cast<unsigned char>();

    // Create a std::vector<unsigned char> and copy the data from the Eigen matrix
    std::vector<unsigned char> result(mappedMatrix.data(), mappedMatrix.data() + mappedMatrix.size());

    return result;
}

int write_image_to_file(const char *filename, RgbMatrix &rgbMatrix, unsigned width, unsigned height)
{
    assert(rgbMatrix.rows() == width * height && "Image dimensions do not match!");

    unsigned error = lodepng::encode(filename, to_char_vector(rgbMatrix), width, height, LCT_RGB);

    if (error)
    {
        std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return 1;
    }

    return 0;
}