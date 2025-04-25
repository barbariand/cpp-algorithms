
#ifndef CINDY_TESTING_FRAMEWORK_UTILS_H // Conventionally add _H suffix
#define CINDY_TESTING_FRAMEWORK_UTILS_H

#include <iostream>
#include <mutex>  // For std::call_once, std::once_flag
#include <string> // Include necessary headers for function bodies

// Platform specific includes and macros for IS_TERMINAL
#ifdef _WIN32
#include <io.h>
#include <stdio.h>
#define IS_TERMINAL(stream) _isatty(_fileno(stream))
#else
#include <stdio.h>
#include <unistd.h>
#define IS_TERMINAL(stream) isatty(fileno(stream))
#endif

namespace testing {

// --- Color Constants ---
const char *const BG_GREEN = "\033[42m"; // Green Background
const char *const BG_RED = "\033[41m";   // Red Background
const char *const RESET = "\033[0m";
const char *const BLACK = "\033[30m";
const char *const RED = "\033[31m";
const char *const GREEN = "\033[32m";
const char *const YELLOW = "\033[33m";
const char *const BLUE = "\033[34m";
const char *const MAGENTA = "\033[35m";
const char *const CYAN = "\033[36m";
const char *const WHITE = "\033[37m";
const char *const BOLD_RED = "\033[1;31m";
const char *const BOLD_GREEN = "\033[1;32m";
const char *const BOLD_YELLOW = "\033[1;33m";
const char *const BOLD_CYAN = "\033[1;36m";

// --- TTY Check (Definition in Header) ---
inline bool is_stdout_a_tty() {
  static bool stdout_is_tty_cached;
  static std::once_flag flag;
  std::call_once(flag,
                 []() { stdout_is_tty_cached = (IS_TERMINAL(stdout) != 0); });
  return stdout_is_tty_cached;
}

// --- Printing Functions (Definitions in Header) ---

inline void print_colored(const std::string &text, const char *color_code) {
  if (is_stdout_a_tty() && color_code != RESET &&
      color_code[0] != '\0') { // Added check for non-reset/empty color
    std::cout << color_code << text << RESET;
  } else {
    std::cout << text;
  }
}

inline void print_colored(const char *text, const char *color_code) {
  if (is_stdout_a_tty() && color_code != RESET && color_code[0] != '\0') {
    std::cout << color_code << text << RESET;
  } else {
    std::cout << text;
  }
}

inline void print_colored_line(const std::string &text,
                               const char *color_code) {
  if (is_stdout_a_tty() && color_code != RESET && color_code[0] != '\0') {
    std::cout << color_code << text << RESET << std::endl;
  } else {
    std::cout << text << std::endl;
  }
}

// Make sure the parameter order matches the declaration if split
inline void print_colored_line(const char *text, const char *color_code) {
  if (is_stdout_a_tty() && color_code != RESET && color_code[0] != '\0') {
    std::cout << color_code << text << RESET << std::endl;
  } else {
    std::cout << text << std::endl;
  }
}

// --- printArray Template (Already correctly defined in header) ---
template <typename T>
void printArray(const T *arr, int nrOfElements) { // Made arr const T*
  std::cout << "["; // Removed extra space for consistency maybe?
  for (int i = 0; i < nrOfElements; i++) {
    std::cout << arr[i] << (i == nrOfElements - 1 ? "" : ", ");
  }
  std::cout << "]" << std::endl;
}

} // namespace testing

#endif // CINDY_TESTING_FRAMEWORK_UTILS_H
