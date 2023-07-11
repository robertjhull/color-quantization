#pragma once

#include "shared.h"

void get_largest_eigenv(const CovMatrix &covariance, double &largestEigenvalue, VectorXd &largestEigenvector);
CovMatrix calculate_covariance_matrix(const MatrixXd &data);
VectorXd calculate_pca_scores(const PixelSubset &targetSubset);
int find_cutting_point_index(const VectorXd &sortedPcaScores);
void sort_data_by_pca_score(PixelMatrix &pixels, VectorXd pcaScores, PixelMatrix &sortedPixels, VectorXd &sortedPcaScores);
void partition(vector<PixelSubset> &subsets);
int determine_optimal_subset(vector<PixelSubset> &subsets);
void quantize(PixelMatrix &originalImage, const Options &options);