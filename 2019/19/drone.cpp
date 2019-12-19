#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>

#include "intcode.h"

struct Vec {
  Vec() : x(0), y(0) {}

  Vec(int x, int y) : x(x), y(y) {}

  Vec operator+(Vec b) const { return Vec(x + b.x, y + b.y); }

  bool operator==(const Vec &other) const {
    return x == other.x && y == other.y;
  }

  int lengthSquared() const { return x * x + y * y; }

  int x;
  int y;
};

const int SHIP_SIZE = 100;
const int MAXN = 50;

struct Drone {
  explicit Drone(Program program) : _program(std::move(program)) {}

  int explore() {
    int total = 0;
    for (int y = 0; y < MAXN; y++) {
      for (int x = 0; x < MAXN; x++) {
        int out = readPosition(Vec(x, y));
        if (out) {
          if (x == MAXN - 1) {
            A = std::min(A, y);
            B = std::max(B, y);
          }

          total += out;
          putchar('#');
        } else {
          putchar('.');
        }
      }
      putchar('\n');
    }
    putchar('\n');

    return total;
  }

  int readPosition(Vec pos) {
    _cpu.clearState();
    _cpu.loadProgram(_program);

    _cpu.pushInput(pos.x);
    _cpu.pushInput(pos.y);
    _cpu.runUntilOutput();
    return _cpu.consumeOutput();
  }

  bool shipFits(Vec pos) {
    return readPosition(pos + Vec(SHIP_SIZE - 1, 0)) &&
           readPosition(pos + Vec(0, SHIP_SIZE - 1));
  }

  Vec _findPositionForShip() const {
    // the math here is wrong (it gives answers that are off by ~40)
    double x = (A + MAXN - 1) * SHIP_SIZE / (double)(B - A);
    double y = (B + MAXN - 1) * A * SHIP_SIZE / (double)((MAXN - 1) * (B - A));
    return Vec(round(x), round(y));
  }

  Vec findPositionForShip() {
    const int DELTA = 60;

    const Vec pos = _findPositionForShip();

    // look in the neighborhood because the math in _findPositionForShip() is
    // not perfect
    Vec best = pos;
    for (int y = pos.y - DELTA; y < pos.y + DELTA; y++) {
      for (int x = pos.x - DELTA; x < pos.x + DELTA; x++) {
        if (x < 0 || y < 0)
          continue;
        Vec candidate(x, y);
        if (candidate.lengthSquared() < best.lengthSquared() &&
            shipFits(candidate)) {
          best = candidate;
        }
      }
    }

    return best;
  }

private:
  int A = 0x3f3f3f3f; // smallest y at east border
  int B = 0;          // biggest y at east border

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

  Drone drone(std::move(program));

  int total = drone.explore();
  printf("Total in %dx%d area %d\n", MAXN, MAXN, total);

  Vec pos = drone.findPositionForShip();
  printf("Ship position: (%d, %d), x*10000+y = %d\n", pos.x, pos.y,
         pos.x * 10000 + pos.y);

  return 0;
}
