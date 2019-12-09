#include <cassert>
#include <cstdio>
#include <cstring>

#define MAXN 100000
static int data[MAXN];

struct VM {
  VM(int *mem, int len) : _mem(mem), _len(len) {}

  void run() {
    while (decodeAndExecute()) {
      ;
    }
  }

  bool decodeAndExecute() {
    clearRegisters();

    int opcode = deref(pc);
    op = opcode % 100;
    switch (op) {
      case 1:  // ADD
      case 2:  // MUL
      case 7:  // LT
      case 8:  // EQ
        mode0 = (opcode % 1000) / 100;
        mode1 = (opcode % 10000) / 1000;
        mode2 = opcode / 10000;

        fetchArg(mode0, deref(pc + 1), &r0);
        fetchArg(mode1, deref(pc + 2), &r1);
        fetchDestArg(mode2, deref(pc + 3), &r2);
        break;
      case 3:  // IN
        mode0 = (opcode % 1000) / 100;
        mode1 = -1;
        mode2 = -1;

        fetchDestArg(mode0, deref(pc + 1), &r2);
        break;

      case 4:  // OUT
        mode0 = (opcode % 1000) / 100;
        mode1 = -1;
        mode2 = -1;

        fetchArg(mode0, deref(pc + 1), &r0);
        break;

      case 5:  // JMP_IF_TRUE
      case 6:  // JMP_IF_FALSE
        mode0 = (opcode % 1000) / 100;
        mode1 = (opcode % 10000) / 1000;
        mode2 = -1;

        fetchArg(mode0, deref(pc + 1), &r0);
        fetchArg(mode1, deref(pc + 2), &r1);
        break;

      case 99:  // HLT
        mode0 = -1;
        mode1 = -1;
        mode2 = -1;
        break;
    }

    // execute
    bool ret = true;
    switch (op) {
      case 1:  // ADD
        printf("ADD\n");
        *r2 = r0 + r1;
        pc += 4;
        break;
      case 2:  // MUL
        printf("MUL\n");
        *r2 = r0 * r1;
        pc += 4;
        break;
      case 3: {  // IN
        printf("IN\n");
        int scanned = scanf("%d", r2);
        assert(scanned == 1);
        pc += 2;
        break;
      }
      case 4: {
        printf("OUT\n");
        printf("%d\n", r0);
        pc += 2;
        break;
      }
      case 5:  // JMP_IF_TRUE
        if (r0 != 0) {
          pc = r1;
        } else {
          pc += 3;
        }
        break;
      case 6:  // JMP_IF_FALSE
        if (r0 == 0) {
          pc = r1;
        } else {
          pc += 3;
        }
        break;
      case 7:  // LT
        *r2 = r0 < r1 ? 1 : 0;
        pc += 4;
        break;
      case 8:  // EQ
        *r2 = r0 == r1 ? 1 : 0;
        pc += 4;
        break;
      case 99:
        printf("HLT\n");
        pc += 1;
        ret = false;
        break;
    }

    return ret;
  }

  void fetchArg(int mode, int mem_cell_val, int *out_reg) {
    if (mode == 0) {  // pos
      *out_reg = deref(mem_cell_val);
    } else if (mode == 1) {  // immediate
      *out_reg = mem_cell_val;
    } else {
      assert(false && "invalid mode");
    }
  }

  void fetchDestArg(int mode, int mem_cell_val, int **out_reg) {
    if (mode == 0) {
      *out_reg = deref_dest(mem_cell_val);
    } else if (mode == 1) {
      assert(false && "dest param will never be in immediate mode");
    } else {
      assert(false && "invalid mode");
    }
  }

  int deref(int addr) {
    assert(addr >= 0 && addr < _len);
    int val = _mem[addr];
    printf("deref[%d]: %d\n", addr, val);
    return val;
  }

  int *deref_dest(int addr) {
    assert(addr >= 0 && addr < _len);
    int *val = &_mem[addr];
    printf("deref[%d]*: %d\n", addr, *val);
    return val;
  }

  int pc = 0;
  int op;
  int mode0 = -1;
  int mode1 = -1;
  int mode2 = -1;

  int r0 = 0;
  int r1 = 0;
  int *r2 = nullptr;

  void clearRegisters() {
    r0 = 0;
    r1 = 0;
    r2 = nullptr;
  }

 private:
  int *_mem;
  int _len;
};

int main() {
  int len = 0;
  while (scanf("%d", &data[len++])) {
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  VM vm(data, len);
  vm.run();

  printf("mem[0] == %d\n", vm.deref(0));

  return 0;
}
