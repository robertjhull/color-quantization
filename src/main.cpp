#include <iostream>
#include <chrono>
#include <stdexcept>
#include <string>

#include "lodepng.h"
#include "shared.h"
#include "image.h"
#include "quantize.h"
#include "dither.h"

using namespace std;

// void displayHelpMessages()
// {
//     cout << "Usage: main.exe [options] file\n"
//             "Options:\n"
//             "-h or --help              Display this information.\n"
//             "--target-help             Display target specific command line options.\n"
//             "-v or --version           Display program version information.\n"
//             "--palettes                Display list of supported palettes.\n"
//          << endl;
// }

void execute(Options &options)
{
    if (options.filename.empty())
    {
        cout << "Must provide a valid filename!" << endl;
        return;
    }

    unsigned width, height;
    vector<unsigned char> image = import_png_image(options.filename.c_str(), width, height);
    options.width = width;
    options.height = height;

    PixelMatrix pixelMatrix = convert_to_matrix(image);

    if (!options.paletteFileName.empty())
    {
        unsigned width1, height1;
        vector<unsigned char> palette = import_png_image(options.paletteFileName.c_str(), width1, height1);
        options.targetPalette = convert_to_matrix(palette);
    }

    quantize(pixelMatrix, options);

    // floydSteinbergDither(RgbMatrix, options.targetPalette, options.width);

    int error = write_rgb_image(options.outputFileName.c_str(), convert_to_vector(pixelMatrix), width, height);

    if (error)
    {
        std::cout << options.outputFileName << " encoding error: " << lodepng_error_text(error) << std::endl;
    }
}

int main(int argc, char *argv[])
{
    // Check if there are any command line arguments
    if (argc == 1)
    {
        cout << "No arguments provided." << endl;
        return 0;
    }

    string filename;
    string paletteFileName;

    // Default settings
    unsigned numColors = 16;
    bool logTime = true;
    string outputFilename = "output.png";

    // Process command line arguments
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        string png = ".png";
        string palettes = "palettes/";

        // Check for specific arguments
        if (arg == "-h" || arg == "--help")
        {
            // displayHelpMessages();
        }
        else if (arg == "-v" || arg == "--version")
        {
            // Display version information
            cout << "Version " << VERSION << endl;
        }
        else if (arg == "-palette")
        {
            // Use custom color palette
            paletteFileName = argv[i + 1];
        }
        else if (arg.find(png) != string::npos && arg.find(palettes) == string::npos)
        {
            filename = arg;
        }
        else
        {
            try
            {
                std::size_t pos;
                numColors = std::stoi(arg, &pos);
                if (pos < arg.size())
                {
                    std::cerr << "Trailing characters after number: " << arg << '\n';
                }
            }
            catch (std::invalid_argument const &ex)
            {
                std::cerr << "Invalid number: " << arg << '\n';
            }
            catch (std::out_of_range const &ex)
            {
                std::cerr << "Number out of range: " << arg << '\n';
            }
        }
    }

    Options options{filename, numColors, logTime, outputFilename, paletteFileName};

    if (!filename.empty())
        execute(options);

    return 0;
}