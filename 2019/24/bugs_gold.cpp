#include <unistd.h>
#include <cassert>
#include <cstdio>
#include <unordered_map>

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

//                level -> grid
std::unordered_map<int, uint32_t> grid_at_level;
std::unordered_map<int, uint32_t> tmp_grid_at_level;

bool hasBugOnGrid(uint32_t grid, int i) {
  assert(i >= 0 && i < 25);
  assert(i != 12);
  return (grid & (0x1 << i)) != 0;
}

bool hasBugOnGrid(uint32_t grid, Vec v) {
  assert(v != Vec(2, 2));
  if (v.x < 0 || v.y < 0) return false;
  if (v.x >= 5 || v.y >= 5) return false;
  return hasBugOnGrid(grid, v.y * 5 + v.x);
}

void renderGrid(uint32_t grid) {
  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 5; x++) {
      Vec pos(x, y);
      if (pos == Vec(2, 2)) {
        putchar('?');
      } else if (hasBugOnGrid(grid, pos)) {
        putchar('#');
      } else {
        putchar('.');
      }
    }
    putchar('\n');
  }
  putchar('\n');
  // usleep(200 * 1000);
}

void renderLevels(int uppermost, int lowermost) {
  for (int l = uppermost; l <= lowermost; l++) {
    printf("Depth %d:\n", l);
    renderGrid(grid_at_level[l]);
  }
}

uint32_t gridAtLevel(int level) {
  if (auto *upper_grid = lookup(grid_at_level, level)) {
    return *upper_grid;
  }
  return 0;
}

#define FIRST_ROW_MASK 0x1fu
#define LAST_ROW_MASK (FIRST_ROW_MASK << 20u)
#define FIRST_COL_MASK 0x108421u
#define LAST_COL_MASK (FIRST_COL_MASK << 4u)

int countMasked(uint32_t grid, uint32_t mask) {
  return __builtin_popcount(grid & mask);
}

int countAdjacent(int level, Vec v) {
  uint32_t grid = gridAtLevel(level);

  int count = 0;
  for (auto dir : cardinals) {
    Vec w = v + dir;
    uint32_t upper_grid = gridAtLevel(level - 1);
    if (w.y == -1) {
      if (hasBugOnGrid(upper_grid, Vec(1, 2))) {
        count += 1;
      }
    } else if (w.y == 5) {
      if (hasBugOnGrid(upper_grid, Vec(3, 2))) {
        count += 1;
      }
    } else if (w.x == -1) {
      if (hasBugOnGrid(upper_grid, Vec(2, 1))) {
        count += 1;
      }
    } else if (w.x == 5) {
      if (hasBugOnGrid(upper_grid, Vec(2, 3))) {
        count += 1;
      }
    } else if (w == Vec(2, 2)) {
      uint32_t lower_grid = gridAtLevel(level + 1);
      if (v == Vec(1, 2)) {
        count += countMasked(lower_grid, FIRST_ROW_MASK);
      } else if (v == Vec(2, 3)) {
        count += countMasked(lower_grid, LAST_COL_MASK);
      } else if (v == Vec(3, 2)) {
        count += countMasked(lower_grid, LAST_ROW_MASK);
      } else if (v == Vec(2, 1)) {
        count += countMasked(lower_grid, FIRST_COL_MASK);
      }
    } else if (hasBugOnGrid(grid, w)) {
      count += 1;
    }
  }

  return count;
}

uint32_t iterateGrid(int level, uint32_t grid) {
  uint32_t new_grid = 0;
  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 5; x++) {
      if (x == 2 && y == 2) continue;
      Vec pos(x, y);
      bool new_value = false;
      if (hasBugOnGrid(grid, pos)) {
        new_value = countAdjacent(level, pos) == 1;
      } else {
        int adj = countAdjacent(level, pos);
        new_value = (adj == 1 || adj == 2);
      }
      if (new_value) {
        new_grid |= (0x1 << (pos.y * 5 + pos.x));
      }
    }
  }
  return new_grid;
}

void iterate(int uppermost, int lowermost) {
  tmp_grid_at_level = grid_at_level;
  for (auto &[level, grid] : tmp_grid_at_level) {
    grid = iterateGrid(level, grid);
  }
  std::swap(tmp_grid_at_level, grid_at_level);
}

int countBugs() {
  int count = 0;
  for (auto [level, grid] : grid_at_level) {
    count += __builtin_popcount(grid);
  }
  return count;
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

  grid_at_level[0] = grid;
  int uppermost = 0;
  int lowermost = 0;
  renderLevels(uppermost, lowermost);

  const int MINUTES = 200;
  for (int i = 1; i <= MINUTES; i++) {
    grid_at_level[--uppermost] = 0;
    grid_at_level[++lowermost] = 0;
    iterate(uppermost, lowermost);
    // printf("After %d minutes:\n", i);
    // renderLevels(uppermost, lowermost);
    // putchar('\n');
  }

  printf("count bugs: %d\n", countBugs());

  return 0;
}
