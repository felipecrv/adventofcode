#pragma once

#include <cassert>
#include <unordered_map>

// Useful classes

struct Vec {
  Vec() : x(0), y(0) {}

  Vec(int x, int y) : x(x), y(y) {}

  Vec &operator+=(Vec other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vec &operator-=(Vec other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  Vec operator+(Vec b) const { return Vec(x + b.x, y + b.y); }
  Vec operator-(Vec b) const { return Vec(x - b.x, y - b.y); }

  bool operator==(const Vec &other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Vec &other) const { return !(*this == other); }

  bool isNull() const { return x == 0 && y == 0; }
  int lengthSquared() const { return x * x + y * y; }

  int x;
  int y;
};

// STL helpers

template <typename MapType>
typename MapType::mapped_type *lookup(MapType &m,
                                      const typename MapType::key_type &k) {
  auto it = m.find(k);
  if (it != m.end()) {
    return &it->second;
  }
  return nullptr;
}

template <typename T> void hash_combine(std::size_t &seed, const T &val) {
  seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// Hashers

namespace std {

template <> struct hash<Vec> {
  size_t operator()(const Vec &v) const {
    size_t h = 0;
    hash_combine(h, v.x);
    hash_combine(h, v.y);
    return h;
  }
};

template <typename A, typename B> struct hash<std::pair<A, B>> {
  size_t operator()(const std::pair<A, B> &k) const {
    size_t h = 0;
    hash_combine(h, k.first);
    hash_combine(h, k.second);
    return h;
  }
};

} // namespace std

// Math functions

long long gcd(long long a, long long b) {
  if (a == 0)
    return b;
  return gcd(b % a, a);
}

long long lcm(long long a, long long b) { return (a * b) / gcd(a, b); }
