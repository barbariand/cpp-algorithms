#include "complexity.hpp"
#include "framework.hpp"
#include "prelude.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
namespace testing_complexity {
using testing_framework::Complexity;
using testing_framework::ControlStatsSnapshot;
using testing_framework::SortingResult;
using testing_utils::print_colored_line;
PRELUDE;
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
    bool use_comparisons) {
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
bool analyze_and_print_complexity(
    const ResultsMap &results,
    const testing_framework::TestOptions &options) // Pass full options struct
{
  bool complexity_mismatch_occurred = false;
  const auto &sizes = options.sizes; // Get sizes from options

  // Find largest size
  int largest_size = 0;
  if (!sizes.empty()) {
    int temp_max = std::numeric_limits<int>::min();
    bool found_positive = false;
    for (int s : sizes) {
      if (s >= 0) {
        temp_max = std::max(temp_max, s);
        found_positive = true;
      }
    }
    if (found_positive)
      largest_size = temp_max;
  }

  // Skip if no valid sizes or results
  if (largest_size <= 0 || results.empty()) {
    print_colored_line("Complexity analysis skipped (no valid sizes/results).",
                       testing_utils::YELLOW);
    return false; // No mismatch possible if not analyzed
  }

  // --- Find Best/Worst Case Types (at largest size) ---
  unsigned long long min_ops_largest_size =
      std::numeric_limits<unsigned long long>::max();
  unsigned long long max_ops_largest_size = 0;
  std::string best_case_type = "N/A";
  std::string worst_case_type = "N/A";

  for (const auto &type_pair : results) {
    const auto &type_results = type_pair.second;
    auto it = std::find_if(
        type_results.begin(), type_results.end(),
        [largest_size](const auto &p) { return p.first == largest_size; });
    if (it != type_results.end()) {
      const SortingResult &res = it->second;
      unsigned long long current_ops = options.estimate_based_on_comparisons
                                           ? res.snapshot.total_comparisons()
                                           : res.snapshot.total_data_moves();
      if (current_ops < min_ops_largest_size) {
        min_ops_largest_size = current_ops;
        best_case_type = type_pair.first;
      }
      if (current_ops >= max_ops_largest_size) {
        max_ops_largest_size = current_ops;
        worst_case_type = type_pair.first;
      }
    }
  }

  // --- Estimate Complexity ---
  std::string complexity_metric_name =
      options.estimate_based_on_comparisons ? "Comparisons" : "Data Moves";
  std::cout << "Estimating complexity based on: " << complexity_metric_name
            << std::endl;

  Complexity best_case_est = Complexity::InsufficientData;
  if (best_case_type != "N/A" && results.count(best_case_type)) {
    std::vector<std::pair<int, ControlStatsSnapshot>> stats;
    for (const auto &p : results.at(best_case_type)) {
      stats.push_back({p.first, p.second.snapshot});
    }
    best_case_est = estimate_complexity_for_type(
        stats, options.estimate_based_on_comparisons);
  }

  Complexity worst_case_est = Complexity::InsufficientData;
  if (worst_case_type != "N/A" && results.count(worst_case_type)) {
    std::vector<std::pair<int, ControlStatsSnapshot>> stats;
    for (const auto &p : results.at(worst_case_type)) {
      stats.push_back({p.first, p.second.snapshot});
    }
    worst_case_est = estimate_complexity_for_type(
        stats, options.estimate_based_on_comparisons);
  }

  // --- Compare with Expectations & Prepare Status Strings ---
  std::string best_status_str;
  if (options.expected_best_complexity.has_value()) {
    if (best_case_est == options.expected_best_complexity.value()) {
      best_status_str = " (Matches Expected)";
    } else {
      best_status_str =
          " (MISMATCH! Expected: " +
          complexity_to_string(options.expected_best_complexity.value()) + ")";
      complexity_mismatch_occurred = true; // Flag mismatch
    }
  } // else: No expectation set, no status string

  std::string worst_status_str;
  if (options.expected_worst_complexity.has_value()) {
    if (worst_case_est == options.expected_worst_complexity.value()) {
      worst_status_str = " (Matches Expected)";
    } else {
      worst_status_str =
          " (MISMATCH! Expected: " +
          complexity_to_string(options.expected_worst_complexity.value()) + ")";
      complexity_mismatch_occurred = true; // Flag mismatch
    }
  } // else: No expectation set, no status string

  // --- Print Complexity Summary ---
  print_colored_line("--- Empirical Complexity Estimation ---",
                     testing_utils::YELLOW);
  std::cout << "Observed Best Case Type (at N=" << largest_size
            << "): " << best_case_type << " (" << min_ops_largest_size << " "
            << complexity_metric_name << ")" << std::endl;
  // Print estimate and comparison status
  print_colored_line("  Estimated Best Case Complexity:  " +
                         complexity_to_string(best_case_est) + best_status_str,
                     (best_status_str.find("MISMATCH") != std::string::npos)
                         ? testing_utils::BOLD_RED
                         : testing_utils::RESET); // Color if mismatch

  std::cout << "Observed Worst Case Type (at N=" << largest_size
            << "): " << worst_case_type << " (" << max_ops_largest_size << " "
            << complexity_metric_name << ")" << std::endl;
  // Print estimate and comparison status
  print_colored_line("  Estimated Worst Case Complexity: " +
                         complexity_to_string(worst_case_est) +
                         worst_status_str,
                     (worst_status_str.find("MISMATCH") != std::string::npos)
                         ? testing_utils::BOLD_RED
                         : testing_utils::RESET); // Color if mismatch

  print_colored_line("Note: Complexity estimation is empirical and based on "
                     "observed ratios for tested sizes.",
                     testing_utils::YELLOW);

  return complexity_mismatch_occurred;
}
} // namespace testing_complexity
