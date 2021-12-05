#pragma once

#include <unordered_map>

struct Pt {
  Pt() : x(0), y(0) {}

  Pt(int x, int y) : x(x), y(y) {}

  Pt &operator+=(Pt other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  bool operator==(const Pt &other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Pt &other) const {
    return x != other.x || y != other.y;
  }

  bool operator>(const Pt &other) const { return x > other.x || y > other.y; }
  bool operator<(const Pt &other) const { return x < other.x || y < other.y; }

  int dot(Pt other) const { return x * other.x + y * other.y; }
  int lengthSquared() const { return x * x + y * y; }
  double length() const { return sqrt((double)lengthSquared()); }

  inline int distSquared(Pt p) const;
  inline double dist(Pt p) const;

  int x;
  int y;
};

inline Pt operator-(const Pt &a, const Pt &b) {
  return Pt(a.x - b.x, a.y - b.y);
}
inline Pt operator+(const Pt &a, const Pt &b) {
  return Pt(a.x + b.x, a.y + b.y);
}

inline int Pt::distSquared(Pt p) const { return ((*this) - p).lengthSquared(); }
inline double Pt::dist(Pt p) const { return sqrt((double)distSquared(p)); }

inline Pt direction(char d) {
  switch (d) {
    case 'U':
      return Pt(0, 1);
    case 'R':
      return Pt(1, 0);
    case 'D':
      return Pt(0, -1);
    case 'L':
      return Pt(-1, 0);
  }
  assert(false);
  return Pt();
}

struct Line {
  Line(Pt a, Pt b) {
    // y-a.y = (b.y-a.y) / (b.x-a.x) * (x-a.x)
    // (a.y-b.y) * x + (b.x-a.x) * y + (a.x-b.x)*a.y + (b.y-a.y)*a.x = 0

    this->a = a.y - b.y;
    this->b = b.x - a.x;
    this->c = (a.x - b.x) * a.y + (b.y - a.y) * a.x;
  }

  bool straight() const { return a == 0 || b == 0; }

  bool contains(Pt p) const {
    int val = a * p.x + b * p.y + c;
    return val == 0;
  }

  int a;
  int b;
  int c;
};

namespace std {

template <>
struct hash<Pt> {
  size_t operator()(const Pt &k) const {
    std::hash<int> hasher;
    return hasher(k.x) ^ hasher(k.y);
  }
};

}  // namespace std
