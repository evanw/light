#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "light_vector.h"

using namespace light;

static int count = 0;

struct I {
  I() { count++; }
  I(int value) : value(value) { count++; }
  I(const I &i) : value(i.value) { count++; }
  ~I() { count--; }

  I &operator =(const I &i) { value = i.value; return *this; }
  bool operator ==(const I &i) const { return value == i.value; }
  bool operator !=(const I &i) const { return !(*this == i); }

  int value = 0;
};

TEST_CASE("empty and size") {
  REQUIRE(count == 0);
  {
    vector<I> v;
    REQUIRE(v.empty());
    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 0);

    for (int i = 0; i < 100; i++) {
      v.emplace_back();
      REQUIRE(!v.empty());
      REQUIRE(v.size() == i + 1);
      REQUIRE(v.capacity() >= v.size());
    }
  }
  REQUIRE(count == 0);
}

TEST_CASE("data") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    I d[] = {1, 2, 3};
    REQUIRE(!memcmp(v.data(), d, sizeof(d)));
  }
  REQUIRE(count == 0);
}

TEST_CASE("default constructor") {
  REQUIRE(count == 0);
  {
    vector<I> v(10);
    REQUIRE(v.size() == 10);
    for (int i = 0; i < 10; i++) {
      REQUIRE(v[i] == 0);
    }
  }
  REQUIRE(count == 0);
}

TEST_CASE("assign") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};

    I d[] = {4, 5};
    v.assign(d, d + 2);
    REQUIRE(v.size() == 2);
    REQUIRE(v[0].value == 4);
    REQUIRE(v[1].value == 5);

    I d2[] = {6, 7, 8, 9};
    v.assign(d2, d2 + 4);
    REQUIRE(v.size() == 4);
    REQUIRE(v[0].value == 6);
    REQUIRE(v[1].value == 7);
    REQUIRE(v[2].value == 8);
    REQUIRE(v[3].value == 9);
  }
  REQUIRE(count == 0);
}

TEST_CASE("for loop") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    int index = 0;
    for (auto &i : v) {
      REQUIRE(i.value == ++index);
    }
  }
  REQUIRE(count == 0);
}

TEST_CASE("reverse for loop") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    int index = 0;
    for (auto it = v.rbegin(); it != v.rend(); it++) {
      REQUIRE(it->value == 3 - index++);
    }
  }
  REQUIRE(count == 0);
}

TEST_CASE("initializer list") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    REQUIRE(v.size() == 3);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 2);
    REQUIRE(v[2].value == 3);
  }
  REQUIRE(count == 0);
}

TEST_CASE("copy constructor") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    vector<I> v2 = v;

    v.push_back(4);
    v2.push_back(5);

    REQUIRE(v.size() == 4);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 2);
    REQUIRE(v[2].value == 3);
    REQUIRE(v[3].value == 4);

    REQUIRE(v2.size() == 4);
    REQUIRE(v2[0].value == 1);
    REQUIRE(v2[1].value == 2);
    REQUIRE(v2[2].value == 3);
    REQUIRE(v2[3].value == 5);
  }
  REQUIRE(count == 0);
}

TEST_CASE("move constructor") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    vector<I> v2 = std::move(v);

    v.push_back(4);
    v2.push_back(5);

    REQUIRE(v.size() == 1);
    REQUIRE(v[0].value == 4);

    REQUIRE(v2.size() == 4);
    REQUIRE(v2[0].value == 1);
    REQUIRE(v2[1].value == 2);
    REQUIRE(v2[2].value == 3);
    REQUIRE(v2[3].value == 5);
  }
  REQUIRE(count == 0);
}

TEST_CASE("copy assignment") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    vector<I> v2 = {7, 8};
    v = v;
    v2 = v;

    v.push_back(4);
    v2.push_back(5);

    REQUIRE(v.size() == 4);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 2);
    REQUIRE(v[2].value == 3);
    REQUIRE(v[3].value == 4);

    REQUIRE(v2.size() == 4);
    REQUIRE(v2[0].value == 1);
    REQUIRE(v2[1].value == 2);
    REQUIRE(v2[2].value == 3);
    REQUIRE(v2[3].value == 5);
  }
  REQUIRE(count == 0);
}

