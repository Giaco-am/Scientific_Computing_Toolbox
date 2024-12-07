#include "../include/ODE_Module/ODETester.hpp"


#include <iostream>
#include <vector>
#include <algorithm>

using namespace ScientificToolbox;

int main() {
    ODETester tester;
    bool all_passed = true;

    // Test expression parser
    all_passed &= tester.test_expression_parser();
    
    // Test Forward Euler Solver
    all_passed &= tester.test_FESolver();

    if (all_passed) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cerr << "Some tests failed!" << std::endl;
    }

    return !all_passed;  // Return 0 if all tests pass
}