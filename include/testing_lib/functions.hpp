#ifndef CINDY_TESTING_FRAMEWORK_SORTING_FUNCTIONS_H
#define CINDY_TESTING_FRAMEWORK_SORTING_FUNCTIONS_H
#include "framework.hpp"
#include "prelude.hpp"
#include <algorithm>
#include <random>
namespace testing_functions {
PRELUDE;
using testing_framework::Control;
using testing_framework::ControlStatsSnapshot;
using testing_framework::Testing;
inline Testing *reversed(int size) {
  if (size <= 0)
    return nullptr;
  Testing *arr = new Testing[size];
  for (int i = 0; i < size; i++) {

    arr[i] = Testing(size - 1 - i);
  }
  return arr;
}
inline Testing *sorted(int size) {
  if (size <= 0)
    return nullptr;
  Testing *arr = new Testing[size];
  for (int i = 0; i < size; i++) {
    arr[i] = Testing(i);
  }
  return arr;
}

inline Testing *random_unique(int size) {
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
inline Testing *few_unique(int size) {
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

inline Testing *nearly_sorted(int size) {
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
} // namespace testing_functions
#endif