TEST_CASE("move assignment") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    vector<I> v2 = {7, 8};
    v = std::move(v);
    v2 = std::move(v);

    v.push_back(4);
    v2.push_back(5);

    REQUIRE(v.size() == 3);
    REQUIRE(v[0].value == 7);
    REQUIRE(v[1].value == 8);
    REQUIRE(v[2].value == 4);

    REQUIRE(v2.size() == 4);
    REQUIRE(v2[0].value == 1);
    REQUIRE(v2[1].value == 2);
    REQUIRE(v2[2].value == 3);
    REQUIRE(v2[3].value == 5);
  }
  REQUIRE(count == 0);
}

TEST_CASE("clear") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    size_t capacity = v.capacity();
    REQUIRE(!v.empty());
    REQUIRE(capacity != 0);
    v.clear();
    REQUIRE(v.empty());
    REQUIRE(v.capacity() == capacity);
  }
  REQUIRE(count == 0);
}

TEST_CASE("insert") {
  REQUIRE(count == 0);
  {
    vector<I> v;
    v.insert(v.begin(), 1);
    v.insert(v.begin() + 1, 3);
    v.insert(v.begin() + 1, 2);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 2);
    REQUIRE(v[2].value == 3);
  }
  REQUIRE(count == 0);
}

TEST_CASE("insert empty range") {
  REQUIRE(count == 0);
  {
    vector<I> v;
    I d[] = {1, 2, 3};
    v.insert(v.begin(), d, d);
    REQUIRE(v.size() == 0);
    v.insert(v.begin(), d, d + 3);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 2);
    REQUIRE(v[2].value == 3);
  }
  REQUIRE(count == 0);
}

TEST_CASE("insert range") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    I d[] = {-1, -2, -3};
    v.insert(v.begin() + 1, d, d + 3);
    REQUIRE(v.size() == 6);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == -1);
    REQUIRE(v[2].value == -2);
    REQUIRE(v[3].value == -3);
    REQUIRE(v[4].value == 2);
    REQUIRE(v[5].value == 3);
  }
  REQUIRE(count == 0);
}

TEST_CASE("erase") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    v.erase(v.begin() + 1);
    REQUIRE(v.size() == 2);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 3);
  }
  REQUIRE(count == 0);
}

TEST_CASE("erase range") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3, 4};
    v.erase(v.begin() + 1, v.begin() + 1);
    REQUIRE(v.size() == 4);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 2);
    REQUIRE(v[2].value == 3);
    REQUIRE(v[3].value == 4);
    v.erase(v.begin() + 1, v.begin() + 3);
    REQUIRE(v.size() == 2);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 4);
  }
  REQUIRE(count == 0);
}

TEST_CASE("operator ==") {
  REQUIRE(count == 0);
  {
    REQUIRE((vector<I> {1, 2, 3}) == (vector<I> {1, 2, 3}));
    REQUIRE((vector<I> {1, 2, 3}) != (vector<I> {1, 2, 3, 4}));
    REQUIRE((vector<I> {1, 2, 3, 4}) != (vector<I> {1, 2, 3}));
    REQUIRE((vector<I> {1, 2, 3}) != (vector<I> {1, 2, 4}));
  }
  REQUIRE(count == 0);
}

TEST_CASE("push_back") {
  REQUIRE(count == 0);
  {
    vector<I> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 2);
    REQUIRE(v[2].value == 3);
  }
  REQUIRE(count == 0);
}

TEST_CASE("pop_back") {
  REQUIRE(count == 0);
  {
    vector<I> v = {1, 2, 3};
    v.pop_back();
    REQUIRE(v.size() == 2);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 2);
  }
  REQUIRE(count == 0);
}

TEST_CASE("emplace_back") {
  REQUIRE(count == 0);
  {
    vector<I> v;
    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 2);
    REQUIRE(v[2].value == 3);
  }
  REQUIRE(count == 0);
}

TEST_CASE("resize") {
  REQUIRE(count == 0);
  {
    vector<I> v;
    v.resize(3, 1);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 1);
    REQUIRE(v[2].value == 1);
    v.resize(2);
    REQUIRE(v.size() == 2);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 1);
    v.resize(4);
    REQUIRE(v.size() == 4);
    REQUIRE(v[0].value == 1);
    REQUIRE(v[1].value == 1);
    REQUIRE(v[2].value == 0);
    REQUIRE(v[3].value == 0);
  }
  REQUIRE(count == 0);
}

TEST_CASE("reserve") {
  REQUIRE(count == 0);
  {
    vector<I> v;
    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 0);
    v.reserve(100);
    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 100);
    v.push_back(1);
    REQUIRE(v.size() == 1);
    REQUIRE(v.capacity() == 100);
  }
  REQUIRE(count == 0);
}
