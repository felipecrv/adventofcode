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

using namespace std;

#define NORTH Vec(0, -1)
#define SOUTH Vec(0, 1)
#define WEST Vec(-1, 0)
#define EAST Vec(1, 0)

Vec cardinals[] = {
    NORTH, EAST, SOUTH, WEST,
};

namespace std {

template <> struct hash<pair<Vec, int>> {
  size_t operator()(const pair<Vec, int> &k) const {
    size_t h = 0;
    hash_combine(h, k.first);
    hash_combine(h, k.second);
    return h;
  }
};

} // namespace std

bool contains(const std::unordered_set<pair<Vec, int>> &s,
              const pair<Vec, int> &v) {
  return s.find(v) != s.end();
}

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

        auto &chosen =
            (x >= width - 2 || y >= height - 3 || y == 1 || x == 1) ? outer
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
        if (prev_x && (prev_x == '.' || prev_x == '#')) {
          p[0] = _map[y][x];
          p[1] = _map[y][x + 1];
          chosen.emplace_back(p, Vec(x - 1, y));
          continue;
        }

        char prev_y = _map[y - 1][x];
        if (prev_y && (prev_y == '.' || prev_y == '#')) {
          p[0] = _map[y][x];
          p[1] = _map[y + 1][x];
          chosen.emplace_back(p, Vec(x, y - 1));
          continue;
        }

        char next_x = _map[y][x + 1];
        if (next_x && (next_x == '.' || next_x == '#')) {
          char p[3];
          p[0] = _map[y][x - 1];
          p[1] = _map[y][x];
          chosen.emplace_back(p, Vec(x + 1, y));
          continue;
        }

        char next_y = _map[y + 1][x];
        if (next_y && (next_y == '.' || next_y == '#')) {
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
      printf("%s\n", o.first.c_str());
      for (auto &i : inner) {
        if (o.first == i.first && o.second != i.second) {
          outer_exits[o.second] = i.second;
          inner_exits[i.second] = o.second;
        }
      }
    }
  }

  int findShortestPath() {
    unordered_set<pair<Vec, int>> visited;
    unordered_map<pair<Vec, int>, int> dist;
    queue<pair<Vec, int>> q;

    pair<Vec, int> source = make_pair(AA, 0); // 0 is the outermost world
    visited.insert(source);
    dist[source] = 0;
    q.push(source);
    while (!q.empty()) {
      auto v = q.front();
      q.pop();

      if (v == make_pair(ZZ, 0)) {
        // found ZZ in the outermost world
        return dist[v];
      }

      for (auto &dir : cardinals) {
        auto w = make_pair(v.first + dir, v.second);
        char c = _map[w.first.y][w.first.x];
        if (c != '.') {
          continue;
        }
        if (contains(visited, w)) {
          continue;
        }
        visited.insert(w);

        dist[w] = dist[v] + 1;

        q.push(w);
      }
      if (v.second > 0) {
        if (Vec *w_vec = lookup(outer_exits, v.first)) {
          // leaving to an outer world
          auto w = make_pair(*w_vec, v.second - 1);

          char c = _map[w_vec->y][w_vec->x];
          assert(c == '.');
          if (contains(visited, w)) {
            continue;
          }
          visited.insert(w);

          dist[w] = dist[v] + 1;

          q.push(w);
        }
      }
      if (Vec *w_vec = lookup(inner_exits, v.first)) {
        // leaving to an inner world
        auto w = make_pair(*w_vec, v.second + 1);

        char c = _map[w_vec->y][w_vec->x];
        assert(c == '.');
        if (contains(visited, w)) {
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
    /*
    for (auto & [ _, pos ] : inner) {
      _map[pos.y][pos.x] = '@';
    }
    for (auto & [ _, pos ] : outer) {
      _map[pos.y][pos.x] = 'X';
    }
    */

    for (int i = 0; i < height; i++) {
      bool printed_something = false;
      for (int j = 0; j < width; j++) {
        if (_map[i][j]) {
          putchar(_map[i][j]);
          printed_something = true;
        }
      }
      if (printed_something)
        putchar('\n');
    }
    putchar('\n');
  }

  Vec AA;
  Vec ZZ;
  vector<pair<string, Vec>> inner;
  vector<pair<string, Vec>> outer;
  unordered_map<Vec, Vec> inner_exits;
  unordered_map<Vec, Vec> outer_exits;
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

    width = max(width, x + 1);
    height = max(height, y + 1);

    donut._map[y][x] = c;
    x += 1;
  }

  donut.findExits(width, height);
  donut.render(width, height);

  int dist = donut.findShortestPath();
  printf("%d\n", dist);

  return 0;
}
