#include "bsi_arithmetic.h"
#include <algorithm>

namespace bsi_arithmetic {

BSI add(const BSI& a, const BSI& b) {
    // Result may need one more bit slice for carry
    size_t max_bit_slices = std::max(a.getNumBitSlices(), b.getNumBitSlices()) + 1;
    BSI result(max_bit_slices);
    
    // Initialize carry bitmap
    RoaringBitmap carry;
    
    // Process bit slices from LSB to MSB
    for (size_t i = 0; i < max_bit_slices - 1; i++) {
        // Get current bit slices or empty if out of range
        RoaringBitmap a_slice = (i < a.getNumBitSlices()) ? a.getBitSlice(i) : RoaringBitmap();
        RoaringBitmap b_slice = (i < b.getNumBitSlices()) ? b.getBitSlice(i) : RoaringBitmap();
        
        // Current bit = a_i XOR b_i XOR carry
        RoaringBitmap result_bit = (a_slice ^ b_slice) ^ carry;
        result.getBitSlice(i) = result_bit;
        
        // New carry = (a_i AND b_i) OR ((a_i XOR b_i) AND carry)
        carry = (a_slice & b_slice) | ((a_slice ^ b_slice) & carry);
    }
    
    // Set the highest bit slice (carry)
    result.getBitSlice(max_bit_slices - 1) = carry;
    
    return result;
}

BSI subtract(const BSI& a, const BSI& b) {
    size_t max_bit_slices = std::max(a.getNumBitSlices(), b.getNumBitSlices());
    BSI result(max_bit_slices);
    
    // Initialize borrow bitmap
    RoaringBitmap borrow;
    
    // Create universe bitmap (all positions with value in a or b)
    RoaringBitmap universe;
    for (size_t j = 0; j < a.getNumBitSlices(); j++) {
        universe |= a.getBitSlice(j);
    }
    for (size_t j = 0; j < b.getNumBitSlices(); j++) {
        universe |= b.getBitSlice(j);
    }
    
    // Process bit slices from LSB to MSB
    for (size_t i = 0; i < max_bit_slices; i++) {
        RoaringBitmap a_slice = (i < a.getNumBitSlices()) ? a.getBitSlice(i) : RoaringBitmap();
        RoaringBitmap b_slice = (i < b.getNumBitSlices()) ? b.getBitSlice(i) : RoaringBitmap();
        
        // Current bit = a_i XOR b_i XOR borrow
        RoaringBitmap result_bit = (a_slice ^ b_slice) ^ borrow;
        result.getBitSlice(i) = result_bit;
        
        // Calculate NOT operations using universe
        RoaringBitmap not_a_slice = universe - a_slice;
        RoaringBitmap not_a_xor_b = universe - (a_slice ^ b_slice);
        
        // New borrow = (~a_i AND b_i) OR (~(a_i XOR b_i) AND borrow)
        borrow = (not_a_slice & b_slice) | (not_a_xor_b & borrow);
    }
    
    return result;
}

BSI multiply(const BSI& a, const BSI& b) {
    // General multiplication with O(s1*s2) complexity
    size_t max_bit_slices = a.getNumBitSlices() + b.getNumBitSlices();
    BSI result(max_bit_slices);
    
    // For each bit in b, create partial product and shift
    for (size_t j = 0; j < b.getNumBitSlices(); j++) {
        RoaringBitmap b_j = b.getBitSlice(j);
        
        if (b_j.isEmpty()) {
            continue;
        }
        
        // Create partial result for this bit position
        BSI partial_result(a.getNumBitSlices() + 1);
        
        for (size_t i = 0; i < a.getNumBitSlices(); i++) {
            RoaringBitmap a_i = a.getBitSlice(i);
            
            if (a_i.isEmpty()) {
                continue;
            }
            
            // Partial result bit = a_i AND b_j
            partial_result.getBitSlice(i) = a_i & b_j;
        }
        
        // Shift partial result by j bits
        BSI shifted_partial_result(max_bit_slices);
        for (size_t i = 0; i < partial_result.getNumBitSlices(); i++) {
            if (i + j < max_bit_slices) {
                shifted_partial_result.getBitSlice(i + j) = partial_result.getBitSlice(i);
            }
        }
        
        // Add to final result
        result = add(result, shifted_partial_result);
    }
    
    return result;
}

BSI multiply(const BSI& a, const RoaringBitmap& mask) {
    // Specialized multiplication for binary mask with O(s) complexity
    BSI result(a.getNumBitSlices());
    
    for (size_t i = 0; i < a.getNumBitSlices(); i++) {
        RoaringBitmap a_i = a.getBitSlice(i);
        
        if (a_i.isEmpty()) {
            continue;
        }
        
        // Result bit = a_i AND mask
        result.getBitSlice(i) = a_i & mask;
    }
    
    return result;
}

RoaringBitmap less_than(const BSI& a, const BSI& b) {
    // Implementation of Algorithm 1 from the paper
    RoaringBitmap result;
    
    size_t max_bit_slices = std::max(a.getNumBitSlices(), b.getNumBitSlices());
    
    // Process from MSB to LSB
    for (int i = max_bit_slices - 1; i >= 0; i--) {
        RoaringBitmap a_i = (i < a.getNumBitSlices()) ? a.getBitSlice(i) : RoaringBitmap();
        RoaringBitmap b_i = (i < b.getNumBitSlices()) ? b.getBitSlice(i) : RoaringBitmap();
        
        // result = ((b_i OR result) ANDNOT a_i) OR (b_i AND result)
        result = ((b_i | result) - a_i) | (b_i & result);
    }
    
    return result;
}

RoaringBitmap equals(const BSI& a, const BSI& b) {
    // Implementation of Algorithm 2 from the paper
    
    // Find positions with non-zero values in a and b
    RoaringBitmap a_nonzero;
    for (size_t i = 0; i < a.getNumBitSlices(); i++) {
        a_nonzero |= a.getBitSlice(i);
    }
    
    RoaringBitmap b_nonzero;
    for (size_t i = 0; i < b.getNumBitSlices(); i++) {
        b_nonzero |= b.getBitSlice(i);
    }
    
    // Start with all positions in a
    RoaringBitmap result = a_nonzero;
    
    size_t max_bit_slices = std::max(a.getNumBitSlices(), b.getNumBitSlices());
    
    // Filter out positions where bits differ
    for (size_t i = 0; i < max_bit_slices; i++) {
        RoaringBitmap a_i = (i < a.getNumBitSlices()) ? a.getBitSlice(i) : RoaringBitmap();
        RoaringBitmap b_i = (i < b.getNumBitSlices()) ? b.getBitSlice(i) : RoaringBitmap();
        
        // Remove positions where bits differ
        result -= (a_i ^ b_i);
    }
    
    // Keep only positions with values in both a and b
    result &= b_nonzero;
    
    return result;
}

RoaringBitmap not_equals(const BSI& a, const BSI& b) {
    // Implementation of Algorithm 3 from the paper
    
    // Find positions with non-zero values in a and b
    RoaringBitmap a_nonzero;
    for (size_t i = 0; i < a.getNumBitSlices(); i++) {
        a_nonzero |= a.getBitSlice(i);
    }
    
    RoaringBitmap b_nonzero;
    for (size_t i = 0; i < b.getNumBitSlices(); i++) {
        b_nonzero |= b.getBitSlice(i);
    }
    
    // Start with empty result
    RoaringBitmap result;
    
    size_t max_bit_slices = std::max(a.getNumBitSlices(), b.getNumBitSlices());
    
    // Collect positions where bits differ
    for (size_t i = 0; i < max_bit_slices; i++) {
        RoaringBitmap a_i = (i < a.getNumBitSlices()) ? a.getBitSlice(i) : RoaringBitmap();
        RoaringBitmap b_i = (i < b.getNumBitSlices()) ? b.getBitSlice(i) : RoaringBitmap();
        
        // Add positions where bits differ
        result |= (a_i ^ b_i);
    }
    
    // Keep only positions with values in both a and b
    result &= a_nonzero;
    result &= b_nonzero;
    
    return result;
}

RoaringBitmap greater_than(const BSI& a, const BSI& b) {
    // a > b is equivalent to b < a
    return less_than(b, a);
}

RoaringBitmap less_than_or_equals(const BSI& a, const BSI& b) {
    RoaringBitmap lt = less_than(a, b);
    RoaringBitmap eq = equals(a, b);
    return lt | eq;
}

RoaringBitmap greater_than_or_equals(const BSI& a, const BSI& b) {
    RoaringBitmap gt = greater_than(a, b);
    RoaringBitmap eq = equals(a, b);
    return gt | eq;
}

BSI sum_bsi(const BSI& a, const BSI& b) {
    // Element-wise sum is the same as regular addition
    return add(a, b);
}

BSI max_bsi(const BSI& a, const BSI& b) {
    // Find positions where a > b and a <= b
    RoaringBitmap a_gt_b = greater_than(a, b);
    RoaringBitmap a_lte_b = less_than_or_equals(a, b);
    
    BSI result(std::max(a.getNumBitSlices(), b.getNumBitSlices()));
    
    // For positions where a > b, use a's values
    BSI a_part = multiply(a, a_gt_b);
    
    // For positions where a <= b, use b's values
    BSI b_part = multiply(b, a_lte_b);
    
    // Combine both parts
    result = add(a_part, b_part);
    
    return result;
}

BSI mul_bsi(const BSI& a, const BSI& b) {
    // Element-wise multiplication
    return multiply(a, b);
}

RoaringBitmap distinct_pos(const BSI& a, const BSI& b) {
    // Find positions with non-zero values in a or b
    RoaringBitmap a_nonzero;
    for (size_t i = 0; i < a.getNumBitSlices(); i++) {
        a_nonzero |= a.getBitSlice(i);
    }
    
    RoaringBitmap b_nonzero;
    for (size_t i = 0; i < b.getNumBitSlices(); i++) {
        b_nonzero |= b.getBitSlice(i);
    }
    
    // Return union of non-zero positions
    return a_nonzero | b_nonzero;
}

} // namespace bsi_arithmetic