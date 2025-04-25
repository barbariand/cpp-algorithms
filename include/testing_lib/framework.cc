#include "framework.hpp"
#include <cstddef>
#include <iostream>
#include <new>
#include <stdexcept>
#include <utility>
namespace testing {
void ControlStatsSnapshot::print() const {
  std::cout << "--- Control Stats Snapshot ---" << std::endl;
  std::cout << "  < : " << less_than_count << ", > : " << greater_than_count
            << ", <=: " << less_equal_count << ", >=: " << greater_equal_count
            << ", ==: " << equal_count << ", !=: " << not_equal_count
            << std::endl;
  std::cout << "  CopyCtor: " << copy_constructor_count
            << ", CopyAssign: " << copy_assignment_count
            << ", MoveCtor: " << move_constructor_count
            << ", MoveAssign: " << move_assignment_count << std::endl;
  std::cout << "----------------------------" << std::endl;
}

// Helper to get total comparisons
unsigned long long ControlStatsSnapshot::total_comparisons() const {
  return less_than_count + greater_than_count + less_equal_count +
         greater_equal_count + equal_count + not_equal_count;
}

// Helper to get total copy/move operations (often indicative of swaps/data
// movement)
unsigned long long ControlStatsSnapshot::total_data_moves() const {
  return copy_constructor_count + copy_assignment_count +
         move_constructor_count + move_assignment_count;
}
Control &Control::get_instance() {
  static Control instance;
  return instance;
}

void Control::increment_less_than() { ++less_than_count; }

void Control::increment_greater_than() { ++greater_than_count; }

void Control::increment_less_equal() { ++less_equal_count; }

void Control::increment_greater_equal() { ++greater_equal_count; }

void Control::increment_equal() { ++equal_count; }

void Control::increment_not_equal() { ++not_equal_count; }

void Control::increment_copy_constructor() { ++copy_constructor_count; }

void Control::increment_copy_assignment() { ++copy_assignment_count; }

void Control::increment_move_constructor() { ++move_constructor_count; }

void Control::increment_move_assignment() { ++move_assignment_count; }

void Control::print_stats() const {
  std::cout << "--- Operation Stats (Singleton Control) ---" << std::endl;
  std::cout << "Comparisons:" << std::endl;
  std::cout << "  < : " << less_than_count << std::endl;
  std::cout << "  > : " << greater_than_count << std::endl;
  std::cout << "  <=: " << less_equal_count << std::endl;
  std::cout << "  >=: " << greater_equal_count << std::endl;
  std::cout << "  ==: " << equal_count << std::endl;
  std::cout << "  !=: " << not_equal_count << std::endl;
  std::cout << "Object Lifecycle:" << std::endl;
  std::cout << "  Copy Constructions: " << copy_constructor_count << std::endl;
  std::cout << "  Copy Assignments  : " << copy_assignment_count << std::endl;
  std::cout << "  Move Constructions: " << move_constructor_count << std::endl;
  std::cout << "  Move Assignments  : " << move_assignment_count << std::endl;
  std::cout << "-------------------------------------------" << std::endl;
}

ControlStatsSnapshot Control::reset_and_get_snapshot() {
  ControlStatsSnapshot snapshot;

  // 1. Capture current values into the snapshot
  snapshot.less_than_count = this->less_than_count;
  snapshot.greater_than_count = this->greater_than_count;
  snapshot.less_equal_count = this->less_equal_count;
  snapshot.greater_equal_count = this->greater_equal_count;
  snapshot.equal_count = this->equal_count;
  snapshot.not_equal_count = this->not_equal_count;

  snapshot.copy_constructor_count = this->copy_constructor_count;
  snapshot.copy_assignment_count = this->copy_assignment_count;
  snapshot.move_constructor_count = this->move_constructor_count;
  snapshot.move_assignment_count = this->move_assignment_count;

  // 2. Reset the stats in the singleton instance
  this->reset_stats(); // Call the reset_stats member function

  return snapshot;
}

void Control::reset_stats() {
  less_than_count = 0;
  greater_than_count = 0;
  less_equal_count = 0;
  greater_equal_count = 0;
  equal_count = 0;
  not_equal_count = 0;
  copy_constructor_count = 0;
  copy_assignment_count = 0;
  move_constructor_count = 0;
  move_assignment_count = 0;
}

// --- Definitions for class Testing ---

Testing::Testing() : value(0), control(&Control::get_instance()) {
  // Constructor body was empty
}

Testing::Testing(int val) : value(val), control(&Control::get_instance()) {
  // Constructor body was empty
}

Testing::Testing(const Testing &other)
    : value(other.value), control(&Control::get_instance()) {
  control->increment_copy_constructor();
}

Testing &Testing::operator=(const Testing &other) {
  if (this == &other) {
    return *this;
  }
  value = other.value;
  // Assuming control pointer is already initialized correctly in the existing
  // object
  if (control) { // Good practice to check if control is not null
    control->increment_copy_assignment();
  } else {
    // Handle error: control pointer is null, maybe re-assign it?
    // For now, just skip incrementing if control is null.
    // Or initialize it: control = &Control::get_instance();
    // control->increment...
  }
  return *this;
}

Testing::Testing(Testing &&other) noexcept
    : value(other.value),
      control(&Control::get_instance()) { // Note: Member initialization list is
                                          // generally preferred
  // 'other.value' is accessed, usually you'd std::move if it were a resource
  control->increment_move_constructor();
  // Optionally reset other's state if it's a destructive move
  // other.value = 0; // For example
}

Testing &Testing::operator=(Testing &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  value = other.value; // Again, usually std::move if it's a resource
  // Assuming control pointer is already initialized correctly
  if (control) { // Good practice to check
    control->increment_move_assignment();
  } else {
    // Handle error: control pointer is null
  }
  // Optionally reset other's state
  // other.value = 0;
  return *this;
}

int Testing::get_value() const { return value; }

// --- Definitions for free functions (operators for Testing) ---

bool operator<(const Testing &lhs, const Testing &rhs) {
  if (lhs.control) { // Accessing member 'control', ok since declared friend
    lhs.control->increment_less_than();
  }
  return lhs.value < rhs.value;
}

bool operator>(const Testing &lhs, const Testing &rhs) {
  if (lhs.control) {
    lhs.control->increment_greater_than();
  }
  return lhs.value > rhs.value;
}

bool operator<=(const Testing &lhs, const Testing &rhs) {
  if (lhs.control) {
    lhs.control->increment_less_equal();
  }
  return lhs.value <= rhs.value;
}

bool operator>=(const Testing &lhs, const Testing &rhs) {
  if (lhs.control) {
    lhs.control->increment_greater_equal();
  }
  return lhs.value >= rhs.value;
}

bool operator==(const Testing &lhs, const Testing &rhs) {
  if (lhs.control) {
    lhs.control->increment_equal();
  }
  return lhs.value == rhs.value;
}

bool operator!=(const Testing &lhs, const Testing &rhs) {
  if (lhs.control) {
    lhs.control->increment_not_equal();
  }
  return lhs.value != rhs.value;
}

std::ostream &operator<<(std::ostream &os, const Testing &obj) {
  os << obj.value; // Accessing private member 'value', ok since declared friend
  return os;
}
} // namespace testing
