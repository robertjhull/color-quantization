#include "pch/cqt_pch.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "src/shared.h"
#include "src/image.h"

TEST_CASE("Import .png and convert to Eigen Matrix", "[png_import]")
{
    using Catch::Matchers::WithinAbs;

    RgbMatrix mat;
    unsigned width, height;
    std::string target_file_path = "../test/testimage.png";

    mat = import_png_as_matrix(target_file_path.c_str(), width, height);

    double pixel_1_red = 195.0;
    double pixel_1_green = 134.0;
    double pixel_1_blue = 81.0;

    double pixel_2_red = 178.0;
    double pixel_2_green = 139.0;
    double pixel_2_blue = 80.0;

    double pixel_6_red = 200.0;
    double pixel_6_green = 195.0;
    double pixel_6_blue = 198.0;

    CHECK_THAT(mat.row(0)(0), WithinAbs(pixel_1_red, 0.1));
    CHECK_THAT(mat.row(0)(1), WithinAbs(pixel_1_green, 0.1));
    CHECK_THAT(mat.row(0)(2), WithinAbs(pixel_1_blue, 0.1));

    CHECK_THAT(mat.row(1)(0), WithinAbs(pixel_2_red, 0.1));
    CHECK_THAT(mat.row(1)(1), WithinAbs(pixel_2_green, 0.1));
    CHECK_THAT(mat.row(1)(2), WithinAbs(pixel_2_blue, 0.1));

    CHECK_THAT(mat.row(5)(0), WithinAbs(pixel_6_red, 0.1));
    CHECK_THAT(mat.row(5)(1), WithinAbs(pixel_6_green, 0.1));
    CHECK_THAT(mat.row(5)(2), WithinAbs(pixel_6_blue, 0.1));

    CHECK(mat.rows() == width * height);
}

TEST_CASE("Convert Eigen matrix to vector<unsigned char>", "[matrix_to_vector]")
{
    RgbMatrix m(7, 3);
    m << 3, 1, 2,
        4, 5, 1,
        4, 9, 9,
        1, 2, 5,
        4, 2, 7,
        0, 7, 8,
        1, 0, 2;

    std::vector<unsigned char> v = to_char_vector(m);

    CHECK(static_cast<double>(v[0]) == m.row(0)(0));
    CHECK(static_cast<double>(v[1]) == m.row(0)(1));
    CHECK(static_cast<double>(v[2]) == m.row(0)(2));
    CHECK(static_cast<double>(v[3]) == m.row(1)(0));
    CHECK(static_cast<double>(v[4]) == m.row(1)(1));
    CHECK(static_cast<double>(v[5]) == m.row(1)(2));
    CHECK(static_cast<double>(v[6]) == m.row(2)(0));
    CHECK(static_cast<double>(v[7]) == m.row(2)(1));
    CHECK(static_cast<double>(v[8]) == m.row(2)(2));
    CHECK(static_cast<double>(v[9]) == m.row(3)(0));
    CHECK(static_cast<double>(v[10]) == m.row(3)(1));
}