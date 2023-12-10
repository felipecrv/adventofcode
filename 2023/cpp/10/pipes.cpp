#include <cassert>
#include <cstdio>
#include <queue>
#include <set>
#include <vector>

#include "lib.h"

int n = 0, m = 0;
char map[200][200];
Vec start;

template <typename F>
inline void ForEachConn(int x, int y, F &&on_conn) {
  const char tile = map[y][x];
  switch (tile) {
    // '|' is a vertical pipe connecting north and south.
    case '|':
      if (y < n) on_conn(x, y + 1);  // south
      if (y > 0) on_conn(x, y - 1);  // north
      break;
    // '-' is a horizontal pipe connecting east and west.
    case '-':
      if (x < m) on_conn(x + 1, y);  // east
      if (x > 0) on_conn(x - 1, y);  // west
      break;
    // 'L' is a 90-degree bend connecting north and east.
    case 'L':
      if (y > 0) on_conn(x, y - 1);  // north
      if (x < m) on_conn(x + 1, y);  // east
      break;
    // 'J' is a 90-degree bend connecting north and west.
    case 'J':
      if (x > 0) on_conn(x - 1, y);  // west
      if (y > 0) on_conn(x, y - 1);  // north
      break;
    // '7' is a 90-degree bend connecting south and west.
    case '7':
      if (x > 0) on_conn(x - 1, y);  // west
      if (y < n) on_conn(x, y + 1);  // south
      break;
    // 'F' is a 90-degree bend connecting south and east.
    case 'F':
      if (x < m) on_conn(x + 1, y);  // east
      if (y < n) on_conn(x, y + 1);  // south
      break;
    // '.' is ground; there is no pipe in this tile.
    case '.':
      break;
    // 'S' is the starting position of the animal; there is a pipe on this
    //     tile, but your sketch doesn't show what shape the pipe has.
    case 'S':
      if (x > 0) on_conn(x - 1, y);  // west
      if (y < n) on_conn(x, y + 1);  // south
      if (x < m) on_conn(x + 1, y);  // east
      if (y > 0) on_conn(x, y - 1);  // north
      break;
  }
}

bool IsConnTowards(int x0, int y0, int x1, int y1) {
  bool confirmed = false;
  ForEachConn(x0, y0, [x1, y1, &confirmed](int conn_x, int conn_y) {
    if (conn_x == x1 && conn_y == y1) {
      // printf("  %d,%d -> %d,%d\n", x, y, conn_x, conn_y);
      confirmed = true;
    }
  });
  return confirmed;
}

template <typename F>
void ForEachNeigh(int x, int y, F &&on_neigh) {
  ForEachConn(x, y,
              [x, y, on_neigh = std::move(on_neigh)](int conn_x, int conn_y) {
                if (IsConnTowards(conn_x, conn_y, x, y)) {
                  on_neigh(conn_x, conn_y);
                }
              });
}

void print_map() {
  auto tile_to_cstr = [](char c) -> const char * {
    switch (c) {
      case 'F':
        return "┏";
      case '7':
        return "┓";
      case 'J':
        return "┛";
      case 'L':
        return "┗";
      case '|':
        return "┃";
      case '-':
        return "━";
    }
    return nullptr;
  };
  for (int y = 0; y < n; y++) {
    for (int x = 0; x < m; x++) {
      const char tile = map[y][x];
      const char *cstr = tile_to_cstr(tile);
      if (cstr) {
        printf("%s", cstr);
      } else {
        putchar(tile);
      }
    }
    putchar('\n');
  }
  putchar('\n');
}

std::optional<Vec> first_neigh_in_cycle(int x, int y,
                                        const std::set<Vec> &cycle,
                                        const std::vector<Vec> &all_so_far) {
  std::optional<Vec> ret;
  ForEachNeigh(x, y,
               [x, y, &ret, &cycle, &all_so_far](int neigh_x, int neigh_y) {
                 auto neigh = Vec{neigh_x, neigh_y};
                 if (!ret.has_value() && contains(cycle, neigh) &&
                     !linearContains(all_so_far, neigh)) {
                   assert(neigh != (Vec{x, y}));
                   ret = neigh;
                 }
               });
  return ret;
}

std::vector<Vec> orient_corners(const std::set<Vec> &cycle) {
  auto pos = start;
  std::vector<Vec> all_so_far;
  std::vector<Vec> ret;
  for (;;) {
    all_so_far.push_back(pos);
    int x = pos.x;
    int y = pos.y;
    char c = map[y][x];
    const bool is_corner =
        c == 'S' || c == 'F' || c == 'L' || c == '7' || c == 'J';
    if (is_corner) {
      ret.push_back({x, y});
    }
    auto next_pos = first_neigh_in_cycle(x, y, cycle, all_so_far);
    if (!next_pos.has_value()) {
      break;
    }
    pos = *next_pos;
    // printf("-  %d,%d -> %d,%d\n", x, y, pos.first, pos.second);
    if (linearContains(all_so_far, pos)) break;
  }
  return ret;
}

int main() {
  bool first_row = true;
  int x = 0;
  int y = 0;
  for (;;) {
    const char c = getchar();
    switch (c) {
      case '\n':
        if (first_row && m == 0) {
          m = x;
          first_row = false;
        }
        x = 0;
        y += 1;
        break;
      case EOF:
        goto done;
      case 'S':
        start = {x, y};
      default:
        map[y][x] = c;
        x += 1;
        break;
    }
  }
done:
  n = y;

  struct State {
    Vec pos;
    int dist;

    State(Vec pos, int dist) : pos(pos), dist(dist) {}
  };

  std::queue<State> q;
  q.push(State(start, 0));
  std::set<Vec> visited;
  while (!q.empty()) {
    auto v = q.front();
    int x = v.pos.x;
    int y = v.pos.y;
    int dist = v.dist;
    q.pop();
    if (visited.count({x, y})) {
      printf("already visited %d,%d (dist = %d)\n", x, y, dist);
      auto &cycle = visited;
      auto corners = orient_corners(cycle);
      for (int y = 0; y < n; y++) {
        for (int x = 0; x < m; x++) {
          if (!contains(cycle, {x, y})) {
            map[y][x] = '.';
          }
        }
      }
      print_map();

      int shoelace = 0;
      corners.push_back(corners[0]);
      for (int i = 0; i < corners.size() - 1; i++) {
        auto [x0, y0] = corners[i];
        auto [x1, y1] = corners[i + 1];
        shoelace += (y0 + y1) * (x0 - x1);
      }
      assert(shoelace % 2 == 0);
      auto area = (shoelace - (int)visited.size() + 3) / 2;
      printf("area = %d\n", area);
      break;
    }
    visited.insert({x, y});
    ForEachNeigh(x, y, [&, dist](int neigh_x, int neigh_y) {
      assert(map[neigh_y][neigh_x] != '.');
      if (visited.count({neigh_x, neigh_y}) == 0) {
        q.emplace(Vec{neigh_x, neigh_y}, dist + 1);
      }
    });
  }

  return 0;
}
