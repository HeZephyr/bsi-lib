#include "bsi.h"
#include <algorithm>
#include <stdexcept>

BSI::BSI() {}

BSI::BSI(size_t num_bit_slices) : bit_slices_(num_bit_slices) {}

BSI::BSI(const BSI& other) : bit_slices_(other.bit_slices_) {}

BSI::BSI(BSI&& other) noexcept : bit_slices_(std::move(other.bit_slices_)) {}

BSI& BSI::operator=(const BSI& other) {
    if (this != &other) {
        bit_slices_ = other.bit_slices_;
    }
    return *this;
}

BSI& BSI::operator=(BSI&& other) noexcept {
    if (this != &other) {
        bit_slices_ = std::move(other.bit_slices_);
    }
    return *this;
}

size_t BSI::getNumBitSlices() const {
    return bit_slices_.size();
}

void BSI::resize(size_t num_bit_slices) {
    bit_slices_.resize(num_bit_slices);
}

void BSI::setValue(uint32_t position, uint32_t value) {
    // For zero values, remove position from all bit slices
    if (value == 0) {
        for (auto& slice : bit_slices_) {
            slice.remove(position);
        }
        return;
    }

    // Ensure capacity for non-zero values
    ensureCapacity(value);

    // Set bits according to value's binary representation
    for (size_t i = 0; i < bit_slices_.size(); i++) {
        if ((value >> i) & 1) {
            bit_slices_[i].add(position);
        } else {
            bit_slices_[i].remove(position);
        }
    }
}

uint32_t BSI::getValue(uint32_t position) const {
    uint32_t value = 0;
    for (size_t i = 0; i < bit_slices_.size(); i++) {
        if (bit_slices_[i].contains(position)) {
            value |= (1 << i);
        }
    }
    return value;
}

bool BSI::isZero(uint32_t position) const {
    for (const auto& slice : bit_slices_) {
        if (slice.contains(position)) {
            return false;
        }
    }
    return true;
}

uint32_t BSI::getMaxValue() const {
    uint32_t max_value = 0;
    for (size_t i = 0; i < bit_slices_.size(); i++) {
        if (!bit_slices_[i].isEmpty()) {
            max_value |= (1 << i);
        }
    }
    return max_value;
}

uint64_t BSI::sum() const {
    uint64_t total = 0;
    for (size_t i = 0; i < bit_slices_.size(); i++) {
        total += (bit_slices_[i].cardinality() << i);
    }
    return total;
}

uint64_t BSI::count() const {
    if (bit_slices_.empty()) {
        return 0;
    }

    // Create bitmap of all positions with non-zero values
    RoaringBitmap non_zero;
    for (const auto& slice : bit_slices_) {
        non_zero |= slice;
    }
    return non_zero.cardinality();
}

const RoaringBitmap& BSI::getBitSlice(size_t bit_position) const {
    if (bit_position >= bit_slices_.size()) {
        throw std::out_of_range("Bit slice position out of range");
    }
    return bit_slices_[bit_position];
}

RoaringBitmap& BSI::getBitSlice(size_t bit_position) {
    if (bit_position >= bit_slices_.size()) {
        throw std::out_of_range("Bit slice position out of range");
    }
    return bit_slices_[bit_position];
}

BSI BSI::fromValues(const std::vector<uint32_t>& values) {
    if (values.empty()) {
        return BSI();
    }

    // Find maximum value to determine required bit slices
    uint32_t max_value = *std::max_element(values.begin(), values.end());
    size_t num_bit_slices = 0;
    while (max_value > 0) {
        max_value >>= 1;
        num_bit_slices++;
    }

    BSI bsi(num_bit_slices);
    for (size_t i = 0; i < values.size(); i++) {
        bsi.setValue(i, values[i]);
    }
    return bsi;
}

std::vector<std::pair<uint32_t, uint32_t>> BSI::toValues() const {
    std::vector<std::pair<uint32_t, uint32_t>> result;

    if (bit_slices_.empty()) {
        return result;
    }

    // Find all positions with non-zero values
    RoaringBitmap non_zero;
    for (const auto& slice : bit_slices_) {
        non_zero |= slice;
    }

    // Calculate value for each position
    for (uint32_t position : non_zero) {
        uint32_t value = getValue(position);
        result.emplace_back(position, value);
    }

    return result;
}

void BSI::ensureCapacity(uint32_t value) {
    size_t required_bit_slices = 0;
    uint32_t temp = value;
    while (temp > 0) {
        temp >>= 1;
        required_bit_slices++;
    }

    if (required_bit_slices > bit_slices_.size()) {
        bit_slices_.resize(required_bit_slices);
    }
}