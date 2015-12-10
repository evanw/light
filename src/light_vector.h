#pragma once

#include "light_block.h"

#include <assert.h>
#include <initializer_list>
#include <new>

namespace light {
  template <typename T>
  struct vector {
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = T *;
    using const_iterator = const T *;
    using reverse_iterator = std::reverse_iterator<T *>;
    using const_reverse_iterator = std::reverse_iterator<const T *>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    vector() {}
    explicit vector(size_t size) { resize(size); }
    vector(std::initializer_list<T> list) { insert(end(), list.begin(), list.end()); }
    vector(const vector<T> &other) { insert(end(), other.begin(), other.end()); }
    vector(vector<T> &&other) : _block(std::move(other._block)) {}
    ~vector() { clear(); }

    template <typename I>
    vector(I first, I last) { insert(end(), first, last); }

    const_iterator cbegin() const { return reinterpret_cast<const_iterator>(_block.data()); }
    const_iterator cend() const { return begin() + size(); }

    const_iterator begin() const { return reinterpret_cast<const_iterator>(_block.data()); }
    const_iterator end() const { return begin() + size(); }

    iterator begin() { return reinterpret_cast<iterator>(_block.data()); }
    iterator end() { return begin() + size(); }

    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }

    const T &front() const { return at(0); }
    const T &back() const { return at(size() - 1); }

    T &front() { return at(0); }
    T &back() { return at(size() - 1); }

    const T *data() const { return reinterpret_cast<const T *>(_block.data()); }
    T *data() { return reinterpret_cast<T *>(_block.data()); }

    bool empty() const { return _block.empty(); }
    size_t size() const { return _block.size() / sizeof(T); }
    size_t capacity() const { return _block.capacity() / sizeof(T); }

    bool operator ==(const vector<T> &other) const;
    bool operator !=(const vector<T> &other) const { return !(*this == other); }

    vector<T> &operator =(const vector<T> &other) { if (this != &other) assign(other.begin(), other.end()); return *this; }
    vector<T> &operator =(vector<T> &&other) { _block = std::move(other._block); return *this; }

    const T &operator [](size_t i) const { return at(i); }
    T &operator [](size_t i) { return at(i); }

    const T &at(size_t i) const;
    T &at(size_t i);

    template <typename ...Args>
    void emplace_back(Args &&...args);

    template <typename I>
    void assign(I first, I last) { clear(); insert(end(), first, last); }

    template <typename I>
    iterator insert(const_iterator pos, I first, I last);

    iterator insert(const_iterator pos, const T &value);
    iterator insert(const_iterator pos, T &&value);
    iterator erase(const_iterator pos) { return erase(pos, pos + 1); }
    iterator erase(const_iterator first, const_iterator last);
    void clear() { erase(begin(), end()); }
    void resize(size_t new_size);
    void resize(size_t new_size, T value);
    void reserve(size_t capacity) { _block.reserve(capacity * sizeof(T)); }
    void swap(vector<T> &other) { _block.swap(other._block); }
    void push_back(const T &other);
    void push_back(T &&other);
    void pop_back();
    void shrink_to_fit() { _block.shrink_to_fit(); }

  private:
    block _block;
  };

  template <typename T>
  bool vector<T>::operator ==(const vector<T> &other) const {
    if (size() == other.size()) {
      for (size_t i = 0, count = size(); i < count; i++) {
        if (at(i) != other.at(i)) {
          return false;
        }
      }
      return true;
    }
    return false;
  }

  template <typename T>
  const T &vector<T>::at(size_t i) const {
    assert(i < size());
    return reinterpret_cast<const T *>(_block.data())[i];
  }

  template <typename T>
  T &vector<T>::at(size_t i) {
    assert(i < size());
    return reinterpret_cast<T *>(_block.data())[i];
  }

  template <typename T>
  template <typename ...Args>
  void vector<T>::emplace_back(Args &&...args) {
    _block.grow(sizeof(T));
    new(&back()) T(std::forward<Args>(args)...);
  }

  template <typename T>
  template <typename I>
  typename vector<T>::iterator vector<T>::insert(const_iterator pos, I first, I last) {
    ptrdiff_t index = pos - begin();
    _block.insert(index * sizeof(T), std::distance(first, last) * sizeof(T));
    for (size_t i = index; first != last; ) {
      new(&at(i++)) T(*first++);
    }
    return begin() + index;
  }

  template <typename T>
  typename vector<T>::iterator vector<T>::insert(const_iterator pos, const T &value) {
    ptrdiff_t index = pos - begin();
    _block.insert(index * sizeof(T), sizeof(T));
    new(&at(index)) T(value);
    return begin() + index;
  }

  template <typename T>
  typename vector<T>::iterator vector<T>::insert(const_iterator pos, T &&value) {
    ptrdiff_t index = pos - begin();
    _block.insert(index * sizeof(T), sizeof(T));
    new(&at(index)) T(std::move(value));
    return begin() + index;
  }

  template <typename T>
  typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
    assert(begin() <= first && first <= end());
    assert(begin() <= last && last <= end());
    assert(first <= last);
    for (auto i = first; i != last; i++) {
      i->~T();
    }
    size_t index = first - begin();
    _block.remove(index * sizeof(T), std::distance(first, last) * sizeof(T));
    return begin() + index;
  }

  template <typename T>
  void vector<T>::resize(size_t new_size) {
    size_t old_size = size();
    for (size_t i = old_size; i > new_size; ) {
      at(--i).~T();
    }
    _block.resize(new_size * sizeof(T));
    for (size_t i = old_size; i < new_size; ) {
      new(&at(i++)) T();
    }
  }

  template <typename T>
  void vector<T>::resize(size_t new_size, T value) {
    size_t old_size = size();
    for (size_t i = old_size; i > new_size; ) {
      at(--i).~T();
    }
    _block.resize(new_size * sizeof(T));
    for (size_t i = old_size; i < new_size; ) {
      new(&at(i++)) T(value);
    }
  }

  template <typename T>
  void vector<T>::push_back(const T &other) {
    _block.grow(sizeof(T));
    new(&back()) T(other);
  }

  template <typename T>
  void vector<T>::push_back(T &&other) {
    _block.grow(sizeof(T));
    new(&back()) T(std::move(other));
  }

  template <typename T>
  void vector<T>::pop_back() {
    back().~T();
    _block.shrink(sizeof(T));
  }
}
