#ifndef CINDY_TESTING_FRAMEWORK_H
#define CINDY_TESTING_FRAMEWORK_H
#include <iostream>
#include <optional>
#include <vector>
namespace testing_framework {
struct ControlStatsSnapshot {

  unsigned long long less_than_count = 0;
  unsigned long long greater_than_count = 0;
  unsigned long long less_equal_count = 0;
  unsigned long long greater_equal_count = 0;
  unsigned long long equal_count = 0;
  unsigned long long not_equal_count = 0;
  unsigned long long copy_constructor_count = 0;
  unsigned long long copy_assignment_count = 0;
  unsigned long long move_constructor_count = 0;
  unsigned long long move_assignment_count = 0;
  unsigned long long total_comparisons() const;
  unsigned long long total_data_moves() const;
  void print() const;
};

class Control {
public:
  Control(const Control &) = delete;
  Control &operator=(const Control &) = delete;
  Control(Control &&) = delete;
  Control &operator=(Control &&) = delete;

  static Control &get_instance();

  unsigned long long less_than_count = 0;
  unsigned long long greater_than_count = 0;
  unsigned long long less_equal_count = 0;
  unsigned long long greater_equal_count = 0;
  unsigned long long equal_count = 0;
  unsigned long long not_equal_count = 0;
  unsigned long long copy_constructor_count = 0;
  unsigned long long copy_assignment_count = 0;
  unsigned long long move_constructor_count = 0;
  unsigned long long move_assignment_count = 0;

  void increment_less_than();
  void increment_greater_than();
  void increment_less_equal();
  void increment_greater_equal();
  void increment_equal();
  void increment_not_equal();

  void increment_copy_constructor();
  void increment_copy_assignment();
  void increment_move_constructor();
  void increment_move_assignment();

  void print_stats() const;

  ControlStatsSnapshot reset_and_get_snapshot();
  void reset_stats();

private:
  Control() = default;
};

class Testing {
public:
  int value;

  Control *control;

  Testing();
  Testing(int val);

  Testing(const Testing &other);
  Testing(Testing &&other) noexcept;

  Testing &operator=(const Testing &other);
  Testing &operator=(Testing &&other) noexcept;

  ~Testing() = default;

  int get_value() const;

  friend bool operator<(const Testing &lhs, const Testing &rhs);
  friend bool operator>(const Testing &lhs, const Testing &rhs);
  friend bool operator<=(const Testing &lhs, const Testing &rhs);
  friend bool operator>=(const Testing &lhs, const Testing &rhs);
  friend bool operator==(const Testing &lhs, const Testing &rhs);
  friend bool operator!=(const Testing &lhs, const Testing &rhs);
  friend std::ostream &operator<<(std::ostream &os, const Testing &obj);
};
bool operator<(const Testing &lhs, const Testing &rhs);
bool operator>(const Testing &lhs, const Testing &rhs);
bool operator<=(const Testing &lhs, const Testing &rhs);
bool operator>=(const Testing &lhs, const Testing &rhs);
bool operator==(const Testing &lhs, const Testing &rhs);
bool operator!=(const Testing &lhs, const Testing &rhs);
std::ostream &operator<<(std::ostream &os, const Testing &obj);

struct SortingResult {
  ControlStatsSnapshot snapshot;
  bool sorted;
  std::string test_case_name;
  int array_size;
};

enum class Complexity { O1, ON, ONLogN, ON2, Undetermined, InsufficientData };
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

} // namespace testing_framework
#endif
