#ifndef BSI_ARITHMETIC_H
#define BSI_ARITHMETIC_H

#include "bsi.h"

/**
 * Namespace containing arithmetic and comparison operations for BSIs.
 * Implements algorithms from the paper "Large-Scale Metric Computation in Online Controlled Experiment Platform".
 */
namespace bsi_arithmetic {

// Basic arithmetic operations
BSI add(const BSI& a, const BSI& b);
BSI subtract(const BSI& a, const BSI& b);

// Multiplication operations
BSI multiply(const BSI& a, const BSI& b);                   // General case - O(s1*s2)
BSI multiply(const BSI& a, const RoaringBitmap& mask);      // Binary mask case - O(s)

// Comparison operations
RoaringBitmap less_than(const BSI& a, const BSI& b);            // Algorithm 1 from paper
RoaringBitmap equals(const BSI& a, const BSI& b);               // Algorithm 2 from paper
RoaringBitmap not_equals(const BSI& a, const BSI& b);           // Algorithm 3 from paper
RoaringBitmap greater_than(const BSI& a, const BSI& b);
RoaringBitmap less_than_or_equals(const BSI& a, const BSI& b);
RoaringBitmap greater_than_or_equals(const BSI& a, const BSI& b);

// Aggregate operations (element-wise across BSIs)
BSI sum_bsi(const BSI& a, const BSI& b);                // Element-wise sum
BSI max_bsi(const BSI& a, const BSI& b);                // Element-wise maximum
BSI mul_bsi(const BSI& a, const BSI& b);                // Element-wise multiplication
RoaringBitmap distinct_pos(const BSI& a, const BSI& b); // Positions with non-zero values

} // namespace bsi_arithmetic

#endif // BSI_ARITHMETIC_H