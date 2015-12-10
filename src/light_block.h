#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <utility>

namespace light {
  struct block {
    block() {}
    explicit block(size_t size) { resize(size); }
    block(const block &other) { *this = other; }
    block(block &&other) { *this = std::move(other); }
    ~block();

    block &operator =(const block &other);
    block &operator =(block &&other);

    size_t size() const { return _size; }
    size_t capacity() const { return _capacity; }
    bool empty() const { return !_size; }

    const void *data() const { return _data; }
    void *data() { return _data; }

    void clear() { _size = 0; }
    void grow(size_t delta);
    void shrink(size_t delta);
    void resize(size_t size);
    void reserve(size_t capacity);
    void insert(size_t index, size_t delta);
    void remove(size_t index, size_t delta);
    void move(size_t before, size_t after, size_t count);
    void swap(block &other);
    void shrink_to_fit();

  private:
    void change_capacity(size_t capacity);

    uint8_t *_data = nullptr;
    size_t _capacity = 0;
    size_t _size = 0;
  };
}
