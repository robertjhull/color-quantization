if(BUILD_TESTS)
    # Fetch Catch2 using FetchContent, if not already present
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG 3f0283de7a9c43200033da996ff9093be3ac84dc  # specify the version tag
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(cq-test test.cpp)
    target_link_libraries(cq-test PRIVATE Catch2::Catch2)
endif()