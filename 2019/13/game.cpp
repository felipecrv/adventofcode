#include <unistd.h>
#include <cstdio>
#include <vector>

#include "intcode.h"

#define MAXN 100

#define WALL 1
#define BLOCK 2
#define PADDLE 3
#define BALL 4

struct Vec {
  Vec() : x(0), y(0) {}

  Vec(int x, int y) : x(x), y(y) {}

  Vec &operator+=(Vec other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vec &operator-=(Vec other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  bool operator==(const Vec &other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Vec &other) const {
    return x != other.x || y != other.y;
  }

  bool operator>(const Vec &other) const { return x > other.x || y > other.y; }
  bool operator<(const Vec &other) const { return x < other.x || y < other.y; }

  int dot(Vec other) const { return x * other.x + y * other.y; }
  int lengthSquared() const { return x * x + y * y; }

  int x;
  int y;
};

struct State {
  State() { memset(map, 0, sizeof(map)); }

  int &cell(int x, int y) {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < MAXN);
    assert(y < MAXN);
    int &cell = map[y][x];
    min_x = std::min(min_x, x);
    min_y = std::min(min_y, y);
    max_x = std::max(max_x, x);
    max_y = std::max(max_y, y);
    return cell;
  }

  void render() const {
    // printf("\x1B[0;0H");
    for (int y = min_y; y <= max_y; y++) {
      for (int x = min_x; x <= max_x; x++) {
        switch (map[y][x]) {
          case 0:
            putchar(' ');
            break;
          case WALL:
            putchar('|');
            break;
          case BLOCK:
            putchar('#');
            break;
          case PADDLE:
            putchar('-');
            break;
          case BALL:
            putchar('O');
            break;
        }
      }
      putchar('\n');
    }
    putchar('\n');
  }

  void endFrame() const {
    int ms = 12;
    usleep(ms * 1000);
  }

 private:
  int map[MAXN][MAXN];
  int min_x = 0;
  int min_y = 0;
  int max_x = 0;
  int max_y = 0;
};

struct Game {
  explicit Game(Program program) : cpu(std::move(program)) {}

  void update() {
    cpu.status = RUNNING;
    while (cpu.status != HALTED) {
      for (;;) {
        cpu.decodeAndExecute();
        if (cpu.status == PAUSED) {
          // an IN instruction can pause the CPU
          return;
        }
        if (cpu.op == HLT) {
          cpu.status = HALTED;
          break;
        }
        if (cpu.op == OUT) {
          if (cpu.output().size() == 3) {
            break;
          }
        }
      }
      if (cpu.status == HALTED) {
        break;
      }

      int x = cpu.consumeOutput();
      int y = cpu.consumeOutput();
      int kind = cpu.consumeOutput();
      if (x == -1 && y == 0) {
        score = kind;
        continue;
      }

      int &cell = state.cell(x, y);

      cell = kind;
      if (kind == BALL) {
        auto new_ball_pos = Vec(x, y);
        ball_vel = new_ball_pos;
        ball_vel -= ball_pos;
        ball_pos = new_ball_pos;
      } else if (kind == PADDLE) {
        paddle_pos = Vec(x, y);
      }
    }
  }

  void render() const {
    state.render();
    printf("score %d\n", score);
    state.endFrame();
  }

  bool ballMoving() const {
    return ball_vel.y < 0 ||               // going up
           ball_pos.y < paddle_pos.y - 1;  // going down
  }

  void pushInput(int joystick) { cpu.pushInput(joystick); }

  bool over() const { return cpu.status == HALTED; }

  Vec ball_pos;
  Vec ball_vel;
  Vec paddle_pos;

 private:
  State state;
  int score = 0;
  CPU cpu;
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

  program[0] = 2;  // play for free

  Game game(std::move(program));
  game.update();

  int paddle_target_x = 0;
  for (int known_future = 0; !game.over(); known_future--) {
    if (known_future == 0) {
      Game simulation(game);
      for (; simulation.ballMoving(); known_future++) {
        simulation.pushInput(0);
        simulation.update();
      }
      paddle_target_x = simulation.ball_pos.x;

      known_future += 1;  // for the iteration where the ball touches the paddle
    }
    int joystick = 0;
    if (paddle_target_x > game.paddle_pos.x) {
      joystick = 1;
    } else if (paddle_target_x < game.paddle_pos.x) {
      joystick = -1;
    }

    game.pushInput(joystick);
    game.update();
    game.render();
  }

  return 0;
}
