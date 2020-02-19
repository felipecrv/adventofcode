#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <vector>

#define ADD 1
#define MUL 2
#define IN 3
#define OUT 4
#define JMP_IF_TRUE 5
#define JMP_IF_FALSE 6
#define LT 7
#define EQ 8
#define HLT 99

using Program = std::vector<int>;

struct VM {
  explicit VM(Program program) : _mem(std::move(program)) { clearState(); }

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

    int mode0 = -1;
    int mode1 = -1;
    int mode2 = -1;

    // decode
    int opcode = deref(pc);
    op = opcode % 100;
    switch (op) {
      case ADD:
      case MUL:
      case LT:
      case EQ:
        mode0 = (opcode % 1000) / 100;
        mode1 = (opcode % 10000) / 1000;
        mode2 = opcode / 10000;

        fetchArg(mode0, deref(pc + 1), &r0);
        fetchArg(mode1, deref(pc + 2), &r1);
        fetchDestArg(mode2, deref(pc + 3), &r2);
        break;
      case IN:
        mode0 = (opcode % 1000) / 100;

        fetchDestArg(mode0, deref(pc + 1), &r2);
        break;
      case OUT:
        mode0 = (opcode % 1000) / 100;

        fetchArg(mode0, deref(pc + 1), &r0);
        break;
      case JMP_IF_TRUE:
      case JMP_IF_FALSE:
        mode0 = (opcode % 1000) / 100;
        mode1 = (opcode % 10000) / 1000;

        fetchArg(mode0, deref(pc + 1), &r0);
        fetchArg(mode1, deref(pc + 2), &r1);
        break;
      case HLT:
        break;
    }

    // execute
    switch (op) {
      case ADD:
        *r2 = r0 + r1;
        pc += 4;
        break;
      case MUL:
        *r2 = r0 * r1;
        pc += 4;
        break;
      case IN:
        *r2 = consumeInput();
        printf("IN %d\n", *r2);
        pc += 2;
        break;
      case OUT: {
        _out = r0;
        printf("OUT %d\n", _out);
        pc += 2;

        has_output = true;
        break;
      }
      case JMP_IF_TRUE:
        if (r0 != 0) {
          pc = r1;
        } else {
          pc += 3;
        }
        break;
      case JMP_IF_FALSE:
        if (r0 == 0) {
          pc = r1;
        } else {
          pc += 3;
        }
        break;
      case LT:
        *r2 = r0 < r1 ? 1 : 0;
        pc += 4;
        break;
      case EQ:
        *r2 = r0 == r1 ? 1 : 0;
        pc += 4;
        break;
      case HLT:
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
    assert(addr >= 0 && addr < _mem.size());
    int val = _mem[addr];
    return val;
  }

  int *derefDest(int addr) {
    assert(addr >= 0 && addr < _mem.size());
    int *val = &_mem[addr];
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
    op = 0;

    clearRegisters();

    did_halt = false;
    has_output = false;

    _input = std::queue<int>();
    _out = 0;
  }

  int pc;
  int op;

  int r0;
  int r1;
  int *r2;

  bool did_halt;
  bool has_output;

 private:
  std::queue<int> _input;
  int _out;

  Program _mem;
};

int runAllAmplifiers(Program program, std::vector<int> phases) {
  std::vector<VM> vms;
  vms.emplace_back(std::move(program));
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);

  for (int i = 0; i < 5; i++) {
    vms[i].pushInput(phases[i]);
  }
  vms[0].pushInput(0);

  int out = 0;
  for (int i = 0; i < 5; i++) {
    VM &vm = vms[i];
    vm.run();
    out = vm.consumeOutput();
    vm.run();
    assert(vm.did_halt);
    if (i < 4) {
      vms[i + 1].pushInput(out);
    }
  }

  return out;
}

int runAllAmplifiersInLoop(Program program, std::vector<int> phases) {
  std::vector<VM> vms;
  vms.emplace_back(std::move(program));
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);
  vms.push_back(vms[0]);
  auto *e = &vms[4];

  for (int i = 0; i < 5; i++) {
    vms[i].pushInput(phases[i]);
  }
  vms[0].pushInput(0);

  int e_out = 0;
  int out = 0;
  int i = 0;
  for (;;) {
    VM &vm = vms[i];
    vm.run();
    if (vm.did_halt) {
      break;
    }
    assert(vm.has_output);
    out = vm.consumeOutput();
    if (&vm == e) {
      e_out = out;
    }
    int next = (i + 1) % 5;
    vms[next].pushInput(out);
    i = next;
  }

  return e_out;
}

int maximizeAmplifiersThrust(Program program, bool in_loop,
                             std::vector<int> *out_max_phases) {
  auto run_all_amplifiers = in_loop ? runAllAmplifiersInLoop : runAllAmplifiers;

  int max_thrust = 0;
  std::vector<int> phases;
  if (in_loop) {
    phases = {5, 6, 7, 8, 9};
  } else {
    phases = {0, 1, 2, 3, 4};
  }

  *out_max_phases = phases;
  do {
    int thrust = run_all_amplifiers(program, phases);
    if (thrust > max_thrust) {
      *out_max_phases = phases;
      max_thrust = thrust;
    }
  } while (next_permutation(phases.begin(), phases.end()));
  return max_thrust;
}

int main() {
  Program data;
  int code;
  while (scanf("%d", &code)) {
    data.push_back(code);
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  // 07/in_silverX
  // int thrust = runAllAmplifiers(data, {4, 3, 2, 1, 0});
  // 43210
  // int thrust = runAllAmplifiers(data, {0, 1, 2, 3, 4});
  // 54321
  // int thrust = runAllAmplifiers(data, {1, 0, 4, 3, 2});
  // 65210

  // 07/in_goldX
  // int thrust = runAllAmplifiersInLoop(data, {9, 8, 7, 6, 5});
  // 139629729
  // int thrust = runAllAmplifiersInLoop(data, {9, 7, 8, 5, 6});
  // 18216
  // printf("thrust == %d\n", thrust);

  std::vector<int> max_phases;
  int max_thrust =
      maximizeAmplifiersThrust(data, /* in_loop */ true, &max_phases);
  printf("max_thrust: %d\n", max_thrust);
  printf("config:     %d%d%d%d%d\n", max_phases[0], max_phases[1],
         max_phases[2], max_phases[3], max_phases[4]);

  return 0;
}
