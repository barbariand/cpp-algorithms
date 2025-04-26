#ifndef QUEUEARRAY_H
#define QUEUEARRAY_H
#include <stdexcept>

template <typename T> class QueueArray {
private:
  T *elements;
  int capacity;

public:
  QueueArray();
  virtual ~QueueArray();
  QueueArray(const QueueArray &other) = delete;
  QueueArray &operator=(const QueueArray &other) = delete;
  void enqueue(const T &element);
  T dequeue();
  const T &peek() const;
  bool isEmpty() const;
  int size() const;
};

template <typename T> inline QueueArray<T>::QueueArray() {}

template <typename T> inline QueueArray<T>::~QueueArray() {}

template <typename T> inline void QueueArray<T>::enqueue(const T &element) {}

template <typename T> inline T QueueArray<T>::dequeue() {
  T removeThis;
  return removeThis;
}

template <typename T> inline const T &QueueArray<T>::peek() const {
  T removeThis;
  return removeThis;
}

template <typename T> inline bool QueueArray<T>::isEmpty() const {
  return false;
}

template <typename T> inline int QueueArray<T>::size() const { return -1; }

#endif
