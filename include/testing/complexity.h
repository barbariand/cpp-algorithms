#ifndef CINDY_TESTING_FRAMEWORK_COMPLEXITY_H
#define CINDY_TESTING_FRAMEWORK_COMPLEXITY_H
#include "utils.h"
#include <string>
namespace testing_complexity {
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
#endif
