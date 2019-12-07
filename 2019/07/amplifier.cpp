#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <vector>

using namespace std;

#define MAXN 100000
static int data[MAXN];

struct VM {
  VM(const int *mem, int len) : _len(len) {
    clearState();
    for (int i = 0; i < len; i++) {
      _mem.push_back(mem[i]);
    }
  }

  void run() {
    assert(!has_output);
    for (;;) {
      decodeAndExecute();
      if (did_halt || has_output) {
        return;
      }
    }
  }

  void decodeAndExecute() {
    clearRegisters();

    // decode
    int op = deref(pc);
    opcode = op % 100;
    switch (opcode) {
      case 1:  // ADD
      case 2:  // MUL
      case 7:  // LT
      case 8:  // EQ
        mode0 = (op % 1000) / 100;
        mode1 = (op % 10000) / 1000;
        mode2 = op / 10000;

        fetchArg(mode0, deref(pc + 1), &r0);
        fetchArg(mode1, deref(pc + 2), &r1);
        fetchDestArg(mode2, deref(pc + 3), &r2);
        break;
      case 3:  // IN
        mode0 = (op % 1000) / 100;
        mode1 = -1;
        mode2 = -1;

        fetchDestArg(mode0, deref(pc + 1), &r2);
        break;

      case 4:  // OUT
        mode0 = (op % 1000) / 100;
        mode1 = -1;
        mode2 = -1;

        fetchArg(mode0, deref(pc + 1), &r0);
        break;

      case 5:  // JMP_IF_TRUE
      case 6:  // JMP_IF_FALSE
        mode0 = (op % 1000) / 100;
        mode1 = (op % 10000) / 1000;
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
    switch (opcode) {
      case 1:  // ADD
        *r2 = r0 + r1;
        pc += 4;
        break;
      case 2:  // MUL
        *r2 = r0 * r1;
        pc += 4;
        break;
      case 3: {  // IN
        *r2 = consumeInput();
        printf("IN %d\n", *r2);
        pc += 2;
        break;
      }
      case 4: {
        _out = r0;
        printf("OUT %d\n", _out);
        pc += 2;

        has_output = true;
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
      case 99:  // HLT
        printf("HLT\n");
        pc += 1;
        did_halt = true;
        break;
    }
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
      *out_reg = derefDest(mem_cell_val);
    } else if (mode == 1) {
      assert(false && "dest param will never be in immediate mode");
    } else {
      assert(false && "invalid mode");
    }
  }

  int deref(int addr) {
    // printf("deref[%d]: ", addr);
    assert(addr >= 0 && addr < _len);
    int val = _mem[addr];
    // printf("%d\n", val);
    return val;
  }

  int *derefDest(int addr) {
    // printf("deref[%d]*: ", addr);
    assert(addr >= 0 && addr < _len);
    int *val = &_mem[addr];
    // printf("%d\n", *val);
    return val;
  }

  void pushInput(int value) { _input.push(value); }

  int consumeInput() {
    assert(!_input.empty());
    int value = _input.front();
    _input.pop();
    return value;
  }

  int consumeOutput() {
    assert(has_output);
    has_output = false;
    return _out;
  }

  void clearRegisters() {
    r0 = 0;
    r1 = 0;
    r2 = nullptr;
  }

  void clearState() {
    pc = 0;
    opcode = 0;
    mode0 = -1;
    mode1 = -1;
    mode2 = -1;

    clearRegisters();

    did_halt = false;
    has_output = false;

    _input = queue<int>();
    _out = 0;
  }

  int pc;
  int opcode;
  int mode0;
  int mode1;
  int mode2;

  int r0;
  int r1;
  int *r2;

  bool did_halt;
  bool has_output;

 private:
  queue<int> _input;
  int _out;

  vector<int> _mem;
  int _len;
};

int runAllAmplifiers(const int *prog, int len, vector<int> phases) {
  VM a(prog, len);
  VM b = a;
  VM c = a;
  VM d = a;
  VM e = a;
  VM *vms[] = {&a, &b, &c, &d, &e};

  for (int i = 0; i < 5; i++) {
    vms[i]->pushInput(phases[i]);
  }
  a.pushInput(0);

  int out = 0;
  for (int i = 0; i < 5; i++) {
    VM *vm = vms[i];
    vm->run();
    out = vm->consumeOutput();
    vm->run();
    assert(vm->did_halt);
    if (i < 4) {
      vms[i + 1]->pushInput(out);
    }
  }

  return out;
}

int runAllAmplifiersInLoop(const int *prog, int len, vector<int> phases) {
  VM a(prog, len);
  VM b = a;
  VM c = a;
  VM d = a;
  VM e = a;
  VM *vms[] = {&a, &b, &c, &d, &e};

  for (int i = 0; i < 5; i++) {
    vms[i]->pushInput(phases[i]);
  }
  a.pushInput(0);

  int e_out = 0;
  int out = 0;
  int i = 0;
  for (;;) {
    VM *vm = vms[i];
    vm->run();
    if (vm->did_halt) {
      break;
    }
    assert(vm->has_output);
    out = vm->consumeOutput();
    if (vm == &e) {
      e_out = out;
    }
    int next = (i + 1) % 5;
    vms[next]->pushInput(out);
    i = next;
  }

  return e_out;
}

int maximizeAmplifiersThrust(const int *prog, int len, bool in_loop,
                             vector<int> *out_max_phases) {
  auto run_all_amplifiers = in_loop ? runAllAmplifiersInLoop : runAllAmplifiers;

  int max_thrust = 0;
  vector<int> phases;
  if (in_loop) {
    phases = {5, 6, 7, 8, 9};
  } else {
    phases = {0, 1, 2, 3, 4};
  }

  *out_max_phases = phases;
  do {
    int thrust = run_all_amplifiers(data, len, phases);
    if (thrust > max_thrust) {
      *out_max_phases = phases;
      max_thrust = thrust;
    }
  } while (next_permutation(phases.begin(), phases.end()));
  return max_thrust;
}

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

  // 07/in_silverX
  // int thrust = runAllAmplifiers(data, len, {4, 3, 2, 1, 0});
  // 43210
  // int thrust = runAllAmplifiers(data, len, {0, 1, 2, 3, 4});
  // 54321
  // int thrust = runAllAmplifiers(data, len, {1, 0, 4, 3, 2});
  // 65210

  // 07/in_goldX
  // int thrust = runAllAmplifiersInLoop(data, len, {9, 8, 7, 6, 5});
  // 139629729
  // int thrust = runAllAmplifiersInLoop(data, len, {9, 7, 8, 5, 6});
  // 18216
  // printf("thrust == %d\n", thrust);

  vector<int> max_phases;
  // int max_thrust = maximizeAmplifiersThrust(data, len, false, &max_phases);
  int max_thrust = maximizeAmplifiersThrust(data, len, true, &max_phases);
  printf("max_thrust: %d\n", max_thrust);
  printf("config:     %d%d%d%d%d\n", max_phases[0], max_phases[1],
         max_phases[2], max_phases[3], max_phases[4]);

  return 0;
}
