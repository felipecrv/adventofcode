#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <utility>
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

  int x;
  int y;
};

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
    if (!(i >= 0 && i < MAXD + 1 + MAXD)) {
      printf("%d\n", i);
      assert(false);
    }
    if (!(j >= 0 && j < MAXD + 1 + MAXD)) {
      printf("%d\n", j);
      assert(false);
    }
    return _grid[i][j];
  }

  char &cell(Pt p) { return cell(p.x, p.y); }

  void drawFromInput(char wire) {
    _cursor = Pt(0, 0);

    char d;
    int steps;
    while (true) {
      char d = getchar();
      scanf("%d", &steps);
      char c = getchar();
      printf("%c%d\n", d, steps);
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
    for (int k = 0; k < steps; k++) {
      if (cell(_cursor) != '.' && cell(_cursor) != wire) {
        _crosses.push_back(_cursor);
        cell(_cursor) = 'x';
      } else {
        cell(_cursor) = wire;
      }
      _cursor += dir;
    }
  }

  void print(int n) {
    for (int y = n; y > -n; y--) {
      for (int x = -n; x < n; x++) {
        putchar(cell(x, y));
      }
      putchar('\n');
    }
  }

  char _grid[MAXD + 1 + MAXD][MAXD + 1 + MAXD];
  Pt _cursor;
  std::vector<Pt> _crosses;
};

Grid grid;

int main() {
  grid.drawFromInput(POSITIVE);
  grid.print(10);
  grid.drawFromInput(NEGATIVE);
  grid.print(10);
  int min_dist = std::numeric_limits<int>::max();
  for (auto cross : grid._crosses) {
    if (cross.x == 0 && cross.y == 0) {
      continue;
    }
    min_dist = min(abs(cross.x) + abs(cross.y), min_dist);
  }
  printf("min_dist: %d\n", min_dist);

  return 0;
}
