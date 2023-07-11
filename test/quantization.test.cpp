#include <cmath>
#include <catch.hpp>

#include "src/shared.h"
#include "src/quantization.h"

bool DoubleEqual(double a, double b)
{
    return std::fabs(a - b) < .0001;
}

TEST_CASE("Calculate covariance matrix", "[covariance_matrix]")
{
    PixelMatrix m(4, 3);
    m << 1.0, 2.0, 3.0,
        2.0, 4.0, 1.0,
        3.0, 1.0, 1.0,
        4.0, 1.0, 2.0;

    CovMatrix covariance = calculate_covariance_matrix(m);

    REQUIRE(DoubleEqual(covariance.row(0)(0), 1.6666666667));
    REQUIRE(DoubleEqual(covariance.row(0)(1), -1.0));
    REQUIRE(DoubleEqual(covariance.row(0)(2), -0.5));

    REQUIRE(DoubleEqual(covariance.row(1)(0), -1.0));
    REQUIRE(DoubleEqual(covariance.row(1)(1), 2.0));
    REQUIRE(DoubleEqual(covariance.row(1)(2), -0.3333333333333333));

    REQUIRE(DoubleEqual(covariance.row(2)(0), -0.5));
    REQUIRE(DoubleEqual(covariance.row(2)(1), -0.3333333333333333));
    REQUIRE(DoubleEqual(covariance.row(2)(2), 0.9166666666666667));
}

TEST_CASE("Determine largest eigenvalue and eigenvector", "[largest_eigenv]")
{
    VectorXd v;
    double d;
    CovMatrix c;
    c << 0.95, 2.0, 0.5,
        2.0, -1.45, -1.6666666667,
        0.5, -1.6666666667, 1.5;

    get_largest_eigenv(c, d, v);

    REQUIRE(DoubleEqual(d, 2.46261));

    REQUIRE(DoubleEqual(v(0), 0.490258));
    REQUIRE(DoubleEqual(v(1), 0.541518));
    REQUIRE(DoubleEqual(v(2), -0.682939));
}

TEST_CASE("Find cutting point in PCA scores", "[cutting_point]")
{
    VectorXd a(7), b(6), c(6);
    int idx_a, idx_b, idx_c;

    a << -4.0, -1.3, 0.1, 0.8, 2.0, 5.5, 9.1;
    b << -1.0, -0.8, -0.3, 0.2, 0.6, 1.0;
    c << -9.1, -5.5, -2.0, 0.1, 1.3, 4.0;

    idx_a = find_cutting_point_index(a);
    idx_b = find_cutting_point_index(b);
    idx_c = find_cutting_point_index(c);

    REQUIRE(idx_a == 4);
    REQUIRE(idx_b == 2);
    REQUIRE(idx_c == 1);
}

TEST_CASE("Sort PCA scores and data matrix", "[sorting_by_pca]")
{
    VectorXd _a(7), _b(6), _c(6), a(7), b(6), c(6);
    MatrixXd _x(7, 3), _y(6, 3), _z(6, 3), x(7, 3), y(6, 3), z(6, 3);

    _a << 0.8, 9.1, 2.0, 5.5, -4.0, -1.3, 0.1;
    _b << 1.0, 0.2, -0.3, -0.8, 0.6, -1.0;
    _c << -9.1, -5.5, -2.0, 0.1, 1.3, 4.0;

    _x << 3, 1, 2, // 3
        4, 5, 1,   // 6
        4, 9, 9,   // 4
        1, 2, 5,   // 5
        4, 2, 7,   // 0
        0, 7, 8,   // 1
        1, 0, 2;   // 2

    _y << 3, 1, 2, // 5
        4, 5, 1,   // 3
        4, 9, 9,   // 2
        1, 2, 5,   // 1
        4, 2, 7,   // 4
        0, 7, 8;   // 0

    _z = _y;

    sort_data_by_pca_score(_x, _a, x, a);
    sort_data_by_pca_score(_y, _b, y, b);
    sort_data_by_pca_score(_z, _c, z, c);

    REQUIRE((x.row(0)(0) == 4 && x.row(0)(1) == 2 && x.row(0)(2) == 7));
    REQUIRE((x.row(1)(0) == 0 && x.row(1)(1) == 7 && x.row(1)(2) == 8));
    REQUIRE((x.row(2)(0) == 1 && x.row(2)(1) == 0 && x.row(2)(2) == 2));
    REQUIRE((x.row(3)(0) == 3 && x.row(3)(1) == 1 && x.row(3)(2) == 2));
    REQUIRE((x.row(4)(0) == 4 && x.row(4)(1) == 9 && x.row(4)(2) == 9));
    REQUIRE((x.row(5)(0) == 1 && x.row(5)(1) == 2 && x.row(5)(2) == 5));
    REQUIRE((x.row(6)(0) == 4 && x.row(6)(1) == 5 && x.row(6)(2) == 1));

    REQUIRE((y.row(0)(0) == 0 && y.row(0)(1) == 7 && y.row(0)(2) == 8));
    REQUIRE((y.row(1)(0) == 1 && y.row(1)(1) == 2 && y.row(1)(2) == 5));
    REQUIRE((y.row(2)(0) == 4 && y.row(2)(1) == 9 && y.row(2)(2) == 9));
    REQUIRE((y.row(3)(0) == 4 && y.row(3)(1) == 5 && y.row(3)(2) == 1));
    REQUIRE((y.row(4)(0) == 4 && y.row(4)(1) == 2 && y.row(4)(2) == 7));
    REQUIRE((y.row(5)(0) == 3 && y.row(5)(1) == 1 && y.row(5)(2) == 2));

    REQUIRE((z.row(0)(0) == 3 && z.row(0)(1) == 1 && z.row(0)(2) == 2));
    REQUIRE((z.row(1)(0) == 4 && z.row(1)(1) == 5 && z.row(1)(2) == 1));
    REQUIRE((z.row(2)(0) == 4 && z.row(2)(1) == 9 && z.row(2)(2) == 9));
    REQUIRE((z.row(3)(0) == 1 && z.row(3)(1) == 2 && z.row(3)(2) == 5));
    REQUIRE((z.row(4)(0) == 4 && z.row(4)(1) == 2 && z.row(4)(2) == 7));
    REQUIRE((z.row(5)(0) == 0 && z.row(5)(1) == 7 && z.row(5)(2) == 8));
}