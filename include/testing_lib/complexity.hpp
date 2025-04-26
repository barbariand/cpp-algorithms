#ifndef CINDY_TESTING_FRAMEWORK_COMPLEXITY_H
#define CINDY_TESTING_FRAMEWORK_COMPLEXITY_H
#include "framework.hpp"
#include "prelude.hpp"
#include "utils.hpp"
#include <string>
namespace testing_complexity {
using testing_framework::Complexity;
using testing_framework::Control;
using testing_framework::ControlStatsSnapshot;
using testing_framework::SortingResult;
using testing_framework::Testing;
using testing_utils::print_colored;
using testing_utils::print_colored_line;
PRELUDE;
std::string complexity_to_string(Complexity c);

Complexity estimate_complexity_from_pair(double n1, double ops1, double n2,
                                         double ops2);
Complexity estimate_complexity_for_type(
    const std::vector<std::pair<int, ControlStatsSnapshot>> &data,
    bool use_comparisons = true);

bool analyze_and_print_complexity(
    const ResultsMap &results, const testing_framework::TestOptions &options);
} // namespace testing_complexity

#endif
