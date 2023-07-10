#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <vector>
#include <numeric>
#include <cstdio>
#include <chrono>

#include "shared.h"
#include "dither.h"
#include "quantize.h"

// #define NDEBUG

using namespace std;
using namespace Eigen;
using namespace chrono;

cov_matrix calculate_covariance_matrix(const MatrixXd &data)
{
    cov_matrix covariance;
    MatrixXd mat = data;

    mat.rowwise() -= data.colwise().mean();

    covariance = (mat.transpose() * mat) / static_cast<double>(data.rows() - 1);

    return covariance;
}

void get_largest_eigenv(const cov_matrix &covariance, double &largestEigenvalue, VectorXd &largestEigenvector)
{
    int index;
    EigenSolver<PixelMatrix> solver(covariance, true);
    MatrixXd eigenVectors = solver.eigenvectors().real();
    VectorXd eigenValues = solver.eigenvalues().real();

    largestEigenvalue = eigenValues.maxCoeff(&index);
    largestEigenvector = eigenVectors.col(index);
}

VectorXd pca(const PixelMatrix &data)
{
    cout << "Performing Principal Component Analysis...";
    cov_matrix covariance;
    double largestEigenvalue;
    VectorXd largestEigenvector;
    VectorXd pcaScores;

    covariance = calculate_covariance_matrix(data);

    get_largest_eigenv(covariance, largestEigenvalue, largestEigenvector);

    VectorXd vec = VectorXd::Constant(data.rows(), 1.0);
    pcaScores = (data - (vec * data.colwise().mean())) * largestEigenvector;

    cout << " done." << endl;
    return pcaScores;
}

