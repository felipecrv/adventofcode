#include <algorithm>
#include <cassert>
#include <cstdio>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "lib.h"

#define NORTH Vec(0, -1)
#define SOUTH Vec(0, 1)
#define WEST Vec(-1, 0)
#define EAST Vec(1, 0)

Vec cardinals[] = {
    NORTH, EAST, SOUTH, WEST,
};

using WorldPos = std::pair<Vec, int>;
using Portal = std::pair<std::string, Vec>;

struct Donut {
  Donut() { memset(_map, 0, sizeof(_map)); }

#define MAXN 250

  void findExits(int width, int height) {
    char p[3];
    p[2] = 0;

    for (int y = 1; y < height; y++) {
      for (int x = 1; x < width; x++) {
        char c = _map[y][x];
        if (!isalpha(c)) {
          continue;
        }

        auto &chosen = (x >= width - 2 || y >= height - 3 || y == 1 || x == 1)
                           ? outer
                           : inner;

        if (y == 1) {
          p[0] = _map[0][x];
          p[1] = _map[1][x];
          chosen.emplace_back(p, Vec(x, 2));
          continue;
        }

        if (x == 1) {
          p[0] = _map[y][0];
          p[1] = _map[y][1];
          chosen.emplace_back(p, Vec(2, y));
          continue;
        }

        char prev_x = _map[y][x - 1];
        if (prev_x && prev_x == '.') {
          p[0] = _map[y][x];
          p[1] = _map[y][x + 1];
          chosen.emplace_back(p, Vec(x - 1, y));
          continue;
        }

        char prev_y = _map[y - 1][x];
        if (prev_y && prev_y == '.') {
          p[0] = _map[y][x];
          p[1] = _map[y + 1][x];
          chosen.emplace_back(p, Vec(x, y - 1));
          continue;
        }

        char next_x = _map[y][x + 1];
        if (next_x && next_x == '.') {
          char p[3];
          p[0] = _map[y][x - 1];
          p[1] = _map[y][x];
          chosen.emplace_back(p, Vec(x + 1, y));
          continue;
        }

        char next_y = _map[y + 1][x];
        if (next_y && next_y == '.') {
          char p[3];
          p[0] = _map[y - 1][x];
          p[1] = _map[y][x];
          chosen.emplace_back(p, Vec(x, y + 1));
          continue;
        }
      }
    }

    for (auto &o : outer) {
      if (o.first == "AA") {
        AA = o.second;
      } else if (o.first == "ZZ") {
        ZZ = o.second;
      }
      for (auto &i : inner) {
        if (o.first == i.first && o.second != i.second) {
          outer_exits[o.second] = i.second;
          inner_exits[i.second] = o.second;
        }
      }
    }
  }

  int findShortestPath(bool multi_worlds) {
    std::unordered_set<WorldPos> visited;
    std::unordered_map<WorldPos, int> dist;
    std::queue<WorldPos> q;
    std::vector<WorldPos> border;

    WorldPos source(AA, 0); // 0 is the outermost world
    visited.insert(source);
    dist[source] = 0;
    q.push(source);

    while (!q.empty()) {
      auto v = q.front();
      q.pop();

      if (v == std::make_pair(ZZ, 0)) {
        // found ZZ in the outermost world
        return dist[v];
      }

      border.clear();
      for (auto &dir : cardinals) {
        border.emplace_back(v.first + dir, v.second);
      }
      // edge to outer worlds
      if (v.second > 0) {
        if (Vec *exit = lookup(outer_exits, v.first)) {
          border.emplace_back(*exit, multi_worlds ? v.second - 1 : 0);
        }
      }
      // edge to inner worlds
      if (Vec *exit = lookup(inner_exits, v.first)) {
        border.emplace_back(*exit, multi_worlds ? v.second + 1 : 0);
      }

      for (auto &w : border) {
        char c = _map[w.first.y][w.first.x];
        if (c != '.' || contains(visited, w)) {
          continue;
        }
        visited.insert(w);
        dist[w] = dist[v] + 1;
        q.push(w);
      }
    }

    assert(false);
    return 0x3f3f3f3f;
  }

  void render(int width, int height) {
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        putchar(_map[y][x]);
      }
      putchar('\n');
    }
    putchar('\n');
  }

  Vec AA;
  Vec ZZ;
  std::vector<Portal> inner;
  std::vector<Portal> outer;
  std::unordered_map<Vec, Vec> inner_exits;
  std::unordered_map<Vec, Vec> outer_exits;
  char _map[MAXN][MAXN];
};

Donut donut;

int main() {
  int width = 0;
  int height = 0;
  for (int x = 0, y = 0;;) {
    char c = getchar();
    if (c == EOF) {
      break;
    }
    if (c == '\n') {
      y += 1;
      x = 0;
      continue;
    }

    width = std::max(width, x + 1);
    height = std::max(height, y + 1);

    donut._map[y][x] = c;
    x += 1;
  }

  donut.findExits(width, height);
  donut.render(width, height);

  // change to false for first phase of the problem
  bool multi_worlds = true;
  int dist = donut.findShortestPath(multi_worlds);
  printf("Distance AA -> ZZ = %d\n", dist);

  return 0;
}
