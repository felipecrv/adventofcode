#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <string>
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

std::vector<int> parseProgram(const std::string &ss) {
  std::vector<int> program;

  const char *s = ss.c_str();
  bool done = false;
  for (int code; !done && sscanf(s, "%d", &code) == 1;) {
    program.push_back(code);
    if (*s == '-' || isspace(*s)) {
      s++;
    }
    while (isdigit(*s)) {
      s++;
    }
    if (*s == ',') {
      s++;
    }
    if (*s == EOF || *s == 0) {
      break;
    }
  }
  return program;
}

struct VM {
  VM(std::vector<int> program) : _mem(std::move(program)) { clearState(); }

  explicit VM(const std::string &program) : VM(parseProgram(program)) {}

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
    int op = deref(pc);
    opcode = op % 100;
    switch (opcode) {
      case ADD:
      case MUL:
      case LT:
      case EQ:
        mode0 = (op % 1000) / 100;
        mode1 = (op % 10000) / 1000;
        mode2 = op / 10000;

        fetchArg(mode0, deref(pc + 1), &r0);
        fetchArg(mode1, deref(pc + 2), &r1);
        fetchDestArg(mode2, deref(pc + 3), &r2);
        break;
      case IN:
        mode0 = (op % 1000) / 100;

        fetchDestArg(mode0, deref(pc + 1), &r2);
        break;
      case OUT:
        mode0 = (op % 1000) / 100;

        fetchArg(mode0, deref(pc + 1), &r0);
        break;
      case JMP_IF_TRUE:
      case JMP_IF_FALSE:
        mode0 = (op % 1000) / 100;
        mode1 = (op % 10000) / 1000;

        fetchArg(mode0, deref(pc + 1), &r0);
        fetchArg(mode1, deref(pc + 2), &r1);
        break;
      case HLT:
        break;
    }

    // execute
    switch (opcode) {
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
    // printf("deref[%d]: ", addr);
    assert(addr >= 0 && addr < _mem.size());
    int val = _mem[addr];
    // printf("%d\n", val);
    return val;
  }

  int *derefDest(int addr) {
    // printf("deref[%d]*: ", addr);
    assert(addr >= 0 && addr < _mem.size());
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

    clearRegisters();

    did_halt = false;
    has_output = false;

    _input = std::queue<int>();
    _out = 0;
  }

  int pc;
  int opcode;

  int r0;
  int r1;
  int *r2;

  bool did_halt;
  bool has_output;

 private:
  std::queue<int> _input;
  int _out;

  std::vector<int> _mem;
};

std::vector<int> runProgramAndGetOutput(std::string program,
                                        const std::vector<int> &input) {
  VM vm(std::move(program));
  for (auto i : input) {
    vm.pushInput(i);
  }

  std::vector<int> output;
  for (;;) {
    vm.run();
    if (vm.did_halt) {
      break;
    }
    int out = vm.consumeOutput();
    output.push_back(out);
  }
  return output;
}

std::vector<int> runProgramAndGetOutput(std::string program, int input) {
  return runProgramAndGetOutput(std::move(program), std::vector<int>({input}));
}

int runProgramAndGetFirstOutput(std::string program,
                                const std::vector<int> &input) {
  VM vm(std::move(program));
  for (auto i : input) {
    vm.pushInput(i);
  }
  vm.run();
  return vm.consumeOutput();
}

int runProgramAndGetFirstOutput(std::string program, int input) {
  return runProgramAndGetFirstOutput(std::move(program),
                                     std::vector<int>({input}));
}
