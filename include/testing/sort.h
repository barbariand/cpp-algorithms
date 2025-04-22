#ifndef CINDY_TESTING_FRAMEWORK_SORT
#define CINDY_TESTING_FRAMEWORK_SORT
#include "./framework.h"
#include "complexity.h"
#include <iostream>
#include <optional>
#include <vector>
namespace testing {
using testing_complexity::Complexity;
struct TestOptions {
  std::vector<int> sizes = {10, 50, 100, /*200, 500, 1000, 2000, 5000, 10000*/};
  bool verbose = false; // Default: Only log details on failure
                        // If true: Log details even on success

  // --- Complexity Analysis Options ---
  bool estimate_based_on_comparisons = true; // Metric for estimation
  std::optional<Complexity> expected_best_complexity =
      std::nullopt; // Optional expected best case
  std::optional<Complexity> expected_worst_complexity =
      std::nullopt; // Optional expected worst case
};
struct AlgorithmTestConfig {
  std::string name;
  const SortingFunction &funcptr;
  TestOptions options = {};
};

using SortingFunction = std::function<void, Testing[], int>;
enum class AlgorithmRunStatus {
  PASSED, // All verification checks passed, complexity matches if checked
  PASSED_COMPLEXITY_WARN, // Verification passed, but complexity estimate
                          // mismatched expectation
  FAILED_VERIFICATION     // One or more test cases failed is_sorted check
};
int test_all_algorithms(
    const std::vector<AlgorithmTestConfig> &algorithms_to_test);
AlgorithmRunStatus testing_sort_func(std::string name,
                                     const SortingFunction &funcptr,
                                     const TestOptions &options = {});
} // namespace testing
#endif
