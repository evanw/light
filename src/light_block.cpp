#include "light_block.h"

#include <algorithm>
#include <assert.h>
#include <memory.h>

using namespace light;

block::~block() {
  free(_data);
}

block &block::operator =(const block &other) {
  if (this != &other) {
    resize(other._size);
    memcpy(_data, other._data, other._size);
  }
  return *this;
}

block &block::operator =(block &&other) {
  if (this != &other) {
    swap(other);
  }
  return *this;
}

void block::grow(size_t delta) {
  resize(_size + delta);
}

void block::shrink(size_t delta) {
  assert(_size >= delta);
  resize(_size - delta);
}

void block::resize(size_t size) {
  if (size > _capacity) {
    reserve(size * 2);
  }
  _size = size;
}

void block::reserve(size_t capacity) {
  if (capacity > _capacity) {
    change_capacity(capacity);
  }
}

void block::insert(size_t index, size_t delta) {
  assert(index <= _size);
  size_t count = _size - index;
  grow(delta);
  move(index, index + delta, count);
}

void block::remove(size_t index, size_t delta) {
  assert(index <= _size);
  assert(delta <= _size);
  assert(index + delta <= _size);
  move(index + delta, index, _size - index - delta);
  shrink(delta);
}

void block::move(size_t before, size_t after, size_t count) {
  assert(before + count <= _size);
  assert(after + count <= _size);
  memmove(_data + after, _data + before, count);
}

void block::swap(block &other) {
  std::swap(_data, other._data);
  std::swap(_capacity, other._capacity);
  std::swap(_size, other._size);
}

void block::shrink_to_fit() {
  if (_size != _capacity) {
    change_capacity(_size);
  }
}

void block::change_capacity(size_t capacity) {
  assert(capacity >= _size);
  _capacity = capacity;
  uint8_t *data = nullptr;
  int status = posix_memalign(reinterpret_cast<void **>(&data), 16, _capacity);
  assert(status == 0);
  if (_data != nullptr) {
    memcpy(data, _data, _size);
    free(_data);
  }
  _data = data;
}
