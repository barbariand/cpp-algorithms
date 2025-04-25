#ifndef STACKLINKEDLIST_H
#define STACKLINKEDLIST_H
#include <stdexcept>

template <typename T> class StackLinkedList {
private:
  class Node {
  public:
    T data;
    Node *next;
    Node(T data, Node *next = nullptr) : data(data), next(next) {}
  };

public:
  StackLinkedList();
  virtual ~StackLinkedList();
  StackLinkedList(const StackLinkedList &other) = delete;
  StackLinkedList &operator=(const StackLinkedList &other) = delete;
  void push(const T &element);
  T pop();
  const T &peek() const;
  bool isEmpty() const;
  int size() const;
};

template <typename T> inline StackLinkedList<T>::StackLinkedList() {}

template <typename T> inline StackLinkedList<T>::~StackLinkedList() {}

template <typename T> inline void StackLinkedList<T>::push(const T &element) {}

template <typename T> inline T StackLinkedList<T>::pop() {
  T removeThis;
  return removeThis;
}

template <typename T> inline const T &StackLinkedList<T>::peek() const {
  T removeThis;
  return removeThis;
}

template <typename T> inline bool StackLinkedList<T>::isEmpty() const {
  return false;
}

template <typename T> inline int StackLinkedList<T>::size() const { return 0; }

#endif
