#ifndef A2_HPP
#define A2_HPP
#include <algorithm>
#include <iostream>
#define swp(i, j) std::swap(elements[i], elements[j]);

template <typename T> void printArray2(T *arr, int nrOfElements) {
  std::cout << "[";
  for (int i = 0; i < nrOfElements; i++) {
    std::cout << arr[i] << (i == nrOfElements - 1 ? "" : ", ");
  }
  std::cout << "]" << std::endl;
}
template <class T>
void Merge(T *elements, T *left, T *right, int nrOfElements,
           int leftNrOfElements, int rightNrOfElements) {
  int i = 0, j = 0, k = 0;

  while (j < leftNrOfElements and k < rightNrOfElements) {
    if (left[j] <= right[k]) {
      elements[i] = left[j];
      ++j;
    } else {
      elements[i] = right[k];
      ++k;
    }
    ++i;
  }

  while (j < leftNrOfElements) {
    elements[i] = left[j];
    ++i;
    ++j;
  }
  while (k < rightNrOfElements) {
    elements[i] = right[k];
    ++i;
    ++k;
  }
}
template <class T> void Mergesort(T elements[], int nrOfElements) {
  if (nrOfElements <= 1) {
    return;
  }
  int leftNrOfElements = nrOfElements / 2;
  int rightNrOfElements = nrOfElements - leftNrOfElements;
  T *left = new T[leftNrOfElements];
  T *right = new T[rightNrOfElements];
  for (int i = 0; i < leftNrOfElements; i++) {
    left[i] = elements[i];
  }
  for (int i = 0; i < rightNrOfElements; i++) {
    right[i] = elements[i + leftNrOfElements];
  }

  Mergesort(left, leftNrOfElements);
  Mergesort(right, rightNrOfElements);
  Merge(elements, left, right, nrOfElements, leftNrOfElements,
        rightNrOfElements);
}

template <class T> void MergeBook(T elements[], int start, int mid, int end) {
  int leftNrOfElements = mid - start + 1;
  int rightNrOfElements = end - mid;
  T *left = new T[leftNrOfElements];
  T *right = new T[rightNrOfElements];
  for (int i = 0; i < leftNrOfElements; i++) {
    left[i] = elements[start + i];
  }
  for (int j = 0; j < rightNrOfElements; j++) {
    right[j] = elements[mid + j + 1];
  }
  int i = 0, j = 0;
  int k = start;
  while (i < leftNrOfElements && j < rightNrOfElements) {
    if (left[i] <= right[j]) {
      elements[k] = left[i];
      ++i;
    } else {
      elements[k] = right[j];
      ++j;
    }
    ++k;
  }

  while (i < leftNrOfElements) {
    elements[k] = left[i];
    ++i;
    ++k;
  }

  while (j < rightNrOfElements) {
    elements[k] = right[j];
    ++j;
    ++k;
  }
  delete[] left;
  delete[] right;
}
template <class T> void MergesortBookImpl(T *elements, int p, int r) {
  if (p >= r) {
    return;
  }
  int q = (p + r) / 2;
  MergesortBookImpl(elements, p, q);
  MergesortBookImpl(elements, q + 1, r);
  MergeBook(elements, p, q, r);
}
template <class T> void MergesortBook(T elements[], int nrOfElements) {
  MergesortBookImpl(elements, 0, nrOfElements - 1);
}

template <class T> int PartitionLomuto(T elements[], int start, int end) {
  T pivot = elements[end];
  int i = (start - 1);
  for (int j = start; j <= end - 1; j++) {
    if (elements[j] <= pivot) {
      i++;
      swp(i, j);
    }
  }
  swp(i + 1, end);
  return (i + 1);
}

template <class T>
void QuicksortLomutoRecursive(T elements[], int start, int end) {
  if (start < end) {
    int pivot = PartitionLomuto(elements, start, end);
    QuicksortLomutoRecursive(elements, start, pivot - 1);
    QuicksortLomutoRecursive(elements, pivot + 1, end);
  }
}

template <class T> void QuicksortLomuto(T elements[], int nrOfElements) {
  QuicksortLomutoRecursive(elements, 0, nrOfElements - 1);
}

template <class T> int PartitionHoare(T elements[], int start, int end) {
  T pivot = elements[start];
  int i = (start - 1);
  int j = (end + 1);
  while (true) {

    do {
      i++;
    } while (elements[i] < pivot);

    do {
      j--;
    } while (elements[j] > pivot);

    if (i >= j)

      return j;
    swp(i, j);
  }
}

