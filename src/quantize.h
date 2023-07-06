#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>
#include <string>

#include "shared.h"

// Custom equality comparer for Eigen::RowVectorXd
// struct PixelEqual
// {
//     bool operator()(const Pixel &p1, const Pixel &p2) const
//     {
//         return p1.isApprox(p2);
//     }
// };

// // Custom hash function specialization for Eigen::RowVectorXd
// struct PixelHash
// {
//     std::size_t operator()(const Eigen::RowVectorXd &vector) const
//     {
//         // Use Eigen's built-in hash function
//         std::size_t seed = 0;
//         for (Eigen::Index i = 0; i < vector.size(); ++i)
//         {
//             auto elem = *(vector.data() + i);
//             seed ^= std::hash<typename RowVectorXd::Scalar>()(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//         }
//         return seed;
//     }
// };

Pixel find_closest_pixel_value(const Pixel &targetColor, const PixelMatrix &colorPalette);
void quantize(PixelMatrix &originalImage, const Options &options);