#include <cassert>
#include <cstdio>
#include <vector>

const int MAX_N = 200;
char map[MAX_N][MAX_N];
int n = 0, m = 0;

struct Galaxy {
  int x, y;

  Galaxy(int x, int y) : x(x), y(y) {}
};

std::vector<Galaxy> galaxies;
int cumm_dist_y[MAX_N];
int cumm_dist_x[MAX_N];

long long SumAllDistances(int space_mult) {
  cumm_dist_y[0] = 0;
  for (int y = 0; y < n; y++) {
    bool row_contains_galaxy = false;
    for (int x = 0; x < m && !row_contains_galaxy; x++) {
      if (map[y][x] == '#') {
        row_contains_galaxy = true;
      }
    }
    cumm_dist_y[y + 1] =
        cumm_dist_y[y] + (row_contains_galaxy ? 1 : space_mult);
  }

  cumm_dist_x[0] = 0;
  for (int x = 0; x < m; x++) {
    bool col_contains_galaxy = false;
    for (int y = 0; y < n && !col_contains_galaxy; y++) {
      if (map[y][x] == '#') {
        col_contains_galaxy = true;
      }
    }
    cumm_dist_x[x + 1] =
        cumm_dist_x[x] + (col_contains_galaxy ? 1 : space_mult);
  }

  long long sum = 0;
  for (int i = 0; i < galaxies.size(); i++) {
    for (int j = i + 1; j < galaxies.size(); j++) {
      auto g0 = galaxies[i];
      auto g1 = galaxies[j];
      int dist = abs(cumm_dist_x[g0.x + 1] - cumm_dist_x[g1.x + 1]) +
                 abs(cumm_dist_y[g0.y + 1] - cumm_dist_y[g1.y + 1]);
      sum += dist;
    }
  }
  return sum;
}

int main() {
  bool first_row = true;
  int x = 0;
  int y = 0;
  for (;;) {
    char c = getchar();
    switch (c) {
      case '\n':
        if (first_row && m == 0) {
          m = x;
          first_row = false;
        }
        x = 0;
        y += 1;
        break;
      case '#':
        galaxies.emplace_back(x, y);
        [[fallthrough]];
      case '.':
        map[y][x] = c;
        x += 1;
        break;
      case EOF:
        goto done_parsing;
      default:
        assert(false);
    }
  }
done_parsing:
  n = y;

  long long sum = SumAllDistances(2);
  printf("Part 1: %lld\n", sum);
  sum = SumAllDistances(1000000);
  printf("Part 2: %lld\n", sum);
  return 0;
}
