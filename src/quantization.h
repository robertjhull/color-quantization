#pragma once

#include "shared.h"

using namespace Eigen;

void get_largest_eigenv(const CovMatrix &covariance, double &largestEigenvalue, VectorXd &largestEigenvector);
CovMatrix calculate_covariance_matrix(const MatrixXd &data);
Eigen::VectorXd calculate_pca_scores(const PixelSubset &targetSubset);
int find_cutting_point_index(const VectorXd &sortedPcaScores);
void sort_data_by_pca_score(MatrixRgb &pixels, VectorXd pcaScores, MatrixRgb &sortedPixels, VectorXd &sortedPcaScores);
void partition(std::vector<PixelSubset> &subsets);
int determine_optimal_subset(std::vector<PixelSubset> &subsets);
void quantize(MatrixRgb &originalImage, const Options &options);