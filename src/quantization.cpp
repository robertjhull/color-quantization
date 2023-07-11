#include "shared.h"
#include "dither.h"
#include "quantization.h"
#include "palette.h"
#include "log.h"

// #define NDEBUG

using namespace std;
using namespace Eigen;
using namespace chrono;

/*
 * Calculates the variance-covariance matrix from the given pixel data. This is then used to determine
 * eigenvalues and eigenvectors of the given data.
 */
CovMatrix calculate_covariance_matrix(const MatrixXd &data)
{
    CovMatrix covariance;
    MatrixXd mat = data;

    mat.rowwise() -= data.colwise().mean();

    covariance = (mat.transpose() * mat) / static_cast<double>(data.rows() - 1);

    return covariance;
}

/*
 * Calculates the first (largest) eigenvalue and corresponding eigenvalue from a given covariance matrix.
 */
void get_largest_eigenv(const CovMatrix &covariance, double &largestEigenvalue, VectorXd &largestEigenvector)
{
    int index;
    EigenSolver<PixelMatrix> solver(covariance, true);
    MatrixXd eigenVectors = solver.eigenvectors().real();
    VectorXd eigenValues = solver.eigenvalues().real();

    largestEigenvalue = eigenValues.maxCoeff(&index);
    largestEigenvector = eigenVectors.col(index);
}

/*
 * Calculates the principal component scores of the pixels contained in the target subset by the following
 * equation:
 *
 *   scores = (X - 1Ne * mX) * V
 *
 * Where mX is a vector comprising the component-wise (column-wise) means of the pixel set X, 1Ne is an
 * N-dimensional column vector with all elements initialized to 1.0, and V is the eigenvector corresponding
 * to the largest eigenvalue.
 */
VectorXd calculate_pca_scores(const PixelSubset &targetSubset)
{
    VectorXd vec = VectorXd::Constant(targetSubset.data.rows(), 1.0);
    return (targetSubset.data - (vec * targetSubset.data.colwise().mean())) * targetSubset.largestEigenvector;
}

/*
 * Find the optimal cutting point in a sorted list of PCA scores based on comparing maximum separability.
 */
int find_cutting_point_index(const VectorXd &sortedPcaScores)
{
    cout << "Locating optimal cutting point...";

    unsigned index;
    double point;
    unsigned l = 0, r = sortedPcaScores.size();

    /*
     * Separability G(d) of point d* is defined as:
     *
     *     G(d) = w_1(d) * w_2(d) * (m_1(d) - m_2(d))^2
     *
     * Where w_1(d) and m_2(d) are the number of pixels having a PCA score less than or equal to d* and the
     * mean of these scores, respectively. Similarly, w_2(d) and m_2(d) are the number of pixels having a PCA
     * score greater than d* and the mean of these scores, respectively.
     */
    auto separability = [&sortedPcaScores](unsigned i)
    {
        int n_w1 = i + 1;
        int n_w2 = sortedPcaScores.size() - n_w1;

        if (n_w2 == 0)
            return 0.0;

        double d_m1 = sortedPcaScores.head(n_w1).mean();
        double d_m2 = sortedPcaScores.tail(n_w2).mean();

        return n_w1 * n_w2 * pow(d_m1 - d_m2, 2);
    };

    /*
     * Finds max separability by binary search. The PCA scores have a clear "peak" in the sorted vector which
     * is found by comparing neighbor separability scores.
     */
    while (l < r)
    {
        index = static_cast<unsigned>((r + l) / 2);

        point = separability(index);

        if (index < r && point < separability(index + 1))
        {
            l = index;
        }

        else if (index > l && point < separability(index - 1))
        {
            r = index;
        }

        else
        {
            cout << " done. [d*=" << point << " at Idx=" << index << "]" << endl;
            return index;
        }
    }
}

void sort_data_by_pca_score(PixelMatrix &pixels, VectorXd pcaScores, PixelMatrix &sortedPixels, VectorXd &sortedPcaScores)
{
    // Sort the pixel indices by PCA score and use to slice into new matrix.
    vector<int> indices(pcaScores.size());
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [&pcaScores](int i, int j)
         { return pcaScores(i) < pcaScores(j); }); // doesn't work on MatrixXd, so we sort the indices instead
    sortedPixels = pixels(indices, Eigen::all);

    // Sorting PCA scores in ascending order.
    vector<double> scores(pcaScores.data(), pcaScores.data() + pcaScores.size());
    sort(scores.begin(), scores.end()); // doesn't work on VectorXd, hence the conversion to vector and back
    copy(scores.begin(), scores.end(), sortedPcaScores.data());
}

