add_executable(cq main.cpp dither.cpp image.cpp lodepng.cpp palette.cpp quantization.cpp)  # Replace with your source files
target_include_directories(cq PRIVATE ${eigen_SOURCE_DIR})
