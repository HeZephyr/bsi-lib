#include "bsi.h"
#include "bsi_arithmetic.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

// Utility function to print bitmap contents
void print_bitmap(const RoaringBitmap& bitmap, const std::string& name) {
    std::cout << name << ": {";
    bool first = true;
    for (uint32_t value : bitmap) {
        if (!first) std::cout << ", ";
        std::cout << value;
        first = false;
    }
    std::cout << "}" << std::endl;
}

// Utility function to print BSI as a table similar to the paper's figures
void print_bsi_table(const BSI& bsi, const std::string& name, size_t max_positions = 8) {
    std::cout << "BSI " << name << " representation:" << std::endl;
    
    // Print position header
    std::cout << std::setw(10) << "Position" << " | ";
    for (size_t i = 0; i < max_positions; i++) {
        std::cout << std::setw(3) << i << " ";
    }
    std::cout << std::endl;
    
    // Print separator
    std::cout << std::string(10, '-') << "-|-" << std::string(4 * max_positions, '-') << std::endl;
    
    // Print decimal values
    std::cout << std::setw(10) << "Decimal" << " | ";
    for (size_t i = 0; i < max_positions; i++) {
        std::cout << std::setw(3) << bsi.getValue(i) << " ";
    }
    std::cout << std::endl;
    
    // Print binary representation
    std::cout << std::setw(10) << "Binary" << " | ";
    for (size_t i = 0; i < max_positions; i++) {
        uint32_t value = bsi.getValue(i);
        std::string binary = "";
        
        // If value is 0, print 0
        if (value == 0) {
            binary = "0";
        } else {
            // Convert to binary
            while (value > 0) {
                binary = (value % 2 ? "1" : "0") + binary;
                value /= 2;
            }
        }
        
        std::cout << std::setw(3) << binary << " ";
    }
    std::cout << std::endl;
    
    // Print bit slices
    for (size_t slice = 0; slice < bsi.getNumBitSlices(); slice++) {
        std::cout << std::setw(10) << "Bit slice " + std::to_string(slice) << " | ";
        for (size_t i = 0; i < max_positions; i++) {
            uint32_t value = bsi.getValue(i);
            bool bit = (value >> slice) & 1;
            std::cout << std::setw(3) << (bit ? "1" : "0") << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Recreate Figure 1 from the paper
void demo_bsi_representation() {
    std::cout << "===== BSI Representation (Figure 1) =====" << std::endl;
    
    // Create values from Figure 1
    std::vector<uint32_t> values = {4, 34, 213, 57, 0, 76, 127, 55};
    
    // Create BSI from values
    BSI bsi = BSI::fromValues(values);
    
    // Print BSI table
    print_bsi_table(bsi, "from Figure 1");
}

// Recreate Figure 2 (Addition) from the paper
void demo_bsi_addition() {
    std::cout << "\n===== BSI Addition (Figure 2) =====" << std::endl;
    
    // Create values from Figure 2
    std::vector<uint32_t> values_a = {2, 4, 2, 5};
    std::vector<uint32_t> values_b = {1, 3, 0, 1};
    
    // Create BSIs
    BSI bsi_a = BSI::fromValues(values_a);
    BSI bsi_b = BSI::fromValues(values_b);
    
    // Print BSI tables
    print_bsi_table(bsi_a, "A", 4);
    print_bsi_table(bsi_b, "B", 4);
    
    // Perform addition
    BSI result = bsi_arithmetic::add(bsi_a, bsi_b);
    
    // Print result
    print_bsi_table(result, "A + B (Result)", 4);
    
    // Verify results
    std::vector<uint32_t> expected_result = {3, 7, 2, 6};
    BSI expected = BSI::fromValues(expected_result);
    
    bool correct = true;
    for (size_t i = 0; i < 4; i++) {
        if (result.getValue(i) != expected.getValue(i)) {
            correct = false;
            break;
        }
    }
    
    std::cout << "Addition verification: " << (correct ? "CORRECT" : "INCORRECT") << std::endl;
}

// Demonstrate BSI comparison operations (Algorithm 1, 2, 3)
void demo_bsi_comparison() {
    std::cout << "\n===== BSI Comparison Operations (Algorithms 1, 2, 3) =====" << std::endl;
    
    // Create test values
    std::vector<uint32_t> values_a = {10, 20, 15, 25, 30};
    std::vector<uint32_t> values_b = {5, 20, 25, 15, 40};
    
    // Create BSIs
    BSI bsi_a = BSI::fromValues(values_a);
    BSI bsi_b = BSI::fromValues(values_b);
    
    // Print BSI tables
    print_bsi_table(bsi_a, "A", 5);
    print_bsi_table(bsi_b, "B", 5);
    
    // Apply Algorithm 1: Less Than
    std::cout << "Algorithm 1: Less Than (A < B)" << std::endl;
    RoaringBitmap lt_result = bsi_arithmetic::less_than(bsi_a, bsi_b);
    print_bitmap(lt_result, "A < B");
    
    // Expected positions where A < B: 2, 4
    RoaringBitmap lt_expected;
    lt_expected.add(2);
    lt_expected.add(4);
    std::cout << "Expected: {2, 4}" << std::endl;
    
    // Apply Algorithm 2: Equals
    std::cout << "\nAlgorithm 2: Equals (A = B)" << std::endl;
    RoaringBitmap eq_result = bsi_arithmetic::equals(bsi_a, bsi_b);
    print_bitmap(eq_result, "A = B");
    
    // Expected positions where A = B: 1
    RoaringBitmap eq_expected;
    eq_expected.add(1);
    std::cout << "Expected: {1}" << std::endl;
    
    // Apply Algorithm 3: Not Equals
    std::cout << "\nAlgorithm 3: Not Equals (A ≠ B)" << std::endl;
    RoaringBitmap ne_result = bsi_arithmetic::not_equals(bsi_a, bsi_b);
    print_bitmap(ne_result, "A ≠ B");
    
    // Expected positions where A ≠ B: 0, 2, 3, 4
    RoaringBitmap ne_expected;
    ne_expected.add(0);
    ne_expected.add(2);
    ne_expected.add(3);
    ne_expected.add(4);
    std::cout << "Expected: {0, 2, 3, 4}" << std::endl;
}

// Demonstrate metric computation scenarios from the paper
void demo_metric_computation() {
    std::cout << "\n===== Metric Computation Scenarios =====" << std::endl;
    
    // Simulate metric calculation using BSI operations
    
    // 1. Create simulated expose data for an experiment
    std::cout << "Step 1: Simulating experiment expose data" << std::endl;
    
    BSI strategy_a(3); // First strategy
    strategy_a.setValue(0, 1); // user 0 exposed to strategy A
    strategy_a.setValue(1, 1); // user 1 exposed to strategy A
    strategy_a.setValue(2, 1); // user 2 exposed to strategy A
    strategy_a.setValue(3, 1); // user 3 exposed to strategy A
    
    BSI strategy_b(3); // Second strategy
    strategy_b.setValue(4, 1); // user 4 exposed to strategy B
    strategy_b.setValue(5, 1); // user 5 exposed to strategy B
    strategy_b.setValue(6, 1); // user 6 exposed to strategy B
    strategy_b.setValue(7, 1); // user 7 exposed to strategy B
    
    print_bsi_table(strategy_a, "Strategy A Exposure", 8);
    print_bsi_table(strategy_b, "Strategy B Exposure", 8);
    
    // 2. Create simulated metric data
    std::cout << "Step 2: Simulating user metric data" << std::endl;
    
    BSI user_metric(8);
    user_metric.setValue(0, 10); // user 0 has metric value 10
    user_metric.setValue(1, 12); // user 1 has metric value 12
    user_metric.setValue(2, 15); // user 2 has metric value 15
    user_metric.setValue(3, 11); // user 3 has metric value 11
    user_metric.setValue(4, 20); // user 4 has metric value 20
    user_metric.setValue(5, 22); // user 5 has metric value 22
    user_metric.setValue(6, 18); // user 6 has metric value 18
    user_metric.setValue(7, 23); // user 7 has metric value 23
    
    print_bsi_table(user_metric, "User Metric Values", 8);
    
    // 3. Filter metric by strategy using multiply operation
    std::cout << "Step 3: Filtering metrics by strategy" << std::endl;
    
    // Get bitmap of positions for each strategy
    RoaringBitmap strategy_a_positions;
    RoaringBitmap strategy_b_positions;
    
    for (size_t i = 0; i < 8; i++) {
        if (strategy_a.getValue(i) > 0) strategy_a_positions.add(i);
        if (strategy_b.getValue(i) > 0) strategy_b_positions.add(i);
    }
    
    // Filter metrics by strategy positions
    BSI strategy_a_metrics = bsi_arithmetic::multiply(user_metric, strategy_a_positions);
    BSI strategy_b_metrics = bsi_arithmetic::multiply(user_metric, strategy_b_positions);
    
    print_bsi_table(strategy_a_metrics, "Strategy A Metrics", 8);
    print_bsi_table(strategy_b_metrics, "Strategy B Metrics", 8);
    
    // 4. Compute aggregates for each strategy
    std::cout << "Step 4: Computing aggregate metrics per strategy" << std::endl;
    
    uint64_t strategy_a_sum = strategy_a_metrics.sum();
    uint64_t strategy_a_count = strategy_a_metrics.count();
    double strategy_a_avg = strategy_a_count > 0 ? static_cast<double>(strategy_a_sum) / strategy_a_count : 0;
    
    uint64_t strategy_b_sum = strategy_b_metrics.sum();
    uint64_t strategy_b_count = strategy_b_metrics.count();
    double strategy_b_avg = strategy_b_count > 0 ? static_cast<double>(strategy_b_sum) / strategy_b_count : 0;
    
    std::cout << "Strategy A:" << std::endl;
    std::cout << "  Sum: " << strategy_a_sum << std::endl;
    std::cout << "  Count: " << strategy_a_count << std::endl;
    std::cout << "  Average: " << strategy_a_avg << std::endl;
    
    std::cout << "Strategy B:" << std::endl;
    std::cout << "  Sum: " << strategy_b_sum << std::endl;
    std::cout << "  Count: " << strategy_b_count << std::endl;
    std::cout << "  Average: " << strategy_b_avg << std::endl;
    
    // 5. Demonstrate dimension filtering (e.g., by client type)
    std::cout << "\nStep 5: Dimension filtering example (by client type)" << std::endl;
    
    // Create a dimension for client type
    BSI client_type(3);
    client_type.setValue(0, 1); // user 0 has client type 1
    client_type.setValue(1, 1); // user 1 has client type 1
    client_type.setValue(4, 1); // user 4 has client type 1
    client_type.setValue(5, 1); // user 5 has client type 1
    client_type.setValue(2, 2); // user 2 has client type 2
    client_type.setValue(3, 2); // user 3 has client type 2
    client_type.setValue(6, 2); // user 6 has client type 2
    client_type.setValue(7, 2); // user 7 has client type 2
    
    print_bsi_table(client_type, "Client Type", 8);
    
    // Create a filter for client type 1
    RoaringBitmap client_type_1_filter;
    for (size_t i = 0; i < 8; i++) {
        if (client_type.getValue(i) == 1) client_type_1_filter.add(i);
    }
    
    // Apply dimension filter to strategy metrics
    BSI strategy_a_type1_metrics = bsi_arithmetic::multiply(strategy_a_metrics, client_type_1_filter);
    BSI strategy_b_type1_metrics = bsi_arithmetic::multiply(strategy_b_metrics, client_type_1_filter);
    
    print_bsi_table(strategy_a_type1_metrics, "Strategy A, Client Type 1", 8);
    print_bsi_table(strategy_b_type1_metrics, "Strategy B, Client Type 1", 8);
    
    // Compute filtered aggregates
    uint64_t strategy_a_type1_sum = strategy_a_type1_metrics.sum();
    uint64_t strategy_a_type1_count = strategy_a_type1_metrics.count();
    double strategy_a_type1_avg = strategy_a_type1_count > 0 ? 
                               static_cast<double>(strategy_a_type1_sum) / strategy_a_type1_count : 0;
    
    uint64_t strategy_b_type1_sum = strategy_b_type1_metrics.sum();
    uint64_t strategy_b_type1_count = strategy_b_type1_metrics.count();
    double strategy_b_type1_avg = strategy_b_type1_count > 0 ? 
                               static_cast<double>(strategy_b_type1_sum) / strategy_b_type1_count : 0;
    
    std::cout << "Strategy A (Client Type 1):" << std::endl;
    std::cout << "  Sum: " << strategy_a_type1_sum << std::endl;
    std::cout << "  Count: " << strategy_a_type1_count << std::endl;
    std::cout << "  Average: " << strategy_a_type1_avg << std::endl;
    
    std::cout << "Strategy B (Client Type 1):" << std::endl;
    std::cout << "  Sum: " << strategy_b_type1_sum << std::endl;
    std::cout << "  Count: " << strategy_b_type1_count << std::endl;
    std::cout << "  Average: " << strategy_b_type1_avg << std::endl;
}

int main() {
    // Demonstrate BSI representation (Figure 1)
    demo_bsi_representation();
    
    // Demonstrate BSI addition (Figure 2)
    demo_bsi_addition();
    
    // Demonstrate BSI comparison operations
    demo_bsi_comparison();
    
    // Demonstrate metric computation scenarios
    demo_metric_computation();
    
    return 0;
}