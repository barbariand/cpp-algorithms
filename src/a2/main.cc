#include "a2.h"
#include <iostream>
#include <include/testing>
using std::string;
using testing::AlgorithmTestConfig;
using testing_utils::print_colored_line;
using testing::test_all_algorithms;
using testing_framework::Testing;
int main() {
  std::vector<AlgorithmTestConfig> algorithms = {
      {"mergesortBook", MergesortBook<Testing>},
      {"mergesort", Mergesort<Testing>},
      {"heapsort", Heapsort<Testing>},
      {"quicksortLomuto", QuicksortLomuto<Testing>},
      {"quicksortHoareimpproved", QuicksortHoareImproved<Testing>},
      {"quicksortHoaremedian3", QuicksortHoareImprovedMedian3<Testing>},
      {"quicksortHoare", QuicksortHoare<Testing>},
  };

  // Run the tests for all configured algorithms
  int final_status = test_all_algorithms(algorithms);

  // Report final overall status based on the summary function's return
  std::cout << "\nOverall Test Suite Result: ";
  if (final_status == 0) {
    print_colored_line("ALL PASSED", testing_utils::BOLD_GREEN);
  } else {
    print_colored_line("SOME FAILURES", testing_utils::BOLD_RED);
  }

  return final_status; // Return 0 if all passed, 1 if any failed
}