void test_print_first_three_rows_of_matrix(MatrixXd &data, string name)
{
    cout << endl
         << "========== " << name << " =========" << endl;
    for (unsigned i = 0; i < 3; i++)
    {
        cout << data.row(i)(0) << "    " << data.row(i)(1) << "    " << data.row(i)(2) << endl;
    }
    cout << "==================================" << endl
         << endl;
    ;
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

    /* Separability G(d) of point d* is defined as:
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

        double d_m1 = sortedPcaScores.head(n_w1).mean();
        double d_m2 = sortedPcaScores.tail(n_w2).mean();

        return n_w1 * n_w2 * pow(d_m1 - d_m2, 2);
    };

    while (l < r)
    {
        index = static_cast<unsigned>((r + l) / 2);

        point = separability(index);

        if (index < r && point < separability(index + 1))
        {
            l = index;
            continue;
        }

        else if (index > l && point < separability(index - 1))
        {
            r = index;
            continue;
        }

        else
        {
            cout << " done. [d*=" << point << " at Idx=" << index << "]" << endl;

            return index;
        }
    }
}

vector<PixelMatrix> lda_partition(const VectorXd pcaScores, const PixelMatrix pixels)
{
    vector<PixelMatrix> subsets;

    // Sort the pixel indices by PCA score and use to slice into new matrix.
    vector<int> indices(pcaScores.size());
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [&pcaScores](int i, int j)
         { return pcaScores(i) < pcaScores(j); }); // doesn't work on MatrixXd, so we sort the indices instead
    PixelMatrix sortedPixels = pixels(indices, Eigen::all);

    // Sorting PCA scores in ascending order.
    vector<double> scores(pcaScores.data(), pcaScores.data() + pcaScores.size());
    VectorXd sortedPcaScores(scores.size());
    sort(scores.begin(), scores.end()); // doesn't work on VectorXd, hence the conversion to vector and back
    copy(scores.begin(), scores.end(), sortedPcaScores.data());

    int cuttingPointIndex = find_cutting_point_index(sortedPcaScores);
    PixelMatrix pixelSubsetA = sortedPixels.topRows(cuttingPointIndex);
    PixelMatrix pixelSubsetB = sortedPixels.bottomRows(sortedPixels.rows() - cuttingPointIndex);
    subsets.emplace_back(pixelSubsetA);
    subsets.emplace_back(pixelSubsetB);

    return subsets;
}

PixelMatrix get_reduced_palette(const vector<PixelMatrix> &subsets)
{
    cout << "Reducing palette...";
    PixelMatrix reducedPalette(subsets.size(), 3);

    for (unsigned i = 0; i < subsets.size(); ++i)
    {
        reducedPalette.row(i) = subsets[i].colwise().mean();
    }

    cout << " done." << endl;
    log_color_hex_values(reducedPalette);

    return reducedPalette;
}

Pixel find_closest_pixel_value(const Pixel &targetColor, const PixelMatrix &colorPalette)
{
    int closestIndex = 0;
    double minDistance = MAX_DOUBLE;

    // Lambda for calculating Euclidean distance between two RGB values.
    auto calculateDistance = [](const Pixel &color1, const Pixel &color2)
    {
        double dr = color1[0] - color2[0];
        double dg = color1[1] - color2[1];
        double db = color1[2] - color2[2];
        return sqrt((dr * dr) + (dg * dg) + (db * db));
    };

    for (unsigned i = 0; i < colorPalette.rows(); ++i)
    {
        double distance = calculateDistance(targetColor, colorPalette.row(i));
        if (distance < minDistance)
        {
            minDistance = distance;
            closestIndex = i;
        }
    }

    return colorPalette.row(closestIndex);
}

bool color_has_been_used(const Pixel &targetColor, const vector<Pixel> colors_used)
{
    auto iter = std::find_if(colors_used.begin(), colors_used.end(), [&targetColor](const Pixel &color)
                             { return color.isApprox(targetColor); });

    return iter != colors_used.end();
}

void map_to_palette(PixelMatrix &originalImage, const PixelMatrix palette)
{
    vector<Pixel> colors_used;
    Pixel color = find_closest_pixel_value(originalImage.row(0), palette);
    colors_used.emplace_back(color);

    for (unsigned pixel = 0; pixel < originalImage.rows(); ++pixel)
    {
        color = find_closest_pixel_value(originalImage.row(pixel), palette);

        if (!color_has_been_used(color, colors_used))
            colors_used.emplace_back(color);

        originalImage.row(pixel) = color;
    }

    std::cout << colors_used.size() << " colors used out of palette of " << palette.rows() << endl;
}

/*
    Color quantization method based on principal component analysis and linear discriminant analysis
    for palette-based image generation.
*/
void quantize(PixelMatrix &originalImage, const Options &options)
{
    chrono::_V2::system_clock::time_point start, stop;

    // OPTIONAL LOGGING
    if (options.logTime)
    {
        cout << endl
             << "Starting color quantization of " << options.filename << "..." << endl
             << "Image dimensions: " << options.width << "x" << options.height << endl
             << "Target number of colors: " << options.targetNumColors << endl;

        if (!options.paletteFileName.empty())
            cout << "Custom palette loaded: " << options.paletteFileName << endl;

        start = chrono::high_resolution_clock::now();
    }

    PixelMatrix data = originalImage;

    // Initialize the vector for holding matrix subsets with the original matrix as the first element.
    vector<PixelMatrix> subsets;
    subsets.emplace_back(data);

    unsigned safeguard = 0;

    while (subsets.size() < options.targetNumColors)
    {
        // Loop should only execute N times for N colors.
        if (safeguard > options.targetNumColors)
            break;

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
        int largestSubsetIndex = 0;
        double largestValue = 0;
        for (unsigned x = 0; x < subsets.size(); ++x)
        {
            double value;
            cov_matrix cov = calculate_covariance_matrix(subsets[x]);

            EigenSolver<MatrixXd> solver(cov, true);

            VectorXd eigenvalues = solver.eigenvalues().real();

            value = eigenvalues.maxCoeff() * subsets[x].rows();

            if (value > largestValue)
            {
                largestSubsetIndex = x;
                largestValue = value;
            }
        }

        VectorXd pcaScores = pca(subsets[largestSubsetIndex]);
        vector<PixelMatrix> partitions = lda_partition(pcaScores, subsets[largestSubsetIndex]);

        subsets.erase(subsets.begin() + largestSubsetIndex);
        subsets.emplace_back(partitions[0]);
        subsets.emplace_back(partitions[1]);

        cout << subsets.size() << " subsets partitioned out of " << options.targetNumColors << endl;

        safeguard++;
    }

    cout << endl
         << "All subsets partitioned." << endl;

    // Get the reduced color palette from the partitioned subsets.
    PixelMatrix reducedPalette = get_reduced_palette(subsets);
    assert(reducedPalette.rows() == options.targetNumColors && "Reduced palette size should match the target number of colors!");

    map_to_palette(originalImage, reducedPalette);

    if (!options.paletteFileName.empty())
        map_to_palette(originalImage, options.targetPalette);

    // OPTIONAL LOGGING
    if (options.logTime)
    {
        stop = chrono::high_resolution_clock::now();
        // Print execution time in seconds
        auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
        std::cout << "Finished color quantization to " << options.targetNumColors << " colors finished in " << duration.count() << " seconds." << std::endl;
    }
}