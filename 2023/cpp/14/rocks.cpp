#include <cassert>
#include <cstdio>
#include <optional>
#include <vector>

#include "lib.h"

struct XAxis {
  int size;
  int &operator()(Vec &v) { return v.x; }
};

struct YAxis {
  int size;
  int &operator()(Vec &v) { return v.y; }
};

template <class Axis>
struct Forward {
  Axis axis;
  Forward(Axis axis) : axis(axis) {}
  void start(Vec &v) { axis(v) = 0; }
  void next(Vec &v) { axis(v) += 1; }
  bool done(Vec &v) { return axis(v) >= axis.size; }
};

template <class Axis>
struct Backward {
  Axis axis;
  Backward(Axis axis) : axis(axis) {}
  void start(Vec &v) { axis(v) = axis.size - 1; }
  void next(Vec &v) { axis(v) -= 1; }
  bool done(Vec &v) { return axis(v) < 0; }
};

struct Grid {
  static const int MAX_N = 100;
  int n = 0, m = 0;
  char map[MAX_N][MAX_N];

  void Clear() { n = m = 0; }

  static bool ParseGrid(Grid *grid) {
    grid->Clear();
    int x = 0;
    int y = 0;
    for (;;) {
      char c = getchar();
      switch (c) {
        case '\n':
          if (x == 0) {
            goto done_parsing;
          }
          if (y == 0 && grid->m == 0) {
            grid->m = x;
          }
          x = 0;
          y += 1;
          break;
        case 'O':
        case '.':
        case '#':
          grid->map[y][x] = c;
          x += 1;
          break;
        case EOF:
          if (y == 0) {
            return false;
          }
          goto done_parsing;
        default:
          assert(false);
      }
    }
  done_parsing:
    grid->n = y;
    return true;
  }

  char &operator[](Vec pos) { return map[pos.y][pos.x]; }

  void PrintMap() {
    for (int y = 0; y < n; y++) {
      for (int x = 0; x < m; x++) putchar(map[y][x]);
      putchar('\n');
    }
    putchar('\n');
  }

  size_t Hash() {
    size_t h = 0;
    std::hash<std::string_view> sv_hasher;
    for (int y = 0; y < n; y++) {
      hash_combine(h, sv_hasher(std::string_view(map[y], m)));
    }
    return h;
  }

  int TotalNorthLoad() {
    int sum = 0;
    for (int y = 0; y < n; y++) {
      for (int x = 0; x < m; x++) {
        if (map[y][x] == 'O') sum += n - y;
      }
    }
    return sum;
  }

  // clang-format off
  std::pair<Backward<XAxis>, Forward<YAxis>>  North() { return {Backward{XAxis{m}}, Forward{YAxis{n}}}; }
  std::pair<Forward<XAxis>,  Backward<YAxis>> South() { return {Forward{XAxis{m}},  Backward{YAxis{n}}}; }
  std::pair<Backward<YAxis>, Forward<XAxis>>  West()  { return {Backward{YAxis{n}}, Forward{XAxis{m}}}; }
  std::pair<Forward<YAxis>,  Backward<XAxis>> East()  { return {Forward{YAxis{n}},  Backward{XAxis{m}}}; }
  // clang-format on

  template <class Direction>
  void Tilt(Direction dir) {
    auto outer = dir.first;
    auto inner = dir.second;
    Vec pos0;
    for (outer.start(pos0); !outer.done(pos0); outer.next(pos0)) {
      for (inner.start(pos0); !inner.done(pos0); inner.next(pos0)) {
        Vec delta;
        Vec pos1 = pos0;
        for (; !inner.done(pos1); inner.next(pos1)) {
          char c = (*this)[pos1];
          if (c == 'O') {
            (*this)[pos1] = '.';
            (*this)[pos0 + delta] = 'O';
            inner.next(delta);
          } else if (c == '.') {
            continue;
          } else if (c == '#') {
            break;
          }
        }
        inner.axis(pos0) = inner.axis(pos1);
      }
    }
  }

  int TiltNorth() {
    Tilt(North());
    return TotalNorthLoad();
  }

  int NWSECycle() {
    Tilt(North());
    Tilt(West());
    Tilt(South());
    Tilt(East());
    return TotalNorthLoad();
  }
};

template <class T>
std::optional<int> find(const std::vector<T> &v, T val) {
  auto it = std::find(v.begin(), v.end(), val);
  if (it == v.end()) return std::nullopt;
  return it - v.begin();
}

int main() {
  Grid grid;
  Grid::ParseGrid(&grid);
  {
    Grid grid0 = grid;
    int sum = grid0.TiltNorth();
    printf("Part 1: %d\n", sum);
  }
  int sum;
  std::vector<size_t> grid_hashes;
  for (int i = 0;; i++) {
    sum = grid.NWSECycle();
    size_t grid_hash = grid.Hash();
    auto prev = find(grid_hashes, grid_hash);
    if (prev) {
      int n = i + (1000000000 - *prev - 1) % (i - *prev);
      for (; i < n; i++) {
        sum = grid.NWSECycle();
      }
      break;
    }
    grid_hashes.push_back(grid_hash);
  }
  printf("Part 2: %d\n", sum);
  return 0;
}
