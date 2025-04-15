cmake_minimum_required(VERSION 3.10)
project(BSI VERSION 0.1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找CRoaring库
find_library(ROARING_LIB roaring)
find_path(ROARING_INCLUDE "roaring/roaring.h")

if(NOT ROARING_LIB OR NOT ROARING_INCLUDE)
    message(FATAL_ERROR "未找到CRoaring库，请先安装。")
endif()

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${ROARING_INCLUDE})

# 源文件
set(BSI_SOURCES
    src/bsi.cpp
    src/roaring_bitmap_wrapper.cpp
)

# 创建BSI库
add_library(bsi STATIC ${BSI_SOURCES})
target_link_libraries(bsi PRIVATE ${ROARING_LIB})