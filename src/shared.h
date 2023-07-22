#pragma once

#include "pch/cqt_pch.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

#define PBSTR "------------------------------------------------------------"
#define PBWIDTH 60
#define MAX_DOUBLE 1.79769e+308
#define MIN_DOUBLE 2.22507e-308

typedef Eigen::MatrixXd MatrixRgb;
typedef Eigen::RowVectorXd Pixel;
typedef Eigen::Matrix3d CovMatrix;
typedef Eigen::Matrix<unsigned char, Eigen::Dynamic, 3, Eigen::RowMajor> MatrixXuc;

struct PixelEqual
{
    bool operator()(const Pixel &p1, const Pixel &p2) const
    {
        return (std::fabs(p1(0) - p2(0)) < 0.1 && std::fabs(p1(1) - p2(1)) < 0.1 && std::fabs(p1(2) - p2(2)) < 0.1);
    }
};

typedef std::map<int, Pixel> ColorMap;

const std::string VERSION = "1.1";

typedef struct
{
    MatrixRgb data;
    CovMatrix covariance;
    double largestEigenvalue;
    Eigen::VectorXd largestEigenvector;
    std::vector<int> indices;
} PixelSubset;

typedef struct
{
    const std::string filename;
    const unsigned targetNumColors;
    const std::string outputFileName;
    const std::string paletteFileName;
    const std::vector<Pixel> targetPalette;
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

void static inline printProgress(const char *action, int completed, int total)
{
    double percentage = static_cast<double>(completed) / static_cast<double>(total);
    int lpad = static_cast<int>(percentage * static_cast<double>(PBWIDTH));
    int rpad = PBWIDTH - lpad;
    printf("\r%s: %3d / %3d [%.*s%*s]", action, completed, total, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

void static inline log_color_hex_values(std::vector<Pixel> &colors)
{
    std::cout << "Palette (Hex): ";
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
    std::cout << std::endl;
}