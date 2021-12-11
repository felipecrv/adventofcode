#include <algorithm>
#include <cassert>
#include <cstdio>
#include <queue>
#include <unordered_set>

#include "../geom.h"  // for Pt
#include "../lib.h"   // for contains

const int MAX_DIM = 1024;
int height, width;
int map[MAX_DIM][MAX_DIM];

void init() {
  height = 0;
  width = 0;
  memset(map, 0, sizeof(map));
}

int &at(int x, int y) { return map[y][x]; }
int &at(Pt p) { return at(p.x, p.y); }

void dump(int step) {
  printf("After step %d:\n", step);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      printf("%d", at(Pt(x, y)));
    }
    putchar('\n');
  }
  putchar('\n');
}

void simulate(int num_steps) {
  bool simult_flashed_before = false;
  long long num_flashes = 0;

  std::unordered_set<Pt> flashed;
  std::queue<Pt> q;

  int step = 0;
  // dump(step);
  for (step = 1;; step++) {
    // re-use structures' buffers
    flashed.clear();
    assert(q.empty());

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        Pt p(x, y);
        if (++at(p) > 9) {    // increase energy for all
          flashed.insert(p);  // flash the ones with enough energy
          q.push(p);          // enqueue for energy sharing
        }
      }
    }

    // share the energy
    while (!q.empty()) {
      const Pt v = q.front();
      q.pop();

      // for all neighbors w: give energy to them
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          const Pt w = v + Pt(dx, dy);
          if (w == v || !w.withinBox(width, height)) continue;

          // increase energy for neighbors that haven't been flashed
          if (at(w) >= 9 && !contains(flashed, w)) {
            flashed.insert(w);  // flash them
            q.push(w);          // push for energy sharing
          } else {
            at(w) += 1;
          }
        }
      }
    }

    // clear the flashed
    for (auto p : flashed) {
      at(p) = 0;
    }
    // count the number of flashes so far
    num_flashes += flashed.size();
    // dump(step);

    const bool simult_flashed = flashed.size() == width * height;
    if (simult_flashed && !simult_flashed_before) {
      printf("all flashed simultaneosuly at step %d\n", step);
      if (step >= num_steps) break;
      simult_flashed_before = true;
    }

    if (step == num_steps) {
      printf("number of flashes after %d steps: %lld\n", step, num_flashes);
      if (simult_flashed_before) break;
    }
  }
}

int main() {
  init();

  int y = 0;
  int x = 0;
  for (;;) {
    char c = getchar();
    if (c == '\n' || c == EOF) {
      width = std::max(x, width);
      y += 1;
      x = 0;
      if (c == EOF) break;
    } else {
      at(x, y) = c - '0';
      x += 1;
    }
  }
  height = y - 1;
  assert(height <= MAX_DIM && width <= MAX_DIM);

  simulate(100);
  return 0;
}
