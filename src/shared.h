#pragma once

#include "pch/cqt_pc.h"

#define PixelMatrix Eigen::MatrixXd
#define Pixel Eigen::RowVectorXd
#define CovMatrix Eigen::Matrix3d
#define Palette vector<Pixel>

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60
#define MAX_DOUBLE 1.79769e+308
#define MIN_DOUBLE 2.22507e-308

using namespace std;
using namespace Eigen;

const string VERSION = "1.1";

typedef struct
{
    PixelMatrix data;
    CovMatrix covariance;
    double largestEigenvalue;
    VectorXd largestEigenvector;
} PixelSubset;

typedef struct
{
    const string filename;
    const unsigned targetNumColors;
    const string outputFileName;
    const string paletteFileName;
    Palette targetPalette;
    unsigned width;
    unsigned height;
    bool dither;
} Options;

void static inline printProgress(double percentage)
{
    int val = static_cast<int>(percentage * 100.0);
    int lpad = static_cast<int>(percentage * static_cast<double>(PBWIDTH));
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

void static inline log_color_hex_values(Palette &colors)
{
    cout << "Palette (Hex): ";
    for (unsigned i = 0; i < colors.size(); i++)
    {
        int r = static_cast<int>(colors[i](0));
        int g = static_cast<int>(colors[i](1));
        int b = static_cast<int>(colors[i](2));

        // Convert RGB values to hex string
        std::stringstream ss;
        ss << std::uppercase << std::setfill('0') << std::hex
           << std::setw(2) << r
           << std::setw(2) << g
           << std::setw(2) << b;

        std::cout << "#" << ss.str() << " ";
    }
    cout << endl;
}