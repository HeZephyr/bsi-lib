#ifndef BSI_H
#define BSI_H

#include "roaring_bitmap.h"
#include <vector>
#include <cstdint>
#include <memory>
#include <string>

/**
 * Bit-Sliced Index (BSI) class for efficient storage and operations on integer collections.
 * BSI represents integers as a series of bitmaps, where each bitmap corresponds to a bit position.
 * Particularly efficient for data following Pareto distribution patterns.
 */
class BSI {
public:
    // Constructors and assignment operators
    BSI();
    explicit BSI(size_t num_bit_slices);
    BSI(const BSI& other);
    BSI(BSI&& other) noexcept;
    BSI& operator=(const BSI& other);
    BSI& operator=(BSI&& other) noexcept;

    // Core BSI operations
    size_t getNumBitSlices() const;
    void resize(size_t num_bit_slices);
    void setValue(uint32_t position, uint32_t value);
    uint32_t getValue(uint32_t position) const;
    bool isZero(uint32_t position) const;
    
    // Aggregate operations
    uint32_t getMaxValue() const;
    uint64_t sum() const;
    uint64_t count() const;
    
    // Bit slice access
    const RoaringBitmap& getBitSlice(size_t bit_position) const;
    RoaringBitmap& getBitSlice(size_t bit_position);
    
    // Static factory methods
    static BSI fromValues(const std::vector<uint32_t>& values);
    std::vector<std::pair<uint32_t, uint32_t>> toValues() const;

private:
    // Storage for bit slices
    std::vector<RoaringBitmap> bit_slices_;
    
    // Ensures BSI has enough bit slices to represent the given value
    void ensureCapacity(uint32_t value);
};

#endif // BSI_H