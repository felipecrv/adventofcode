#include <cassert>
#include <cstdio>
#include <queue>
#include <set>

struct Vec {
  int x, y;

  Vec(int x, int y) : x(x), y(y) {}

  Vec operator+(Vec b) const { return Vec(x + b.x, y + b.y); }

  bool operator<(const Vec &other) const {
    return x < other.x || (x == other.x && y < other.y);
  }

  bool operator==(const Vec &other) const {
    return x == other.x && y == other.y;
  }
};

enum Dir { N, W, S, E };

struct Grid {
  static const int MAX_N = 110;
  int n = 0, m = 0;
  char map[MAX_N][MAX_N];

  void Clear() { n = m = 0; }

  static bool ParseGrid(Grid *grid) {
    grid->Clear();
    int x = 0;
    int y = 0;
    for (;;) {
      char c = getchar();
      switch (c) {
        case '\n':
          if (x == 0) {
            goto done_parsing;
          }
          if (y == 0 && grid->m == 0) {
            grid->m = x;
          }
          x = 0;
          y += 1;
          break;
        case '|':
        case '-':
        case '/':
        case '\\':
        case '.':
          grid->map[y][x] = c;
          x += 1;
          break;
        case EOF:
          if (y == 0) {
            return false;
          }
          goto done_parsing;
        default:
          assert(false);
      }
    }
  done_parsing:
    grid->n = y;
    return true;
  }

  char &operator[](Vec pos) { return map[pos.y][pos.x]; }

  bool WithinBounds(Vec pos) {
    return pos.x >= 0 && pos.x < m && pos.y >= 0 && pos.y < n;
  }

  const Vec NORTH = Vec{0, -1};
  const Vec WEST = Vec{-1, 0};
  const Vec SOUTH = Vec{0, 1};
  const Vec EAST = Vec{1, 0};

  // clang-format off
  Vec opposite(Dir dir) {
    switch (dir) {
      case N: return SOUTH;
      case W: return EAST;
      case S: return NORTH;
      case E: return WEST;
    }
  }

  void CountEnergized0(std::set<std::pair<Vec, Dir>> &visited, Vec origin, Dir from) {
    assert(WithinBounds(origin));
    std::queue<std::pair<Vec, Dir>> queue;
    queue.emplace(origin, from);

    while (!queue.empty()) {
      auto front = queue.front();
      queue.pop();
      auto [v, from] = front;
      if (!WithinBounds(v) ||visited.count(front)) continue;
      visited.insert(front);
      const char c = (*this)[v];
      if (c == '.') {
        queue.emplace(v + opposite(from), from);
      } else if (c == '-') {
        switch (from) {
          case N:
          case S:
            queue.emplace(v + EAST, W);
            queue.emplace(v + WEST, E);
            break;
          case W:
          case E:
            queue.emplace(v + opposite(from), from);
            break;
        }
      } else if (c == '|') {
        switch (from) {
          case N:
          case S:
            queue.emplace(v + opposite(from), from);
            break;
          case W:
          case E:
            queue.emplace(v + NORTH, S);
            queue.emplace(v + SOUTH, N);
            break;
        }
      } else if (c == '\\') {
        switch (from) {
          case N: queue.emplace(v + EAST,  W); break;
          case W: queue.emplace(v + SOUTH, N); break;
          case S: queue.emplace(v + WEST,  E); break;
          case E: queue.emplace(v + NORTH, S); break;
        }
      } else if (c == '/') {
        switch (from) {
          case N: queue.emplace(v + WEST,  E); break;
          case W: queue.emplace(v + NORTH, S); break;
          case S: queue.emplace(v + EAST,  W); break;
          case E: queue.emplace(v + SOUTH, N); break;
        }
      } else {
        assert(false && "unreachable");
      }
    }
  }
  // clang-format on

  int CountEnergized(Vec origin, Dir from) {
    std::set<std::pair<Vec, Dir>> visited;
    CountEnergized0(visited, origin, from);
    auto it = visited.begin();
    if (it == visited.end()) return 0;
    auto pos = it->first;
    int count = 1;
    for (++it; it != visited.end(); ++it) {
      if (pos != it->first) {
        pos = it->first;
        count += 1;
      }
    }
    return count;
  }
};

int main() {
  Grid grid;
  Grid::ParseGrid(&grid);

  int count1 = grid.CountEnergized(Vec{0, 0}, W);
  printf("Part 1: %d\n", count1);

  int count2 = 0;
  for (int x = 0; x < grid.m; x++) {
    count2 =
        std::max(count2, std::max(grid.CountEnergized(Vec{x, 0}, N),
                                  grid.CountEnergized(Vec{x, grid.n - 1}, S)));
  }
  for (int y = 0; y < grid.n; y++) {
    count2 =
        std::max(count2, std::max(grid.CountEnergized(Vec{0, y}, W),
                                  grid.CountEnergized(Vec{grid.m - 1, y}, E)));
  }
  printf("Part 2: %d\n", count2);
  return 0;
}
