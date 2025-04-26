// File: prelude_defs.hpp

#ifndef PRELUDE_DEFS_HPP
#define PRELUDE_DEFS_HPP
#include <functional>// IWYU pragma: keep.
#include <map>// IWYU pragma: keep.
#define PRELUDE                                                                \
  using ResultsMap =                                                           \
      std::map<std::string,                                                    \
               std::vector<std::pair<int, testing_framework::SortingResult>>>; \
  using SortingFunction =                                                      \
      std::function<void(testing_framework::Testing[], int)>;                  \
  using ArrayGenerator = std::function<testing_framework::Testing *(int)>;

// --- End of protected content ---

#endif
