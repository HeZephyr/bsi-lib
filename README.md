# BSI Implementation

A C++ library implementing Bit-Sliced Index (BSI) operations using the CRoaring library, with examples demonstrating their application in metric computation contexts.

## Overview

This implementation provides a C++ interface for Bit-Sliced Index (BSI) operations, built on top of the CRoaring library for efficient bitmap manipulation. BSI is a technique that represents integer values as a series of bitmaps, where each bitmap corresponds to a bit position in the binary representation of values.

This library implements core BSI operations and provides examples demonstrating how these operations can be applied to metric computation scenarios such as those described in the paper "Large-Scale Metric Computation in Online Controlled Experiment Platform" by Xiong and Wang.

## Features

- Efficient BSI implementation using CRoaring as the underlying bitmap engine
- Core BSI operations: addition, subtraction, multiplication
- Comparison operations (<, =, ≠, >, ≤, ≥)
- Aggregate functions (sum, max, count)
- Example applications demonstrating practical use cases

## Requirements

- C++17 compatible compiler
- [CRoaring](https://github.com/RoaringBitmap/CRoaring) library
- CMake 3.10 or higher (for building)

## Installation

### 1. Install CRoaring

The CRoaring library is required for this implementation:

```bash
# For macOS with Homebrew
brew install roaring

# For Ubuntu/Debian
sudo apt-get install libroaring-dev

# Or build from source
git clone https://github.com/RoaringBitmap/CRoaring.git
cd CRoaring
mkdir build && cd build
cmake ..
make
sudo make install
```

### 2. Build and Install BSI

```bash
git clone https://github.com/HeZephyr/bsi-lib.git
cd bsi-lib
mkdir build && cd build
cmake ..
make
sudo make install
```

## Usage

Basic usage example:

```cpp
#include <bsi/bsi.h>
#include <bsi/bsi_arithmetic.h>
#include <iostream>
#include <vector>

int main() {
    // Create BSI from a vector of values
    std::vector<uint32_t> values = {4, 34, 213, 57, 0, 76, 127, 55};
    BSI bsi = BSI::fromValues(values);
    
    // Create another BSI
    BSI bsi2(8);
    bsi2.setValue(0, 10);
    bsi2.setValue(1, 20);
    
    // Perform arithmetic operations
    BSI sum_result = bsi_arithmetic::add(bsi, bsi2);
    
    // Print the result sum
    std::cout << "Sum: " << sum_result.sum() << std::endl;
    
    return 0;
}
```

For a more comprehensive example demonstrating BSI operations, see [paper_example.cpp](examples/paper_example.cpp).

## Building Your Project with BSI

### Using CMake

```cmake
cmake_minimum_required(VERSION 3.10)
project(my_project)

find_path(ROARING_INCLUDE_DIR roaring/roaring.h)
find_library(ROARING_LIBRARY roaring)

find_path(BSI_INCLUDE_DIR bsi/bsi.h)
find_library(BSI_LIBRARY bsi)

include_directories(${ROARING_INCLUDE_DIR} ${BSI_INCLUDE_DIR})

add_executable(my_app main.cpp)
target_link_libraries(my_app ${BSI_LIBRARY} ${ROARING_LIBRARY})
```

## Acknowledgments

This implementation uses the CRoaring library developed by the Roaring Bitmap team. The example application demonstrates concepts from the paper by Xiong and Wang, though the core BSI concepts have been established in earlier literature.

## References

- Lemire, D., Ssi-Yan-Kai, G., & Kaser, O. (2016). Consistently faster and smaller compressed bitmaps with Roaring. *Software: Practice and Experience, 46(11)*, 1547-1569.
- O'Neil, P., & Quass, D. (1997). Improved query performance with variant indexes. *Proceedings of the 1997 ACM SIGMOD International Conference on Management of Data*, 38-49.
- Xiong, T., & Wang, Y. (2024). Large-Scale Metric Computation in Online Controlled Experiment Platform. *Proceedings of the VLDB Endowment, 17(12)*, 4014-4024.

## License

This project is licensed under the MIT License - see the LICENSE file for details.