/*
 * Determine the optimal subset for partitioning. This is determined by the following criteria:
 *
 * (i) A subset that forms a broader distribution takes priority over other subsets.
 * (ii) A subset that contains a larger number of pixels takes priority over other subsets.
 *
 * This criteria also can be represented by the following equation:
 *
 *   optimal_subset = arg max (Ae * Ne)
 *
 * Where Ae is the largest eigenvalue of the covariance matrix of the subset, and Ne is the
 * number of elements contained in the subset
 */
int determine_optimal_subset(vector<PixelSubset> &subsets)
{
    double value;
    double largestValue = 0;
    double largestSubsetIndex = 0;

    for (unsigned x = 0; x < subsets.size(); ++x)
    {
        subsets[x].covariance = calculate_covariance_matrix(subsets[x].data);

        get_largest_eigenv(subsets[x].covariance, subsets[x].largestEigenvalue, subsets[x].largestEigenvector);

        value = subsets[x].largestEigenvalue * subsets[x].data.rows();

        if (value > largestValue)
        {
            largestSubsetIndex = x;
            largestValue = value;
        }
    }

    return largestSubsetIndex;
}

/*
 * Utilizes Linear Discriminant Analysis (LDA) to select an optimal subset of pixels out of the data and partition
 * it on the basis of the of the PCA scores that maximize the separability.
 */
void partition(vector<PixelSubset> &subsets)
{
    int subsetIndex = determine_optimal_subset(subsets);

    VectorXd pcaScores = calculate_pca_scores(subsets[subsetIndex]);

    PixelMatrix sortedPixels;
    PixelSubset pixelSubsetA, pixelSubsetB;
    VectorXd sortedPcaScores(pcaScores.size());

    sort_data_by_pca_score(subsets[subsetIndex].data, pcaScores, sortedPixels, sortedPcaScores);

    int cuttingPointIndex = find_cutting_point_index(pcaScores);

    pixelSubsetA.data = sortedPixels.topRows(cuttingPointIndex);
    pixelSubsetB.data = sortedPixels.bottomRows(sortedPixels.rows() - cuttingPointIndex);

    subsets.erase(subsets.begin() + subsetIndex);

    subsets.emplace_back(pixelSubsetA);
    subsets.emplace_back(pixelSubsetB);
}

/*
    Color quantization method based on principal component analysis and linear discriminant analysis
    for palette-based image generation.
*/
void quantize(PixelMatrix &originalImage, const Options &options)
{
    vector<PixelSubset> subsets;

    LOG_INFO(options, (FILENAME | DIMENSIONS | TARGET_NCOLORS | TARGET_PALETTE));

#ifdef LOG_TIME
    chrono::_V2::system_clock::time_point start, stop;
    start = chrono::high_resolution_clock::now();
#endif

    // Initialize the vector for holding matrix subsets with the original matrix as the first element.
    PixelSubset initialSubset;
    initialSubset.data = originalImage;
    subsets.emplace_back(initialSubset);

    unsigned safeguard = 0;

    while (subsets.size() < options.targetNumColors)
    {
        // Loop should only execute N times for N colors.
        if (safeguard > options.targetNumColors)
            break;

        partition(subsets);

        cout << subsets.size() << " subsets partitioned out of " << options.targetNumColors << endl;

        safeguard++;
    }

    // Get the reduced color palette from the partitioned subsets.
    Palette reducedPalette = get_reduced_palette(subsets);

    assert(reducedPalette.rows() == options.targetNumColors && "Reduced palette size should match the target number of colors!");

    map_to_palette(originalImage, reducedPalette);

// OPTIONAL LOGGING
#ifndef LOG_TIME
    cout << "Finished." << endl;
#endif
#ifdef LOG_TIME
    stop = chrono::high_resolution_clock::now();
    // Print execution time in seconds
    auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
    std::cout << "Finished color quantization to " << options.targetNumColors << " colors finished in " << duration.count() << " seconds." << std::endl;
#endif
}