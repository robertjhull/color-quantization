# Color Quantization with PCA & LDA

Experimenting with color quantization utilizing a combination of Principal Component Analysis (PCA) and Linear Discriminant Analysis (LDA) based off of the approach described in [this paper](https://www.semanticscholar.org/paper/Color-quantization-method-based-on-principal-and-Ueda-Koga/ee4dd342090aa5118e397292978b4aee017c1159). Uses [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) for matrix operations and [LodePNG](https://lodev.org/lodepng/) for loading .PNG files.

![before](https://github.com/robertjhull/color-quantization-tool/assets/67487694/d73da1f7-a2a8-4503-9304-fe0c31e9693a)
![after](https://github.com/robertjhull/color-quantization-tool/assets/67487694/23fabb41-8f76-4e5c-ad41-49ab1b6e8ff9)

## Prerequisites

- CMake (minimum v3.10).
- C++ compiler that supports C++17 standard.

## Building

```bash
git clone https://github.com/robertjhull/color-quantization
cd color-quantization
mkdir build
cd build
cmake ..
make
```

Run the program with the path to the image (must be .png) and the number of colors it should be reduced to.

```bash
# Example:
cq.exe ../test-image.png 8 
```
