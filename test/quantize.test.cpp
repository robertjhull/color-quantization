#include <cmath>
#include <catch.hpp>

#include "src/shared.h"
#include "src/quantize.h"

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

    cov_matrix covariance = calculate_covariance_matrix(m);

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
    cov_matrix c;
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