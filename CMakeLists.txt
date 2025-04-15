# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(bsi)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Roaring Bitmap library
find_path(ROARING_INCLUDE_DIR roaring/roaring.h)
find_library(ROARING_LIBRARY roaring)

if(NOT ROARING_INCLUDE_DIR OR NOT ROARING_LIBRARY)
    message(FATAL_ERROR "Roaring Bitmap library not found. Please install it first.")
endif()

# Add include directories
include_directories(${ROARING_INCLUDE_DIR} include)

# Add library
add_library(bsi
    src/roaring_bitmap.cpp
    src/bsi.cpp
    src/bsi_arithmetic.cpp
)

# Link with the Roaring Bitmap library
target_link_libraries(bsi ${ROARING_LIBRARY})

# Add examples
add_subdirectory(examples)

# Simple installation rules
install(TARGETS bsi
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${HEADERS} DESTINATION include/bsi)