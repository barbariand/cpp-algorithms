#ifndef A1_HPP
#define A1_HPP
#include <algorithm> // Included for use of std::swap()
#include <iostream>

template <class T> int LinearSearch(T elements[], int nrOfElements, T element) {
  for (int i = 0; i < nrOfElements; i++) {
    if (elements[i] == element) {
      return i;
    }
  }
  return -1;
}
template <class T> void Selectionsort(T elements[], int nrOfElements) {
  for (int i = 0; i < nrOfElements - 1; i++) {
    int min_value = i;
    for (int j = i + 1; j < nrOfElements; j++) {
      if (elements[j] < elements[min_value]) {
        min_value = j;
      }
    }
    std::swap(elements[i], elements[min_value]);
  }
}

template <class T> void Insertionsort(T elements[], int nrOfElements) {
  for (int i = 1; i < nrOfElements; i++) {
    T key = elements[i];
    int j = i - 1;
    while (j >= 0 && elements[j] > key) {
      elements[j + 1] = elements[j];
      j--;
    }
    elements[j + 1] = key;
  }
}

template <class T> int BinarySearch(T elements[], int nrOfElements, T element) {
  int start = 0;
  int end = nrOfElements - 1;
  while (start <= end) {
    int middle = (end + start) >> 1;
    if (elements[middle] == element) {
      return middle;
    }
    if (elements[middle] < element) {
      start = middle + 1;
    } else {
      end = middle - 1;
    }
  }
  return -1;
}

template <class T>
int LinearSearchRecursive(T elements[], int nrOfElements, T element) {
  // Implementera en rekursiv linjärsökning.
  // Anropa er egna rekursiva funktion härifrån.
  return LinearSearchRecursiveImpl(elements, nrOfElements, element);
}
template <class T>
int LinearSearchRecursiveImpl(T elements[], int nrOfElelments, T element,
                              int start = 0) {
  if (start >= nrOfElelments) {
    return -1;
  }
  if (elements[start] == element) {
    return start;
  } else {
    return LinearSearchRecursiveImpl(elements, nrOfElelments, element,
                                     start + 1);
  }
}

template <class T>
int BinarySearchRecursiveImpl(T elements[], T element, int start, int end) {
  if (start > end) {
    return -1;
  }
  std::cout << "start:" << start << std::endl;
  std::cout << "end:" << end << std::endl;
  int middle = (end + start) >> 1;
  std::cout << "middle:" << middle << std::endl;
  if (elements[middle] == element) {
    return middle;
  }
  if (elements[middle] < element) {
    return BinarySearchRecursiveImpl(elements, element, middle + 1, end);
  } else {
    return BinarySearchRecursiveImpl(elements, element, start, middle - 1);
  }
}
template <class T>
int BinarySearchRecursive(T elements[], int nrOfElements, T element) {

  return BinarySearchRecursiveImpl(elements, element, 0, nrOfElements - 1);
}

// linear binary search that returns the key where you could insert smth
template <class T>
int BinarySearchLinearForInsertionSort(T elements[], int index) {
  int end = index - 1;
  int start = 0;
  while (start <= end) {
    int middle = (end + start) >> 1;
    if (elements[middle] == elements[index]) {
      return middle;
    }
    if (elements[middle] < elements[index]) {
      start = middle + 1;
    } else {
      end = middle - 1;
    }
  }
  return start;
}
template <class T> void BinaryInsertionsort(T elements[], int nrOfElements) {
  for (int i = 1; i < nrOfElements; i++) {
    T key = elements[i];
    int move_untill = BinarySearchLinearForInsertionSort(elements, i);
    int j = i - 1;
    while (j >= move_untill) {
      elements[j + 1] = elements[j];
      j--;
    }
    elements[j + 1] = key;
  }
}
#endif
