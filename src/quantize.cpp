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

VectorXd pca(const PixelMatrix &rgbMatrix)
{
    cout << "Performing Principal Component Analysis...";
    PixelMatrix data = rgbMatrix;

    // Center the data by subtracting the mean of each column.
    VectorXd mean = data.colwise().mean();
    data.rowwise() -= mean.transpose();

    EigenSolver<PixelMatrix> solver(data.transpose() * data);
    MatrixXd eigenVectors = solver.eigenvectors().real();
    VectorXd eigenValues = solver.eigenvalues().real();

    // Sort eigenvalues in descending order.
    vector<int> sortedIndices{0, 1, 2};
    sort(sortedIndices.begin(), sortedIndices.end(),
         [&eigenValues](int i, int j)
         { return eigenValues(i) > eigenValues(j); });

    // Use the first (largest) eigenvector to calculate PCA scores.
    cout << " done." << endl;
    return data * eigenVectors.col(sortedIndices[0]);
}

vector<PixelMatrix> lda_partition(const VectorXd pcaScores, const PixelMatrix pixels)
{
    cout << "Calculating optimal cutting point..." << endl;

    vector<PixelMatrix> subsets;

    unsigned cuttingPointIndex;
    double maxSeparability = MIN_DOUBLE;
    double optimalCuttingPoint = MIN_DOUBLE;

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

    /*
    Find optimal cutting point d* based on maximum separability G(d). Maximum separability is defined as:

          G(d) = w_1(d) * w_2(d) * (m_1(d) - m_2(d))^2

    Where w_1(d) and m_2(d) are the number of pixels having a PCA score less than or equal to d* and the
    mean of these scores, respectively. Similarly, w_2(d) and m_2(d) are the number of pixels having a PCA
    score greater than d* and the mean of these scores, respectively.
    */
    for (unsigned i = 0; i < sortedPcaScores.size();)
    {
        double cuttingPoint = sortedPcaScores(i);

        int lessThanOrEqual = i + 1;
        int greaterThan = scores.size() - lessThanOrEqual;

        if (lessThanOrEqual == 0 || greaterThan == 0)
        {
            i += static_cast<unsigned>(scores.size() * 0.0001);
            continue;
        }

        double meanLessThanOrEqual = sortedPcaScores.head(lessThanOrEqual).mean();
        double meanGreaterThan = sortedPcaScores.tail(greaterThan).mean();

        double separability = lessThanOrEqual * greaterThan * pow(meanLessThanOrEqual - meanGreaterThan, 2);

        if (separability > maxSeparability)
        {
            maxSeparability = separability;
            optimalCuttingPoint = cuttingPoint;
            cuttingPointIndex = i;
        }

        // Samples only part of the matrix to improve performance. Need to test this for accuracy.
        // TODO: Control this dynamically.
        i += static_cast<unsigned>(sortedPcaScores.size() * 0.0001);

        printProgress(static_cast<double>(i) / static_cast<double>(sortedPcaScores.size() - 1));
    }

    cout << endl
         << "Partitioning pixels around optimal cutting point " << optimalCuttingPoint << endl;

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

        // Locate the largest current subset for partitioning.
        sort(subsets.begin(), subsets.end(), [](const PixelMatrix a, const PixelMatrix b)
             { return a.rows() > b.rows(); });

        VectorXd pcaScores = pca(subsets[0]);
        vector<PixelMatrix> partitions = lda_partition(pcaScores, subsets[0]);

        subsets.erase(subsets.begin());
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