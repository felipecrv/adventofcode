#include <unistd.h>
#include <cassert>
#include <cstdio>
#include <unordered_set>

#include "lib.h"

#define NORTH Vec(0, -1)
#define SOUTH Vec(0, 1)
#define WEST Vec(-1, 0)
#define EAST Vec(1, 0)

Vec cardinals[] = {
    NORTH,
    EAST,
    SOUTH,
    WEST,
};

bool hasBugOnGrid(uint32_t grid, int i) {
  assert(i >= 0 && i < 25);
  return (grid & (0x1 << i)) != 0; }

bool hasBugOnGrid(uint32_t grid, Vec v) {
  if (v.x < 0 || v.y < 0) return false;
  if (v.x >= 5 || v.y >= 5) return false;
  return hasBugOnGrid(grid, v.y * 5 + v.x);
}

int countAdjacent(uint32_t grid, Vec v) {
  int count = 0;
  for (auto dir : cardinals) {
    Vec w = v + dir;
    if (hasBugOnGrid(grid, w)) {
      count += 1;
    }
  }
  return count;
}

uint32_t iterateGrid(uint32_t grid) {
  uint32_t new_grid = 0;
  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 5; x++) {
      Vec pos(x, y);
      bool new_value = false;
      if (hasBugOnGrid(grid, pos)) {
        new_value = countAdjacent(grid, pos) == 1;
      } else {
        int adj = countAdjacent(grid, pos);
        new_value = (adj == 1 || adj == 2);
      }
      if (new_value) {
        new_grid |= (0x1 << (pos.y * 5 + pos.x));
      }
    }
  }
  return new_grid;
}

int main() {
  uint32_t grid = 0;
  int y = 0, x = 0;
  for (;;) {
    int c = getchar();
    if (c == EOF) break;
    if (c == '\n') {
      y += 1;
      x = 0;
      continue;
    }
    if (c == '#') {
      grid |= (0x1 << (y * 5 + x));
    }
    x += 1;
  }

  std::unordered_set<uint32_t> seen;
  seen.insert(grid);
  for (;;) {
    grid = iterateGrid(grid);
    if (contains(seen, grid)) {
      printf("Found cycle: %d\n", grid);
      return 0;
    }
    seen.insert(grid);
  }

  return 0;
}
