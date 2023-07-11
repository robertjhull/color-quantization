#pragma once

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

void get_largest_eigenv(const CovMatrix &covariance, double &largestEigenvalue, VectorXd &largestEigenvector);
CovMatrix calculate_covariance_matrix(const MatrixXd &data);
VectorXd calculate_pca_scores(const PixelSubset &targetSubset);
Pixel find_closest_pixel_value(const Pixel &targetColor, const PixelMatrix &colorPalette);
int find_cutting_point_index(const VectorXd &sortedPcaScores);
void sort_data_by_pca_score(PixelMatrix &pixels, VectorXd pcaScores, PixelMatrix &sortedPixels, VectorXd &sortedPcaScores);
void partition(vector<PixelSubset> &subsets);
int determine_optimal_subset(vector<PixelSubset> &subsets);
void quantize(PixelMatrix &originalImage, const Options &options);