cmake_minimum_required (VERSION 2.8)
project (bfio)

option(BFIO_EXAMPLES "Build bdio examples" ON)
option(BFIO_TESTS "Build bdio tests" OFF)

include_directories(include)

if (BFIO_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BFIO_TESTS)
    add_subdirectory(tests)
endif()
