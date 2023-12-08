cmake_minimum_required(VERSION 3.10)
project(ColorQuantization)

if(POLICY CMP0057)
    cmake_policy(SET CMP0057 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)  # or your desired C++ standard

include(FetchContent)
FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3147391d946bb4b6c68edd901f2add6ac1f31f8c 
)
set(EIGEN_BUILD_TESTING OFF)
FetchContent_MakeAvailable(eigen)

option(BUILD_TESTS "Build the tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(src)
