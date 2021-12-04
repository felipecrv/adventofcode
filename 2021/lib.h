#pragma once

#include <cassert>
#include <numeric>  // for std::__gcd
#include <string>
#include <unordered_map>
#include <vector>

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

  Vec operator-() const { return Vec(-x, -y); }

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

template <typename Container>
bool contains(const Container &s, const typename Container::value_type &v) {
  return s.find(v) != s.end();
}

template <typename Container>
bool linearContains(const Container &s,
                    const typename Container::value_type &v) {
  return std::find(s.begin(), s.end(), v) != s.end();
}

template <typename T>
void hash_combine(std::size_t &seed, const T &val) {
  seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// Hashers

namespace std {

template <>
struct hash<Vec> {
  size_t operator()(const Vec &v) const {
    size_t h = 0;
    hash_combine(h, v.x);
    hash_combine(h, v.y);
    return h;
  }
};

template <typename A, typename B>
struct hash<std::pair<A, B>> {
  size_t operator()(const std::pair<A, B> &k) const {
    size_t h = 0;
    hash_combine(h, k.first);
    hash_combine(h, k.second);
    return h;
  }
};

template <>
struct hash<vector<string>> {
  size_t operator()(const vector<string> &v) const {
    size_t h = 0;
    for (const auto &s : v) {
      hash_combine(h, s);
    }
    return h;
  }
};

}  // namespace std

// String

inline std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> pieces;

  std::string cur;
  for (char c : s) {
    if (c == delim) {
      if (!cur.empty()) {
        pieces.emplace_back(std::move(cur));
      }
    } else {
      cur += c;
    }
  }
  if (!cur.empty()) {
    pieces.emplace_back(std::move(cur));
  }

  return pieces;
}

// Math

inline long long gcd(long long a, long long b) {
  if (a == 0) return b;
  return gcd(b % a, a);

  // return (long long)std::gcd((unsigned long long)a, (unsigned long long)b);
}

inline long long lcm(long long a, long long b) { return (a * b) / gcd(a, b); }

struct Frac {
  Frac(long long num = 0, long long den = 1) { set(num, den); }

  void set(long long num_, long long den_) {
    num = num_ / gcd(num_, den_);
    den = den_ / gcd(num_, den_);
    if (den < 0) {
      num *= -1;
      den *= -1;
    }
  }

  void operator*=(Frac f) { set(num * f.num, den * f.den); }
  void operator+=(Frac f) { set(num * f.den + f.num * den, den * f.den); }
  void operator-=(Frac f) { set(num * f.den - f.num * den, den * f.den); }
  void operator/=(Frac f) { set(num * f.den, den * f.num); }

  long long num;
  long long den;
};

/* clang-format off */
inline bool operator==(Frac a, Frac b) { return a.num * b.den == b.num * a.den; }
inline bool operator!=(Frac a, Frac b) { return !(a == b); }
inline bool operator<(Frac a, Frac b) { return a.num * b.den < b.num * a.den; }
inline bool operator<=(Frac a, Frac b) { return (a == b) || (a < b); }
inline bool operator>(Frac a, Frac b) { return !(a <= b); }
inline bool operator>=(Frac a, Frac b) { return !(a < b); }
inline Frac operator/(Frac a, Frac b) { Frac ret = a; ret /= b; return ret; }
inline Frac operator*(Frac a, Frac b) { Frac ret = a; ret *= b; return ret; }
inline Frac operator+(Frac a, Frac b) { Frac ret = a; ret += b; return ret; }
inline Frac operator-(Frac a, Frac b) { Frac ret = a; ret -= b; return ret; }
inline Frac operator-(Frac f) { return 0 - f; }
/* clang-format on */
