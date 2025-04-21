#include "testing_sort.h"
#include "./utils.h"
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <tuple>
#include <vector>
namespace testing {

Testing *reversed(int size) {
  if (size <= 0)
    return nullptr;
  Testing *arr = new Testing[size];
  for (int i = 0; i < size; i++) {

    arr[i] = Testing(size - 1 - i);
  }
  return arr;
}

struct SortingResult {
  ControlStatsSnapshot snapshot;
  bool sorted;
  std::string test_case_name;
  int array_size;
};

using ArrayGenerator = std::function<Testing *(int)>;

Testing *sorted(int size) {
  if (size <= 0)
    return nullptr;
  Testing *arr = new Testing[size];
  for (int i = 0; i < size; i++) {
    arr[i] = Testing(i);
  }
  return arr;
}

Testing *random_unique(int size) {
  if (size <= 0)
    return nullptr;
  Testing *arr = new Testing[size];
  for (int i = 0; i < size; i++) {
    arr[i] = Testing(i);
  }

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(arr, arr + size, g);
  return arr;
}

Testing *few_unique(int size) {
  if (size <= 0)
    return nullptr;
  int num_unique = std::max(1, size / 10);
  if (num_unique <= 1 && size > 1)
    num_unique = 2;
  Testing *arr = new Testing[size];
  std::random_device rd;
  std::mt19937 g(rd());
  std::uniform_int_distribution<> distrib(0, num_unique - 1);
  for (int i = 0; i < size; i++) {
    arr[i] = Testing(distrib(g));
  }
  return arr;
}

Testing *nearly_sorted(int size) {
  if (size <= 0)
    return nullptr;
  Testing *arr = sorted(size);
  if (size < 2)
    return arr;

  int num_swaps = std::max(1, size / 20);
  std::random_device rd;
  std::mt19937 g(rd());
  std::uniform_int_distribution<> distrib(0, size - 1);

  for (int i = 0; i < num_swaps; ++i) {
    int idx1 = distrib(g);
    int idx2 = distrib(g);

    std::swap(arr[idx1], arr[idx2]);
  }
  return arr;
}
enum class Complexity { O1, ON, ONLogN, ON2, Undetermined, InsufficientData };

std::string complexity_to_string(Complexity c) {
  switch (c) {
  case Complexity::O1:
    return "O(1)";
  case Complexity::ON:
    return "O(N)";
  case Complexity::ONLogN:
    return "O(N log N)";
  case Complexity::ON2:
    return "O(N^2)";
  case Complexity::Undetermined:
    return "Undetermined";
  case Complexity::InsufficientData:
    return "Insufficient Data";
  default:
    std::cerr << "Warning: Unknown Complexity enum value!" << std::endl;
    return "Unknown";
  }
}
SortingResult test_sort_func_single_case(const ArrayGenerator &generator,
                                         int size, const std::string &test_name,
                                         void (*funcptr)(Testing[], int),
                                         bool verbose = true) {
  if (verbose) {
    print_colored_line("\n-- Running Case: " + test_name +
                           " (Size: " + std::to_string(size) + ") --",
                       BOLD_CYAN);
  }
  Testing *arr = generator(size);
  if (!arr) {
    if (size <= 0) {
      if (verbose) {
        print_colored_line("  Skipped (Size <= 0)", testing::YELLOW);
      }

      return SortingResult{Control::get_instance().reset_and_get_snapshot(),
                           true, test_name, size};
    } else {
      print_colored_line("  ERROR: Array generator returned nullptr!",
                         testing::BOLD_RED);
      return SortingResult{Control::get_instance().reset_and_get_snapshot(),
                           false, test_name, size};
    }
  }

  std::unique_ptr<Testing[]> arr_ptr(arr);

  if (verbose) {
    std::cout << "  Initial array: ";
    printArray(arr_ptr.get(), size);
  }

  Control::get_instance().reset_and_get_snapshot();

  funcptr(arr_ptr.get(), size);

  ControlStatsSnapshot stats = Control::get_instance().reset_and_get_snapshot();

  if (verbose) {
    std::cout << "  Sorted array:  ";
    printArray(arr_ptr.get(), size);
  }

  bool sorted_ok = std::is_sorted(arr_ptr.get(), arr_ptr.get() + size);

  if (verbose) {
    std::cout << "  Verification:  ";
    if (sorted_ok) {
      print_colored_line("Passed", testing::BOLD_GREEN);
    } else {
      print_colored_line("Failed", testing::BOLD_RED);
    }
    std::cout << "  Statistics:" << std::endl;
    stats.print();
  }

  return SortingResult{stats, sorted_ok, test_name, size};
}

Complexity estimate_complexity_from_pair(double n1, double ops1, double n2,
                                         double ops2) {
  if (n1 <= 0 || n2 <= 0 || ops1 < 0 || ops2 < 0 ||
      n2 <= n1) { // Allow ops == 0
    return Complexity::InsufficientData;
  }

  // Handle cases where operations are zero or constant
  if (ops1 == 0 && ops2 == 0)
    return Complexity::O1; // Consistently zero operations
  // If ops are constant and non-zero
  if (ops1 > 0 && ops1 == ops2)
    return Complexity::O1;
  // If ops1 is 0 but ops2 > 0, hard to estimate, needs more data
  if (ops1 == 0 && ops2 > 0)
    return Complexity::InsufficientData;
  // ops1 > 0 and ops2 = 0 is unusual unless N=0/1 handled, treat as
  // insufficient
  if (ops1 > 0 && ops2 == 0)
    return Complexity::InsufficientData;

  // Now ops1 > 0 and ops2 > 0
  double size_ratio = n2 / n1;
  double ops_ratio = ops2 / ops1;

  // Calculate expected ratios
  double expected_ratio_n = size_ratio;
  double expected_ratio_nlogn = 1.0; // Default avoids div by zero if n1=1
                                     // Use log base 2, handle n1=1 case
  if (n1 < 1.5) { // If n1 is 1 (or very close), N log N behaves like N
    expected_ratio_nlogn = expected_ratio_n;
  } else if (std::log2(n1) != 0) { // Check divisor is not zero
    expected_ratio_nlogn = size_ratio * (std::log2(n2) / std::log2(n1));
  } else {
    // Should not happen if n1 >= 1.5, but as safety
    expected_ratio_nlogn = std::numeric_limits<double>::max();
  }

  double expected_ratio_n2 = size_ratio * size_ratio;

  // Calculate relative differences
  auto calc_diff = [&](double expected) {
    if (expected <= std::numeric_limits<double>::epsilon())
      return std::numeric_limits<double>::max();
    return std::abs(ops_ratio - expected) / expected;
  };

  double diff_n = calc_diff(expected_ratio_n);
  double diff_nlogn = calc_diff(expected_ratio_nlogn);
  double diff_n2 = calc_diff(expected_ratio_n2);
  double diff_1 = std::abs(ops_ratio - 1.0); // Check for O(1)

  // Determine best fit
  const double tolerance = 0.35;               // Adjust as needed
  const double o1_tolerance = tolerance / 2.0; // O(1) should be closer

  // Find minimum difference
  double min_diff = std::min({diff_1, diff_n, diff_nlogn, diff_n2});

  // Check which complexity corresponds to the minimum difference within
  // tolerance
  if (min_diff == diff_1 && min_diff < o1_tolerance)
    return Complexity::O1;
  if (min_diff == diff_n && min_diff < tolerance)
    return Complexity::ON;
  // Handle NlogN vs N^2 ambiguity: prefer NlogN only if clearly better
  if (min_diff == diff_nlogn && min_diff < tolerance) {
    if (diff_nlogn < diff_n2 * 0.8 || diff_n2 > tolerance * 1.5) {
      return Complexity::ONLogN;
    } else {
      // If NLogN and N2 are both close, lean towards N2 unless NLogN is
      // significantly closer
      if (diff_n2 < tolerance)
        return Complexity::ON2; // If N2 fits, it's possible
      else
        return Complexity::ONLogN; // Otherwise stick with NLogN
    }
  }
  if (min_diff == diff_n2 && min_diff < tolerance)
    return Complexity::ON2;

  // If none fit well
  return Complexity::Undetermined;
}

Complexity estimate_complexity_for_type(
    const std::vector<std::pair<int, ControlStatsSnapshot>> &data,
    bool use_comparisons = true) {
  std::vector<std::pair<double, double>> valid_points;
  for (const auto &p : data) {
    if (p.first > 1) { // Use size > 1 for log/ratio stability
      double ops = use_comparisons
                       ? static_cast<double>(p.second.total_comparisons())
                       : static_cast<double>(p.second.total_data_moves());
      // We need points for ratio, even if ops is 0 for O(1) check in pair
      // estimator
      valid_points.push_back({static_cast<double>(p.first), ops});
    }
  }

  // Sort by size just in case (should already be sorted)
  std::sort(valid_points.begin(), valid_points.end());

  if (valid_points.size() < 2) {
    // Check for O(1) possibility even with few points
    bool all_zero_ops = true;
    bool const_ops = true;
    double first_ops = -1;

    for (const auto &p : valid_points) {
      if (p.second != 0)
        all_zero_ops = false;
      if (first_ops < 0)
        first_ops = p.second;
      if (p.second != first_ops)
        const_ops = false;
    }
    if (!valid_points.empty() &&
        (all_zero_ops || (const_ops && first_ops >= 0))) {
      return Complexity::O1;
    }

    return Complexity::InsufficientData;
  }

  // Use the two largest sizes for the estimation for asymptotic behavior
  const auto &p1 = valid_points[valid_points.size() - 2];
  const auto &p2 = valid_points[valid_points.size() - 1];

  return estimate_complexity_from_pair(p1.first, p1.second, p2.first,
                                       p2.second);
}

// Type alias for the map holding results organized by test case type
// Stores SortingResult to keep pass/fail status along with stats
using ResultsMap =
    std::map<std::string, std::vector<std::pair<int, SortingResult>>>;
using ArrayGenerator =
    std::function<Testing *(int)>; // Assuming definition from previous code

/**
 * @brief Runs all test cases for a given sort function.
 * @param test_generators Vector of {Test Name, Array Generator} pairs.
 * @param sizes Vector of sizes to test.
 * @param funcptr Pointer to the sorting function.
 * @param verbose Whether to print details for each run.
 * @return Pair containing: 1) Map of results organized by type, 2) Overall
 * pass/fail status (true if all passed).
 */
std::pair<ResultsMap, bool> run_all_test_cases(
    const std::vector<std::pair<std::string, ArrayGenerator>> &test_generators,
    const std::vector<int> &sizes, void (*funcptr)(Testing[], int),
    bool verbose) {
  ResultsMap results_by_type;
  bool all_passed = true;

  for (const auto &test_pair : test_generators) {
    const std::string &test_name = test_pair.first;
    const ArrayGenerator &generator = test_pair.second;
    results_by_type[test_name] = {}; // Initialize vector

    for (int size : sizes) {
      if (size < 0)
        continue;

      // Run the single test case
      SortingResult result = test_sort_func_single_case(
          generator, size, test_name, funcptr, verbose);

      // Store the full result (including pass/fail and stats)
      results_by_type[test_name].push_back({size, result});

      if (!result.sorted) {
        all_passed = false;
        if (!verbose) { // Print failure message only if verbose was off
          print_colored_line("  FAIL: " + test_name +
                                 " (Size: " + std::to_string(size) +
                                 ") did not sort correctly!",
                             testing::BOLD_RED);
        }
      }
    }
    // Sort results by size within each type for predictable processing
    std::sort(results_by_type[test_name].begin(),
              results_by_type[test_name].end(),
              [](const auto &a, const auto &b) { return a.first < b.first; });
  }

  return {results_by_type, all_passed};
}

/**
 * @brief Prints the detailed summary table of test results.
 * @param results Data organized by test type.
 */
void print_summary_table(const ResultsMap &results) {
  std::cout << std::left << std::setw(18) << "Test Case" << std::setw(10)
            << "Size" << std::setw(10) << "Result" // Pass/Fail
            << std::setw(18) << "Comparisons" << std::setw(18) << "Data Moves"
            << std::endl;
  std::cout << std::string(74, '-') << std::endl; // Separator line

  for (const auto &type_pair : results) {
    const std::string &test_name = type_pair.first;
    const auto &type_results = type_pair.second;
    bool first_line = true;

    for (const auto &res_pair : type_results) {
      int size = res_pair.first;
      const SortingResult &result = res_pair.second; // Get the full result

      std::cout << std::left << std::setw(18) << (first_line ? test_name : "")
                << std::setw(10) << size << std::setw(10)
                << (result.sorted ? "Passed" : "FAILED") // Use stored status
                << std::setw(18) << result.snapshot.total_comparisons()
                << std::setw(18) << result.snapshot.total_data_moves()
                << std::endl;
      first_line = false;
    }
  }
  std::cout << std::string(74, '-') << std::endl;
}

/**
 * @brief Analyzes results to find best/worst cases and estimate complexity,
 * then prints findings.
 * @param results Data organized by test type.
 * @param largest_size The largest size tested.
 * @param estimate_based_on_comparisons Metric to use for estimation.
 */
void analyze_and_print_complexity(const ResultsMap &results, int largest_size,
                                  bool estimate_based_on_comparisons) {
  if (largest_size <= 0) {
    print_colored_line("Complexity analysis skipped (no valid largest size).",
                       testing::YELLOW);
    return;
  }

  // Variables to track best/worst cases at the largest size
  unsigned long long min_ops_largest_size =
      std::numeric_limits<unsigned long long>::max();
  unsigned long long max_ops_largest_size = 0;
  std::string best_case_type = "N/A";
  std::string worst_case_type = "N/A";
  const SortingResult *best_result_ptr = nullptr;
  const SortingResult *worst_result_ptr = nullptr;

  // Find best/worst types at largest size
  for (const auto &type_pair : results) {
    const auto &type_results = type_pair.second;
    // Find the result for the largest size within this type
    auto it = std::find_if(
        type_results.begin(), type_results.end(),
        [largest_size](const auto &p) { return p.first == largest_size; });

    if (it != type_results.end()) {
      const SortingResult &result_at_largest = it->second;
      unsigned long long current_ops =
          estimate_based_on_comparisons
              ? result_at_largest.snapshot.total_comparisons()
              : result_at_largest.snapshot.total_data_moves();

      if (current_ops < min_ops_largest_size) {
        min_ops_largest_size = current_ops;
        best_case_type = type_pair.first;     // The name of the test case type
        best_result_ptr = &result_at_largest; // Keep pointer to the result data
      }
      if (current_ops >= max_ops_largest_size) { // Use >= to ensure worst is
                                                 // captured if multiple max out
        max_ops_largest_size = current_ops;
        worst_case_type = type_pair.first;
        worst_result_ptr = &result_at_largest;
      }
    }
  }

  // --- Complexity Estimation ---
  std::string complexity_metric_name =
      estimate_based_on_comparisons ? "Comparisons" : "Data Moves";
  std::cout << "Estimating complexity based on: " << complexity_metric_name
            << std::endl;

  Complexity best_case_complexity = Complexity::InsufficientData;
  // Ensure the best_case_type was found AND exists in the map results before
  // estimating
  if (best_case_type != "N/A" && results.count(best_case_type)) {
    // Need to convert vector<pair<int, SortingResult>> to vector<pair<int,
    // ControlStatsSnapshot>> for estimator
    std::vector<std::pair<int, ControlStatsSnapshot>> best_case_stats;
    for (const auto &p : results.at(best_case_type)) {
      best_case_stats.push_back({p.first, p.second.snapshot});
    }
    best_case_complexity = estimate_complexity_for_type(
        best_case_stats, estimate_based_on_comparisons);
  }

  Complexity worst_case_complexity = Complexity::InsufficientData;
  if (worst_case_type != "N/A" && results.count(worst_case_type)) {
    std::vector<std::pair<int, ControlStatsSnapshot>> worst_case_stats;
    for (const auto &p : results.at(worst_case_type)) {
      worst_case_stats.push_back({p.first, p.second.snapshot});
    }
    worst_case_complexity = estimate_complexity_for_type(
        worst_case_stats, estimate_based_on_comparisons);
  }

  // --- Print Complexity Summary ---
  print_colored_line("--- Empirical Complexity Estimation ---",
                     testing::YELLOW);
  std::cout << "Observed Best Case Type (at N=" << largest_size
            << "): " << best_case_type << " (" << min_ops_largest_size << " "
            << complexity_metric_name << ")" << std::endl;
  std::cout << "  Estimated Best Case Complexity:  "
            << complexity_to_string(best_case_complexity) << std::endl;

  std::cout << "Observed Worst Case Type (at N=" << largest_size
            << "): " << worst_case_type << " (" << max_ops_largest_size << " "
            << complexity_metric_name << ")" << std::endl;
  std::cout << "  Estimated Worst Case Complexity: "
            << complexity_to_string(worst_case_complexity) << std::endl;
  print_colored_line("Note: Complexity estimation is empirical and based on "
                     "observed ratios for tested sizes.",
                     testing::YELLOW);
}

/**
 * @brief Main orchestrator function for testing a sorting algorithm. Refactored
 * version.
 *
 * @param name Name of the sorting algorithm (e.g., "Bubble Sort").
 * @param funcptr Pointer to the sorting function to test.
 * @param sizes Vector of array sizes to test (should be multiple and
 * increasing).
 * @param verbose Print details for each individual test case?
 * @param estimate_based_on_comparisons True to base complexity estimation on
 * comparisons, False to use data moves.
 * @return 0 if all tests passed verification, 1 otherwise.
 */
int testing_sort_func(std::string name, void (*funcptr)(Testing[], int),
                      TestOptions options) {
  print_colored_line("--- Testing Algorithm: " + name + " ---",
                     testing::BOLD_CYAN);
  if (options.sizes.size() < 2) {
    print_colored_line("WARNING: Need at least 2 distinct sizes for reliable "
                       "complexity estimation.",
                       testing::YELLOW);
  }

  // Define standard test generators
  std::vector<std::pair<std::string, ArrayGenerator>> test_generators = {
      {"Reversed", reversed},
      {"Sorted", sorted},
      {"Random Unique", random_unique},
      {"Few Unique", few_unique},
      {"Nearly Sorted", nearly_sorted}};

  // Run all test cases and collect results
  auto [results, all_passed] = run_all_test_cases(
      test_generators, options.sizes, funcptr, options.verbose);

  // Print the summary table
  print_colored_line("\n--- Summary Table for: " + name + " ---",
                     testing::BOLD_CYAN);
  print_summary_table(results);

  // Find largest size tested that is >= 0
  int largest_size = 0;
  for (int s : options.sizes) {
    if (s > largest_size)
      largest_size = s;
  }

  // Analyze results for complexity and print findings
  analyze_and_print_complexity(results, largest_size,
                               options.estimate_based_on_comparisons);

  print_colored_line("--- Finished Testing: " + name + " ---",
                     testing::BOLD_CYAN);

  return all_passed ? 0 : 1; // Return 0 on success (all verified), 1 on failure
}

int test_all_algorithms(
    const std::vector<AlgorithmTestConfig> &algorithms_to_test) {
  print_colored_line("\n===== Running Full Algorithm Test Suite =====",
                     testing::BOLD_CYAN);

  std::vector<std::string> passed_algorithms;
  std::vector<std::string> failed_algorithms;
  int overall_status = 0; // 0 = all passed so far, 1 = at least one failure

  // Loop through each algorithm configuration provided
  for (const auto &config : algorithms_to_test) {
    // Run the individual testing function for this algorithm
    // testing_sort_func_final returns 0 on success (all verified), 1 on failure
    int result =
        testing_sort_func(config.name, config.function_pointer, config.options);

    // Record the outcome
    if (result == 0) {
      passed_algorithms.push_back(config.name);
    } else {
      failed_algorithms.push_back(config.name);
      overall_status = 1; // Mark that at least one algorithm failed
    }
    // Add a separator between algorithms for clarity in the full log
    std::cout << "\n----------------------------------------\n" << std::endl;
  }

  // --- Print Final Summary ---
  print_colored_line("===== Full Algorithm Test Suite Summary =====",
                     testing::BOLD_CYAN);

  if (!passed_algorithms.empty()) {
    std::cout << "\nAlgorithms that PASSED all verification checks:"
              << std::endl;
    for (const auto &name : passed_algorithms) {
      // Use std::setw for alignment if desired
      std::cout << "  [ PASS ] ";
      print_colored_line(name, testing::BOLD_GREEN); // Print name in green
    }
  }

  if (!failed_algorithms.empty()) {
    std::cout << "\nAlgorithms that FAILED one or more verification checks:"
              << std::endl;
    for (const auto &name : failed_algorithms) {
      std::cout << "  [ FAIL ] ";
      print_colored_line(name, testing::BOLD_RED); // Print name in red
    }
  } else {
    std::cout << "\nAll tested algorithms passed verification." << std::endl;
  }

  std::cout << "\nTotal Algorithms Tested: " << algorithms_to_test.size()
            << std::endl;
  std::cout << "Passed: " << passed_algorithms.size() << std::endl;
  std::cout << "Failed: " << failed_algorithms.size() << std::endl;

  print_colored_line("============================================",
                     testing::BOLD_CYAN);

  return overall_status;
}
} // namespace testing
