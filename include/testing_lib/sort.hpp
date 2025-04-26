#ifndef CINDY_TESTING_FRAMEWORK_SORT
#define CINDY_TESTING_FRAMEWORK_SORT
#include "prelude.hpp"
#include "framework.hpp"
#include <functional>
#include <map>
#include <optional>
#include <vector>
namespace testing {
using testing_framework::Complexity;
using testing_framework::ControlStatsSnapshot;
using testing_framework::Testing;
using testing_framework::TestOptions;
PRELUDE;
struct AlgorithmTestConfig {
  std::string name;
  const SortingFunction &funcptr;
  TestOptions options = {};
};

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
