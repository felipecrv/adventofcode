#include <unistd.h>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "intcode.h"
#include "lib.h"

using string = std::string;
using Commands = std::vector<std::string>;

#define NORTH Vec(0, -1)
#define SOUTH Vec(0, 1)
#define WEST Vec(-1, 0)
#define EAST Vec(1, 0)

#define NE (NORTH + EAST)
#define SE (SOUTH + EAST)
#define SW (SOUTH + WEST)
#define NW (NORTH + WEST)

Vec cardinals[] = {
    NORTH,
    EAST,
    SOUTH,
    WEST,
};

Vec collaterals[] = {
    NE,
    SE,
    SW,
    NW,
};

int directionToAscii(Vec dir) {
  if (dir == NORTH) return '^';
  if (dir == EAST) return '>';
  if (dir == SOUTH) return 'v';
  if (dir == WEST) return '<';
  return 'X';
}

Vec asciiToDirection(int c) {
  if (c == '^') return NORTH;
  if (c == '>') return EAST;
  if (c == 'v') return SOUTH;
  if (c == '<') return WEST;
  return Vec();
}

int dirToInt(Vec dir) {
  if (dir == NORTH) return 0;
  if (dir == EAST) return 1;
  if (dir == SOUTH) return 2;
  if (dir == WEST) return 3;
  return -1;
}

int countTurns(Vec a, Vec b) {
  int a_pos = dirToInt(a);
  int b_pos = dirToInt(b);
  if (b_pos >= a_pos) {
    return b_pos - a_pos;
  }
  return b_pos + 4 - a_pos;
}

#define MAXN 200

struct Grid {
  Grid() { memset(map, 0, sizeof(map)); }

  int dummy = 0;

  int &set(int x, int y) {
    if (x < 0 || y < 0 || x >= MAXN || y > MAXN) {
      return dummy;
    }
    int &val = map[y][x];
    min_x = std::min(min_x, x);
    min_y = std::min(min_y, y);
    max_x = std::max(max_x, x);
    max_y = std::max(max_y, y);
    return val;
  }

  int &set(Vec pos) { return set(pos.x, pos.y); }

  int get(int x, int y) const {
    if (x < 0 || y < 0 || x > max_x || y > max_y) return 0;
    return map[y][x];
  }

  int get(Vec pos) const { return get(pos.x, pos.y); }

  int sumCalibrationParams() const {
    int sum = 0;
    for (int y = min_y + 1; y <= max_y - 1; y++) {
      for (int x = min_x + 1; x <= max_x - 1; x++) {
        Vec pos = Vec(x, y);
        if (get(pos) == '#') {
          bool is_cross = true;
          for (auto dir : cardinals) {
            if (get(pos + dir) != '#') {
              is_cross = false;
              break;
            }
          }
          for (auto dir : collaterals) {
            if (!is_cross || get(pos + dir) != '.') {
              is_cross = false;
              break;
            }
          }
          if (is_cross) {
            // Found a cross at pos!
            sum += pos.x * pos.y;
          }
        }
      }
    }
    return sum;
  }

  void render(Vec bot_pos, Vec bot_dir) const {
    for (int y = min_y; y <= max_y; y++) {
      for (int x = min_x; x <= max_x; x++) {
        Vec pos(x, y);
        if (pos == bot_pos) {
          putchar(directionToAscii(bot_dir));
          continue;
        }
        int c = get(pos);
        switch (c) {
          case 0:
            putchar(' ');
            break;
          default:
            putchar(c);
            break;
        }
      }
      putchar('\n');
    }
    putchar('\n');
  }

 private:
  int min_x = 0;
  int min_y = 0;
  int max_x = 0;
  int max_y = 0;
  int map[MAXN][MAXN];
};

