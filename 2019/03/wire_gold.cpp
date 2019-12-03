#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>
#include <unordered_map>
#include <vector>

using namespace std;

#define MAXD 20000
#define POSITIVE '+'
#define NEGATIVE '-'

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

  int x;
  int y;
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

Pt direction(char d) {
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

struct Grid {
  Grid() : _cursor(0, 0) { memset(_grid, '.', sizeof(_grid)); }

  char &cell(int x, int y) {
    int i = y + MAXD;
    int j = x + MAXD;
    if (i < 0 || i > 2 * MAXD) {
      printf("%d\n", i);
      assert(false);
    }
    if (j < 0 || i > 2 * MAXD) {
      printf("%d\n", j);
      assert(false);
    }
    return _grid[i][j];
  }

  char &cell(Pt p) { return cell(p.x, p.y); }

  void drawFromInput(char wire) {
    _cursor = Pt(0, 0);

    for (;;) {
      char d;
      int steps;
      scanf("%c%d", &d, &steps);
      char c = getchar();
      Pt dir = direction(d);
      drawSegment(wire, dir, steps);
      if (c == '\n' || c == EOF) {
        break;
      } else {
        assert(c == ',');
      }
    }
  }

  void drawSegment(char wire, Pt dir, int steps) {
    auto &total_steps_map =
        wire == POSITIVE ? _total_steps_positive : _total_steps_negative;

    for (int k = 0; k < steps; k++) {
      auto total_steps = total_steps_map[_cursor];
      _cursor += dir;

      if (cell(_cursor) != '.' && cell(_cursor) != wire) {
        _crosses.push_back(_cursor);
        cell(_cursor) = 'x';
      } else {
        cell(_cursor) = wire;
      }

      total_steps_map[_cursor] = total_steps + 1;
    }
  }

  char _grid[MAXD + 1 + MAXD][MAXD + 1 + MAXD];
  Pt _cursor;
  vector<Pt> _crosses;
  unordered_map<Pt, int> _total_steps_negative;
  unordered_map<Pt, int> _total_steps_positive;
};

Grid grid;

int main() {
  grid.drawFromInput(POSITIVE);
  grid.drawFromInput(NEGATIVE);

  int dist = 0;
  int min_total_steps = numeric_limits<int>::max();

  for (auto cross : grid._crosses) {
    if (cross.x == 0 && cross.y == 0) {
      continue;
    }
    int total_steps =
        grid._total_steps_negative[cross] + grid._total_steps_positive[cross];
    if (total_steps < min_total_steps) {
      min_total_steps = total_steps;
      dist = abs(cross.x) + abs(cross.y);
    }
  }
  printf("dist: %d\n", dist);
  printf("min_total_steps: %d\n", min_total_steps);

  return 0;
}
