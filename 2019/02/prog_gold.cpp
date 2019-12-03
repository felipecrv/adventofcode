#include <cassert>
#include <cstdio>
#include <cstring>

int run(int *mem, int len, int noun, int verb) {
  mem[1] = noun;
  mem[2] = verb;
  bool did_halt = false;

  int pc = 0;
  while (pc < len) {
    int opcode = mem[pc];
    switch (opcode) {
      case 1:
      case 2: {
        int addr_a = mem[pc + 1];
        int addr_b = mem[pc + 2];
        int addr_dest = mem[pc + 3];
        assert(addr_a < len);
        assert(addr_b < len);
        assert(addr_dest < len);
        int a = mem[addr_a];
        int b = mem[addr_b];
        mem[addr_dest] = (opcode == 1) ? a + b : a * b;
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
  return mem[0];
}

int main() {
  const int MAX_MEM = 1000;

  int prog[MAX_MEM];
  int mem[MAX_MEM];

  memset(prog, 0, sizeof(prog));
  memset(mem, 0, sizeof(mem));

  int len = 0;
  while (scanf("%d", &prog[len++])) {
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  for (int noun = 0; noun < 100; noun++) {
    for (int verb = 0; verb < 100; verb++) {
      memcpy(mem, prog, MAX_MEM * sizeof(mem[0]));
      int result = run(mem, len, noun, verb);
      if (result == 19690720) {
        printf("%d %d -> %d\n", noun, verb, result);
        break;
      }
    }
  }

  return 0;
}