// encode commands in terms of A, B, and C
string encodeCommands(const Commands &commands, const Commands &A,
                      const Commands &B, const Commands &C) {
  string encoded;

  auto routineName = [&A, &B, &C](const Commands *routine) -> char {
    if (routine == &A) {
      return 'A';
    }
    if (routine == &B) {
      return 'B';
    }
    if (routine == &C) {
      return 'C';
    }
    assert(false);
    return '?';
  };

  std::vector<const Commands *> routines({&A, &B, &C});

  bool found_routine;
  for (int i = 0; i < commands.size();) {
    for (auto *routine : routines) {
      found_routine = true;
      if (i + routine->size() > commands.size()) {
        found_routine = false;
      }
      for (int j = 0; found_routine && j < routine->size(); j++) {
        auto &com = commands[i + j];
        auto &routine_com = (*routine)[j];
        if (com != routine_com) {
          found_routine = false;
        }
      }

      if (found_routine) {
        i += routine->size();

        const char found = routineName(routine);
        if (!encoded.empty()) {
          encoded.push_back(',');
        }
        encoded.push_back(found);
        break;
      }
    }
  }
  return encoded;
}

struct Bot {
  explicit Bot(Program program) : _program(std::move(program)) {
    _cpu.loadProgram(_program);
  }

  void render() const { grid.render(bot_pos, bot_dir); }

  void scan() {
    int y = 0;
    int x = 0;
    while (!_cpu.halted()) {
      _cpu.runUntilOutput();
      if (_cpu.halted()) {
        break;
      }
      int out = _cpu.consumeOutput();
      if (out == '\n') {
        y += 1;
        x = 0;
      } else {
        grid.set(x, y) = out;
        Vec dir = asciiToDirection(out);
        if (dir.lengthSquared() == 1) {
          bot_pos = Vec(x, y);
          bot_dir = dir;
        }
        x += 1;
      }
      // render();
      // int ms = 11;
      // usleep(ms * 1000);
    }
  }

  int sumCalibrationParams() { return grid.sumCalibrationParams(); }

  int runCommands(Commands commands) {
    string A = "R,4,R,12,R,10,L,12";
    string B = "L,12,L,8,R,10";
    string C = "L,12,R,4,R,12";
    // string encoded = encodeCommands(commands, A, B, C);
    // assert(!encoded.empty());
    // printf("encoded: %s\n", encoded.c_str());
    string encoded = "A,C,C,B,B,A,C,C,B,A";  // 19

    // prepare for clean
    _cpu.clearState();
    _program[0] = 2;
    _cpu.loadProgram(_program);

    encoded += '\n';
    _cpu.pushInput(encoded);
    A += '\n';
    _cpu.pushInput(A);
    B += '\n';
    _cpu.pushInput(B);
    C += '\n';
    _cpu.pushInput(C);

    _cpu.pushInput("n\n");  // no video feed
    _cpu.pushInput("\n");

    int last_output = 0;
    for (;;) {
      _cpu.runUntilOutput();
      if (_cpu.halted()) {
        break;
      }
      last_output = _cpu.consumeOutput();
    }

    return last_output;
  }

  int clean() {
    Commands commands;

    Vec cur_dir = bot_dir;
    Vec picked_dir;
    for (;;) {
      bool picked = false;
      for (auto dir : cardinals) {
        if (dir == -picked_dir) {
          continue;
        }
        auto next = bot_pos + dir;
        int c = grid.get(next);
        if (c == '#') {
          picked_dir = dir;
          picked = true;
          break;
        }
      }
      if (!picked) {
        break;
      }

      int move_len = 0;
      for (;;) {
        Vec next = bot_pos + picked_dir;
        int c = grid.get(next);
        if (c == '#') {
          move_len += 1;
          bot_pos = next;
          continue;
        }
        break;
      }
      char turn = 'R';
      int turns = countTurns(cur_dir, picked_dir);
      // if (turns > 2) {
      // turns = 4 - turns;
      // turn = 'L';
      // }
      string com;
      com += turn;
      for (int i = 0; i < turns; i++) {
        commands.push_back(com);
      }
      cur_dir = picked_dir;
      commands.push_back(std::to_string(move_len));
    }

    return runCommands(commands);
  }

 private:
  Grid grid;

  Vec bot_pos;
  Vec bot_dir;

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

  Bot bot(std::move(program));
  bot.scan();
  bot.render();

  int sum = bot.sumCalibrationParams();
  printf("Calibration params: %d\n", sum);

  int last_output = bot.clean();
  printf("last_output: %d\n", last_output);

  return 0;
}
