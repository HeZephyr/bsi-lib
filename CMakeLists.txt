cmake_minimum_required(VERSION 3.10)
project(BSI_Implementation VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable optimizations for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Find CRoaring library
find_package(roaring REQUIRED)

# Set include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Define source files
set(SOURCES
    roaring_bitmap.cpp
    bsi.cpp
    bsi_arithmetic.cpp
)

# Define header files
set(HEADERS
    roaring_bitmap.h
    bsi.h
    bsi_arithmetic.h
)

# Create library target
add_library(bsi_lib ${SOURCES} ${HEADERS})
target_link_libraries(bsi_lib PRIVATE roaring::roaring)

# Add SIMD optimization flags if available
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
if(COMPILER_SUPPORTS_AVX2)
    target_compile_options(bsi_lib PRIVATE -mavx2)
endif()

# Create an example executable
add_executable(bsi_example examples/bsi_example.cpp)
target_link_libraries(bsi_example PRIVATE bsi_lib)

# Build tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install targets
install(TARGETS bsi_lib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADERS} DESTINATION include/bsi)