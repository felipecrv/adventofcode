#include <cassert>
#include <cstdio>
#include <queue>
#include <stack>
#include <set>
#include <vector>

#include "lib.h"

int n = 0, m = 0;
char map[200][200];
Vec start;

// The first on_conn that returns true will cause ForEachConn to stop iteration
// over connections and return.
template <typename F>
inline bool ForEachConn(int x, int y, F &&on_conn) {
  const char tile = map[y][x];
  // Alway iterate over connection in counter-clockwise order: W, S, E, N.
  switch (tile) {
    case '|':
      // '|' is a vertical pipe connecting south and north.
      return (y < n && on_conn(x, y + 1)) || (y > 0 && on_conn(x, y - 1));
    case '-':
      // '-' is a horizontal pipe connecting west and east.
      return (x > 0 && on_conn(x - 1, y)) || (x < m && on_conn(x + 1, y));
    case 'L':
      // 'L' is a 90-degree bend connecting east and north.
      return (x < m && on_conn(x + 1, y)) || (y > 0 && on_conn(x, y - 1));
    case 'J':
      // 'J' is a 90-degree bend connecting west and north.
      return (x > 0 && on_conn(x - 1, y)) || (y > 0 && on_conn(x, y - 1));
    case '7':
      // '7' is a 90-degree bend connecting west and south.
      return (x > 0 && on_conn(x - 1, y)) || (y < n && on_conn(x, y + 1));
    case 'F':
      // 'F' is a 90-degree bend connecting east and south.
      return (x < m && on_conn(x + 1, y)) || (y < n && on_conn(x, y + 1));
    case '.':
      // '.' is ground; there is no pipe in this tile.
      break;
    case 'S':
      // 'S' is the starting position of the animal; there is a pipe on this
      //     tile, but your sketch doesn't show what shape the pipe has.
      return (x > 0 && on_conn(x - 1, y)) ||  // West
             (y < n && on_conn(x, y + 1)) ||  // South
             (x < m && on_conn(x + 1, y)) ||  // East
             (y > 0 && on_conn(x, y - 1));    // North
  }
  return false;
}

template <typename F>
bool ForEachNeigh(int x, int y, F &&on_neigh) {
  return ForEachConn(
      x, y,
      [x, y, on_neigh = std::move(on_neigh)](int conn_x, int conn_y) -> bool {
        const bool is_conn_towards =
            ForEachConn(conn_x, conn_y, [x, y](int conn_x, int conn_y) {
              return conn_x == x && conn_y == y;
            });
        if (is_conn_towards) {
          return on_neigh(conn_x, conn_y);
        }
        return false;  // continue
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
                   return true;  // break
                 }
                 return false;  // continue
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

  std::stack<State> s;
  s.push(State(start, 0));
  std::set<Vec> visited;
  while (!s.empty()) {
    auto v = s.top();
    int x = v.pos.x;
    int y = v.pos.y;
    int dist = v.dist;
    s.pop();
    puts(":");
    if (v.pos == start && visited.count(start)) {
      // XXX: extract
      const int half_dist = (dist + 1) / 2;
      printf("already visited %d,%d (dist = %d, half-dist = %d)\n", x, y, dist, half_dist);
      auto &cycle = visited;
      puts("orienting corners...");
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
    if (visited.count(v.pos)) {
      continue;
    }
    visited.insert(v.pos);
    ForEachNeigh(x, y, [&, dist](int neigh_x, int neigh_y) {
      auto neigh = Vec{neigh_x, neigh_y};
      assert(map[neigh_y][neigh_x] != '.');
      if (visited.count(neigh) == 0) {
        s.emplace(neigh, dist + 1);
      }
      // XXX: continue here
      // if (neigh == start) {
      // }
      return false;  // continue
    });
  }

  return 0;
}
