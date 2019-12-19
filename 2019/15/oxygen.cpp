#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "intcode.h"
#include "lib.h"

#define MAXN 200

Vec moves[] = {
    Vec(0, -1), // NORTH
    Vec(0, 1),  // SOUTH
    Vec(-1, 0), // WEST
    Vec(1, 0),  // EAST
};

Vec nextPos(Vec cur, int command) { return cur + moves[command - 1]; }

int directionToCommand(Vec dir) {
  assert(dir.lengthSquared() == 1);
  for (int i = 0; i < 4; i++) {
    if (moves[i] == dir) {
      return i + 1;
    }
  }
  assert(false);
  return 0;
}

#define WALL 0
#define SPACE 1
#define OXYGEN 2

struct State {
  State() { reset(Vec(100, 100)); }

  Vec pos() const { return droid_pos; }

  int &cell(int x, int y) {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < MAXN);
    assert(y < MAXN);
    int &val = map[y][x];
    min_x = std::min(min_x, x);
    min_y = std::min(min_y, y);
    max_x = std::max(max_x, x);
    max_y = std::max(max_y, y);
    return val;
  }

  int &cell(Vec pos) { return cell(pos.x, pos.y); }

  bool isExplored(Vec pos) { return cell(pos) < 3; }

  void update(Vec next_pos, int status) {
    switch (status) {
    case WALL:
      break;
    case OXYGEN:
    case SPACE:
      droid_pos = next_pos;
      break;
    }

    // Update the information about the position the robot tried to move
    cell(next_pos) = status;
  }

  void render() {
    for (int y = min_y; y <= max_y; y++) {
      for (int x = min_x; x <= max_x; x++) {
        if (Vec(x, y) == pos()) {
          putchar('D');
          continue;
        }
        switch (cell(x, y)) {
        case WALL:
          putchar('#');
          break;
        case SPACE:
          putchar('.');
          break;
        case OXYGEN:
          putchar('O');
          break;
        default:
          putchar(' ');
        }
      }
      putchar('\n');
    }
    putchar('\n');
  }

  void reset(Vec pos) {
    droid_pos = pos;

    min_x = droid_pos.x;
    min_y = droid_pos.y;
    max_x = droid_pos.x;
    max_y = droid_pos.y;

    memset(map, 0x3f, sizeof(map));
    cell(droid_pos) = SPACE;
  }

private:
  Vec droid_pos;
  int min_x;
  int min_y;
  int max_x;
  int max_y;
  int map[MAXN][MAXN];
};

struct Droid {
  explicit Droid(Program program) : _program(std::move(program)) {
    _cpu.loadProgram(_program);
  }

  int update(int command) {
    // Tell the robot to move and query the status
    _cpu.pushInput(command);
    _cpu.runUntilOutput();
    int status = _cpu.consumeOutput();

    const Vec next_pos = nextPos(state.pos(), command);
    state.update(next_pos, status);

    return status;
  }

  void render() {
    state.render();
    int ms = 11;
    usleep(ms * 1000);
  }

  void backtrack(Vec b) {
    Vec a = state.pos();
    // move from a to b
    if (a == b) {
      return;
    }

    Vec ai = a;
    Vec bi = b;
    std::vector<Vec> path_b; // path from b to ancestor (not including ancestor)
    // find common ancestor
    for (;;) {
      int dist_ai = _dist[ai];
      int dist_bi = _dist[bi];
      if (dist_ai < dist_bi) {
        path_b.push_back(bi);
        bi = _parent[bi];
      } else if (dist_ai > dist_bi) {
        ai = _parent[ai];
      } else {
        if (ai == bi) {
          break;
        }
        path_b.push_back(bi);
        ai = _parent[ai];
        bi = _parent[bi];
      }
    }
    assert(ai == bi);
    Vec ancestor = ai;

    // move to common ancestor
    while (state.pos() != ancestor) {
      Vec parent = _parent[state.pos()];
      Vec dir = parent - state.pos();
      int command = directionToCommand(dir);
      int status = update(command);
      assert(status == SPACE || status == OXYGEN);
    }

    // turn path_b into path from ancestor to b (not including ancestor)
    std::reverse(path_b.begin(), path_b.end());
    for (auto bi : path_b) {
      Vec dir = bi - state.pos();
      int command = directionToCommand(dir);
      int status = update(command);
      assert(status == SPACE || status == OXYGEN);
    }
    assert(state.pos() == b);
  }

  Vec explore() {
    Vec oxygen;

    std::queue<Vec> q;
    Vec pos = state.pos();
    q.push(pos);
    _parent[pos] = pos;
    _dist[pos] = 0;
    while (!q.empty()) {
      Vec pos = q.front();
      q.pop();

      for (int command = 1; command <= 4; command++) {
        Vec next_pos = nextPos(pos, command);
        if (state.isExplored(next_pos)) {
          continue;
        }

        backtrack(pos); // go back to pos

        int status = update(command);

        switch (status) {
        case WALL:
          // nothing to do
          break;
        case SPACE:
        case OXYGEN:
          q.push(next_pos);
          _parent[next_pos] = pos;
          _dist[next_pos] = _dist[pos] + 1;

          if (status == 2) {
            oxygen = state.pos();
          }

          render();
          break;
        }
      }
    }

    return oxygen;
  }

  int fill(Vec oxygen) {
    std::unordered_map<Vec, int> dist;
    int max_dist = 0;

    std::queue<Vec> q;
    q.push(oxygen);

    dist[oxygen] = 0;
    while (!q.empty()) {
      Vec pos = q.front();
      q.pop();

      for (int command = 1; command <= 4; command++) {
        Vec next_pos = nextPos(pos, command);
        int status = state.cell(next_pos);
        switch (status) {
        case WALL:
        case OXYGEN:
          break;
        case SPACE:
          q.push(next_pos);
          dist[next_pos] = dist[pos] + 1;
          max_dist = std::max(max_dist, dist[pos] + 1);
          state.cell(next_pos) = 2;

          render();
          break;
        }
      }
    }

    return max_dist;
  }

  void restart(Vec start_pos) {
    state.reset(start_pos);
    _parent.clear();
    _dist.clear();

    _cpu.clearState();
    _cpu.loadProgram(_program);
  }

private:
  State state;
  std::unordered_map<Vec, Vec> _parent; // parent in the exploration BFS
  std::unordered_map<Vec, int> _dist;   // exploration BFS distance

  Program _program;
  CPU _cpu;
};

int main() {
  Program program;
  Word code;
  while (scanf("%lld", &code)) {
    program.push_back(code);
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  Droid droid(std::move(program));

  Vec oxygen = droid.explore();
  printf("Oxygen found at (%d, %d)\n", oxygen.x, oxygen.y);

  int max_dist = droid.fill(oxygen);

  printf("Oxygen found at (%d, %d)\n", oxygen.x, oxygen.y);
  printf("Time to fill with oxygen oxygen %d\n", max_dist);

  return 0;
}
