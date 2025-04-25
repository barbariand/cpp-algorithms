#include "sort.hpp"
#include "complexity.hpp"
#include "functions.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>
namespace testing {
SortingResult test_sort_func_single_case(const ArrayGenerator &generator,
                                         int size, const std::string &test_name,
                                         const SortingFunction *funcptr,
                                         const TestOptions &options) {

  std::stringstream log_buffer;
  SortingResult result;
  result.test_case_name = test_name;
  result.array_size = size;
  result.sorted = false;

  std::cout << "-- Running Case: " << test_name << " (Size: " << size << ") --"
            << std::endl;

  Testing *arr = generator(size);
  if (!arr) {
    if (size <= 0) {
      std::cout << "  Skipped (Size <= 0)" << std::endl;
      result.snapshot = Control::get_instance().reset_and_get_snapshot();

      result.sorted = (size <= 1);
      return result;
    } else {
      print_colored_line("  ERROR: Array generator returned nullptr!",
                         testing::BOLD_RED);
      result.snapshot = Control::get_instance().reset_and_get_snapshot();
      result.sorted = false;
      return result;
    }
  }
  std::unique_ptr<Testing[]> arr_ptr(arr);

  log_buffer << "  Initial array: ";

  std::streambuf *cout_buf = std::cout.rdbuf();
  std::cout.rdbuf(log_buffer.rdbuf());
  printArray(arr_ptr.get(), size);
  std::cout.rdbuf(cout_buf);
  log_buffer << std::endl;

  Control::get_instance().reset_and_get_snapshot();
  funcptr(arr_ptr.get(), size);
  result.snapshot = Control::get_instance().reset_and_get_snapshot();

  log_buffer << "  Sorted array:  ";
  cout_buf = std::cout.rdbuf();
  std::cout.rdbuf(log_buffer.rdbuf());
  printArray(arr_ptr.get(), size);
  std::cout.rdbuf(cout_buf);
  log_buffer << std::endl;

  log_buffer << "  Statistics:" << std::endl;

  cout_buf = std::cout.rdbuf();
  std::cout.rdbuf(log_buffer.rdbuf());
  result.snapshot.print();
  std::cout.rdbuf(cout_buf);

  result.sorted = std::is_sorted(arr_ptr.get(), arr_ptr.get() + size);

  std::cout << "  Verification: ";
  if (result.sorted) {
    print_colored_line("Passed", testing::BOLD_GREEN);
  } else {
    print_colored_line("Failed", testing::BOLD_RED);
  }

  if (!result.sorted || options.verbose) {
    std::string log_header =
        !result.sorted ? "-- Failure Log --" : "-- Verbose Log --";
    print_colored_line(log_header,
                       !result.sorted ? testing::BOLD_RED : testing::YELLOW);

    std::cout << log_buffer.str();
    std::cout << "-------------------" << std::endl;
  }

  return result;
}

using ResultsMap =
    std::map<std::string, std::vector<std::pair<int, SortingResult>>>;

void print_summary_table(const ResultsMap &results) {

  std::cout << std::left << std::setw(18) << "Test Case" << std::setw(10)
            << "Size" << std::setw(10) << "Result" << std::setw(18)
            << "Comparisons" << std::setw(18) << "Data Moves" << std::endl;
  std::cout << std::string(74, '-') << std::endl;

  const bool is_tty = is_stdout_a_tty();

  for (const auto &type_pair : results) {
    const std::string &test_name = type_pair.first;
    const auto &type_results = type_pair.second;
    bool first_line = true;

    for (const auto &res_pair : type_results) {
      int size = res_pair.first;
      const SortingResult &result = res_pair.second;

      const char *bg_color_code = "";
      if (is_tty) {
        bg_color_code = result.sorted ? BG_GREEN : BG_RED;
        std::cout << bg_color_code;
      }

      std::cout << std::left << std::setw(18) << (first_line ? test_name : "")
                << std::setw(10) << size << std::setw(10)
                << (result.sorted ? "Passed" : "FAILED") << std::setw(18)
                << result.snapshot.total_comparisons() << std::setw(18)
                << result.snapshot.total_data_moves();

      if (is_tty) {
        std::cout << testing::RESET;
      }

      std::cout << std::endl;

      first_line = false;
    }
  }

  std::cout << std::string(74, '-') << std::endl;
}

std::pair<ResultsMap, bool> run_all_test_cases(
    const std::vector<std::pair<std::string, ArrayGenerator>> &test_generators,
    const TestOptions &options, const SortingFunction *funcptr) {
  ResultsMap results_by_type;
  bool overall_verification_passed = true;

  for (const auto &test_pair : test_generators) {
    const std::string &test_name = test_pair.first;
    const ArrayGenerator &generator = test_pair.second;
    results_by_type[test_name] = {};

    for (int size : options.sizes) {
      if (size < 0)
        continue;

      SortingResult result = test_sort_func_single_case(
          generator, size, test_name, funcptr, options);
      results_by_type[test_name].push_back({size, result});

      if (!result.sorted) {
        overall_verification_passed = false;
      }
    }
    std::sort(results_by_type[test_name].begin(),
              results_by_type[test_name].end(),
              [](const auto &a, const auto &b) { return a.first < b.first; });
  }
  return {results_by_type, overall_verification_passed};
}

AlgorithmRunStatus testing_sort_func(std::string name,
                                     const SortingFunction *funcptr,
                                     const TestOptions &options) {
  print_colored_line("--- Testing Algorithm: " + name + " ---", BOLD_CYAN);

  std::vector<std::pair<std::string, ArrayGenerator>> test_generators = {
      {"Reversed", testing_functions::reversed},
      {"Sorted", testing_functions::sorted},
      {"Random Unique", testing_functions::random_unique},
      {"Few Unique", testing_functions::few_unique},
      {"Nearly Sorted", testing_functions::nearly_sorted}};

  auto [results, all_passed_verification] =
      run_all_test_cases(test_generators, options, funcptr);

  print_colored_line("\n--- Summary Table for: " + name + " ---", BOLD_CYAN);
  print_summary_table(results);

  bool complexity_mismatch = analyze_and_print_complexity(results, options);

  print_colored_line("--- Finished Testing: " + name + " ---", BOLD_CYAN);

  if (!all_passed_verification) {
    return AlgorithmRunStatus::FAILED_VERIFICATION;
  } else if (complexity_mismatch) {
    return AlgorithmRunStatus::PASSED_COMPLEXITY_WARN;
  } else {
    return AlgorithmRunStatus::PASSED;
  }
}

int test_all_algorithms(
    const std::vector<AlgorithmTestConfig> &algorithms_to_test) {
  print_colored_line("\n===== Running Full Algorithm Test Suite =====",
                     BOLD_CYAN);

  std::map<std::string, AlgorithmRunStatus> results;
  bool any_warnings_or_failures = false;

  for (const auto &config : algorithms_to_test) {
    AlgorithmRunStatus status =
        testing_sort_func(config.name, config.funcptr, config.options);
    results[config.name] = status;

    if (status != AlgorithmRunStatus::PASSED) {
      any_warnings_or_failures = true;
    }
    std::cout << "\n----------------------------------------\n" << std::endl;
  }

  print_colored_line("===== Full Algorithm Test Suite Summary =====",
                     testing::BOLD_CYAN);
  int pass_count = 0;
  int warn_count = 0;
  int fail_count = 0;

  for (const auto &pair : results) {
    const std::string &name = pair.first;
    AlgorithmRunStatus status = pair.second;

    switch (status) {
    case AlgorithmRunStatus::PASSED:
      std::cout << "  [ PASS ] ";
      print_colored_line(name, testing::BOLD_GREEN);
      pass_count++;
      break;
    case AlgorithmRunStatus::PASSED_COMPLEXITY_WARN:
      std::cout << "  [ WARN ] ";
      print_colored_line(name + " (Complexity Mismatch)", testing::BOLD_YELLOW);
      warn_count++;
      break;
    case AlgorithmRunStatus::FAILED_VERIFICATION:
      std::cout << "  [ FAIL ] ";
      print_colored_line(name + " (Verification Failed)", testing::BOLD_RED);
      fail_count++;
      break;
    }
  }

  std::cout << "\nTotal Algorithms Tested: " << algorithms_to_test.size()
            << std::endl;
  std::cout << "  Passed:              " << pass_count << std::endl;
  std::cout << "  Passed (Warn):       " << warn_count << std::endl;
  std::cout << "  Failed Verification: " << fail_count << std::endl;
  print_colored_line("============================================",
                     testing::BOLD_CYAN);

  return any_warnings_or_failures ? 1 : 0;
}
} // namespace testing
