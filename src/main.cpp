#include "pch/cqt_pch.h"

#include "shared.h"
#include "image.h"
#include "quantization.h"
#include "dither.h"

using namespace std;

void execute(Options &options)
{
    if (options.filename.empty())
    {
        cout << "Must provide a valid filename!" << endl;
        return;
    }

    MatrixRgb matrixRgb = import_png_as_matrix(options.filename.c_str(), options.width, options.height);

    quantize(matrixRgb, options);

    write_image_to_file(options.outputFileName.c_str(), matrixRgb, options.width, options.height);
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

    Options options{filename, numColors, outputFilename, paletteFileName};

    if (!filename.empty())
        execute(options);

    return 0;
}