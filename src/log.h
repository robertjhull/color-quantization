#pragma once

#include "shared.h"

enum LogOptions
{
    FILENAME = 1 << 0,
    DIMENSIONS = 1 << 1,
    TARGET_NCOLORS = 1 << 2,
    TARGET_PALETTE = 1 << 3
};

#define LOG_INFO(options, BIT)                                                                          \
    do                                                                                                  \
    {                                                                                                   \
        if (BIT & FILENAME)                                                                             \
        {                                                                                               \
            std::cout << "Image: " << options.filename << std::endl;                                    \
        }                                                                                               \
        if (BIT & DIMENSIONS)                                                                           \
        {                                                                                               \
            std::cout << "Dimensions: " << options.width << "x" << options.height << "px" << std::endl; \
        }                                                                                               \
        if (BIT & TARGET_NCOLORS)                                                                       \
        {                                                                                               \
            std::cout << "Target number of colors: " << options.targetNumColors << std::endl;           \
        }                                                                                               \
        if (BIT & TARGET_PALETTE && !options.paletteFileName.empty())                                   \
        {                                                                                               \
            std::cout << "Target palette: " << options.paletteFileName << std::endl;                    \
        }                                                                                               \
    } while (0);