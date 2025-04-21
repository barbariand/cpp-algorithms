
#ifndef CINDY_TESTING_FRAMEWORK_SORT
#define CINDY_TESTING_FRAMEWORK_SORT
#include "./testing_framework.h"
#include <iostream>
#include <vector>
namespace testing {
struct TestOptions {
  bool verbose = false;
  bool estimate_based_on_comparisons = true;
  std::vector<int> sizes = {10, 50, 100, 200, 500, 1000, 2000, 5000, 10000};
};
struct AlgorithmTestConfig {
  std::string name;
  void (*function_pointer)(Testing[], int);
  TestOptions options = {};
};
int test_all_algorithms(const std::vector<AlgorithmTestConfig>& algorithms_to_test);
int testing_sort_func(std::string name, void (*funcptr)(Testing[], int),
                      TestOptions options = {});
} // namespace testing
#endif
