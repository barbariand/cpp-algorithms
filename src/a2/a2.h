#ifndef A2_HPP
#define A2_HPP
#include <iostream>
#include <algorithm>

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
      std::swap(elements[i], elements[j]);
    }
  }
  std::swap(elements[i + 1], elements[end]);
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

template <class T> void QuicksortHoare(T elements[], int nrOfElements) {

}

template <class T>
void QuicksortHoareImproved(T elements[], int nrOfElements) {}

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
void QuicksortHoareImprovedMedian3(T elements[], int nrOfElements) {}

template <class T> void Heapsort(T elements[], int nrOfElements) {}

#endif
