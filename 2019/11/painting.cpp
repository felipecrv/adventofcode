#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "intcode.h"

#define MAXN 1000
char grid[MAXN][MAXN];

#define BACKGROUND '\0'
#define BLACK '.'
#define WHITE '#'

char turnLeft(char dir) {
  switch (dir) {
    case '^':
      return '<';
    case '>':
      return '^';
    case 'v':
      return '>';
    case '<':
      return 'v';
  }
  return dir;
}

char turnRight(char dir) {
  switch (dir) {
    case '^':
      return '>';
    case '>':
      return 'v';
    case 'v':
      return '<';
    case '<':
      return '^';
  }
  return dir;
}

struct Robot {
  explicit Robot(Program program) : cpu(std::move(program)) {}

  int currentColorToInput() {
    char c = currentColor();
    if (c == BLACK || c == BACKGROUND) {
      return 0;
    }
    assert(c == WHITE);
    return 1;
  }

  int run(bool first_white) {
    while (cpu.status != HALTED) {
      cpu.pushInput(first_white ? 1 : currentColorToInput());
      first_white = false;

      // Run and get paint command output
      cpu.runUntilOutput();
      if (cpu.status == HALTED) {
        break;
      }
      auto paint_command = cpu.consumeOutput();
      if (paint_command == 0) {
        paint(BLACK);
      } else {
        assert(paint_command == 1);
        paint(WHITE);
      }

      // Run and get turn command output
      cpu.runUntilOutput();
      if (cpu.status == HALTED) {
        break;
      }
      auto turn = cpu.consumeOutput();
      if (turn == 0) {
        direction = turnLeft(direction);
      } else {
        assert(turn == 1);
        direction = turnRight(direction);
      }
      move();
    }

    return paint_area;
  }

  void move() {
    switch (direction) {
      case '^':
        pos_i -= 1;
        break;
      case '>':
        pos_j += 1;
        break;
      case 'v':
        pos_i += 1;
        break;
      case '<':
        pos_j -= 1;
        break;
    }
    min_i = std::min(min_i, pos_i);
    min_j = std::min(min_j, pos_j);
    max_i = std::max(max_i, pos_i);
    max_j = std::max(max_j, pos_j);
  }

  char &currentColor() {
    assert(pos_i >= 0);
    assert(pos_i < MAXN);
    assert(pos_j >= 0);
    assert(pos_j < MAXN);
    return grid[pos_i][pos_j];
  }

  void paint(char color) {
    if (currentColor() == BACKGROUND) {
      paint_area += 1;
    }
    currentColor() = color;
  }

  void display() {
    putchar('\n');
    for (int i = min_i; i <= max_i; i++) {
      for (int j = min_j; j <= max_j; j++) {
        char color = grid[i][j];
        if (color == BLACK || color == BACKGROUND) {
          putchar(' ');
        } else if (color == WHITE) {
          putchar('#');
        }
      }
      putchar('\n');
    }
    putchar('\n');
  }

  CPU cpu;
  int pos_i = 100;
  int pos_j = 100;
  int min_i = pos_i;
  int min_j = pos_j;
  int max_i = pos_i;
  int max_j = pos_j;
  int paint_area = 0;
  char direction = '^';
};

int main() {
  memset(grid, BACKGROUND, sizeof(grid));

  Program program;
  Word opcode;
  while (scanf("%lld", &opcode)) {
    program.push_back(opcode);
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  Robot robot(std::move(program));
  bool first_white = true;
  int paint_area = robot.run(first_white);
  robot.display();
  printf("Painted %d area\n", paint_area);

  return 0;
}
