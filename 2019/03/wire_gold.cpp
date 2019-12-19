#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>
#include <unordered_map>
#include <vector>

#include "lib.h"

using namespace std;

#define MAXD 20000
#define POSITIVE '+'
#define NEGATIVE '-'

Vec direction(char d) {
  switch (d) {
  case 'U':
    return Vec(0, 1);
  case 'R':
    return Vec(1, 0);
  case 'D':
    return Vec(0, -1);
  case 'L':
    return Vec(-1, 0);
  }
  assert(false);
  return Vec();
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

  char &cell(Vec p) { return cell(p.x, p.y); }

  void drawFromInput(char wire) {
    _cursor = Vec(0, 0);

    for (;;) {
      char d;
      int steps;
      scanf("%c%d", &d, &steps);
      char c = getchar();
      Vec dir = direction(d);
      drawSegment(wire, dir, steps);
      if (c == '\n' || c == EOF) {
        break;
      } else {
        assert(c == ',');
      }
    }
  }

  void drawSegment(char wire, Vec dir, int steps) {
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
  Vec _cursor;
  vector<Vec> _crosses;
  unordered_map<Vec, int> _total_steps_negative;
  unordered_map<Vec, int> _total_steps_positive;
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
