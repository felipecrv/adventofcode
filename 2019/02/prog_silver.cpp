#include <cassert>
#include <cstdio>
#include <cstring>

const int MAX_MEM = 1000000;

int prog[MAX_MEM];

int main() {
  memset(prog, 0, sizeof(prog));

  int len = 0;
  while (scanf("%d", &prog[len++])) {
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  prog[1] = 12;
  prog[2] = 2;
  bool did_halt = false;

  int pc = 0;
  while (pc < len) {
    int opcode = prog[pc];
    switch (opcode) {
      case 1:
      case 2: {
        int addr_a = prog[pc + 1];
        int addr_b = prog[pc + 2];
        int addr_dest = prog[pc + 3];
        assert(addr_a < len);
        assert(addr_b < len);
        assert(addr_dest < MAX_MEM);
        int a = prog[addr_a];
        int b = prog[addr_b];
        prog[addr_dest] = (opcode == 1) ? a + b : a * b;
        break;
      }
      case 99:
        goto halt;
        break;
    }
    pc += 4;
    continue;

  halt:
    did_halt = true;
    break;
  }

  assert(did_halt);
  printf("%d\n", prog[0]);

  return 0;
}