template <class T>
void QuicksortHoareRecursive(T elements[], int start, int end) {
  if (start < end) {
    int pivot = PartitionHoare(elements, start, end);
    QuicksortHoareRecursive(elements, start, pivot);
    QuicksortHoareRecursive(elements, pivot + 1, end);
  }
}
template <class T> void QuicksortHoare(T elements[], int nrOfElements) {
  QuicksortHoareRecursive(elements, 0, nrOfElements - 1);
}

template <class T>
int PartitionHoareImproved(T elements[], int start, int end) {
  if (start >= end) {
    return start;
  }
  swp(start, end);
  T pivot_value = elements[start];
  int i = start;
  int j = end + 1;
  while (true) {
    do {
      i++;
    } while (i <= end && elements[i] < pivot_value);
    do {
      j--;
    } while (elements[j] > pivot_value);

    if (i >= j) {
      break;
    }
    swp(i, j);
  }
  swp(start, j);
  return j;
}

template <class T>
void QuicksortHoareImprovedRecursive(T elements[], int start, int end) {
  if (start < end) {
    int pivot = PartitionHoareImproved(elements, start, end);
    QuicksortHoareImprovedRecursive(elements, start, pivot - 1);
    QuicksortHoareImprovedRecursive(elements, pivot + 1, end);
  }
}

template <class T> void QuicksortHoareImproved(T elements[], int nrOfElements) {
  if (nrOfElements > 1) {
    QuicksortHoareImprovedRecursive(elements, 0, nrOfElements - 1);
  }
}
template <class T> int MedianOfThree(T elements[], int start, int end) {
  int mid = (start + end) / 2;
  if ((elements[start] <= elements[mid] && elements[mid] <= elements[end]) ||
      (elements[start] >= elements[mid] and elements[mid] >= elements[end]))
    return mid;
  if ((elements[mid] <= elements[start] && elements[start] <= elements[end]) ||
      (elements[mid] >= elements[start] and elements[start] >= elements[end]))
    return start;
  return end;
}

template <class T>
int PartitionHoareImprovedMedainOf3(T elements[], int start, int end) {
  if (start >= end) {
    return start;
  }
  int pivot = MedianOfThree(elements, start, end);
  swp(start, pivot);
  T pivot_value = elements[start];
  int i = start;
  int j = end + 1;
  while (true) {
    do {
      i++;
    } while (i <= end && elements[i] < pivot_value);
    do {
      j--;
    } while (elements[j] > pivot_value);

    if (i >= j) {
      break;
    }
    swp(i, j);
  }
  swp(start, j);
  return j;
}
template <class T>
void QuicksortHoareImprovedMedian3Recursive(T elements[], int start, int end) {
  if (start < end) {
    int pivot = PartitionHoareImprovedMedainOf3(elements, start, end);
    QuicksortHoareImprovedMedian3Recursive(elements, start, pivot);
    QuicksortHoareImprovedMedian3Recursive(elements, pivot + 1, end);
  }
}
template <class T>
void QuicksortHoareImprovedMedian3(T elements[], int nrOfElements) {
  QuicksortHoareImprovedMedian3Recursive(elements, 0, nrOfElements - 1);
}

inline int left_child(int i) { return 2 * i + 1; }

inline int right_child(int i) { return 2 * i + 2; }

inline int parent(int i) { return (i - 1) / 2; }
template <class T> void max_heapify(T elements[], int i, int heap_size) {
  int l = left_child(i);
  int r = right_child(i);
  int largest = i;
  if (l < heap_size && elements[l] > elements[i]) {
    largest = l;
  };
  if (r < heap_size && elements[r] > elements[largest]) {
    largest = r;
  };
  if (largest != i) {
    swp(i, largest);
    max_heapify(elements, largest, heap_size);
  }
}
template <class T> void build_max_heap(T elements[], int nrOfElements) {
  for (int i = (nrOfElements >> 1) - 1; i >= 0; i--) {
    max_heapify(elements, i, nrOfElements);
  }
}
template <class T> void Heapsort(T elements[], int nrOfElements) {
  build_max_heap(elements, nrOfElements);
  int heap_size = nrOfElements;
  for (int i = nrOfElements - 1; i > 0; i--) {
    swp(0, i);
    heap_size--;
    max_heapify(elements, 0, heap_size);
  }
}

#endif
