#include <catch2/catch_test_macros.hpp>

#include "src/shared.h"
#include "src/palette.h"

TEST_CASE("Find closest pixel value", "[closest_pixel]")
{
    Pixel p(3);
    Pixel p1(3), p2(3), p3(3);
    std::vector<Pixel> palette;

    p1 << 255, 0, 0;
    p2 << 0, 102, 0;
    p3 << 204, 255, 204;

    palette.emplace_back(p1);
    palette.emplace_back(p2);
    palette.emplace_back(p3);

    SECTION("Find closest pixel value to rgb(255, 0, 127)")
    {
        p << 255, 0, 127;

        Pixel result = find_closest_pixel_value(p, palette);
        CHECK(result == palette[0]);
    }
}

TEST_CASE("Get reduced palette", "[reduce_palette]")
{
    std::vector<PixelSubset> v(4);
    PixelSubset s1, s2, s3, s4;

    v.emplace_back(s1);
    v.emplace_back(s2);
    v.emplace_back(s3);
    v.emplace_back(s4);

    MatrixRgb m1(2, 3), m2(2, 3), m3(2, 3), m4(2, 3);

    m1 << 40, 50, 40,
        60, 100, 60;

    m2 << 10, 10, 10,
        100, 100, 100;

    m3 << 20, 50, 90,
        90, 50, 20;

    m4 << 40, 10, 40,
        140, 20, 60;

    v[0].data = m1;
    v[1].data = m1;
    v[2].data = m1;
    v[3].data = m1;

    std::vector<Pixel> palette = get_reduced_palette(v);

    CHECK(palette.size() == v.size());
    CHECK(palette[0][0] == 50);
    CHECK(palette[0][1] == 75);
    CHECK(palette[0][2] == 50);
}