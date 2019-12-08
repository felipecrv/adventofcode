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

enum Status {
  RUNNING = 0,
  HALTED = 1,
  PAUSED = 2  // because of I/O or on purpose
};

struct VM {
  VM(std::vector<int> program) : _mem(std::move(program)) { clearState(); }

  explicit VM(const std::string &program) : VM(parseProgram(program)) {}

  // Runs eagerly until it halts or IN instruction is executed
  // and the input queue is empty.
  void run() {
    assert(status == PAUSED);
    status = RUNNING;
    for (;;) {
      decodeAndExecute();
      if (opcode == HLT) {
        assert(status == RUNNING);
        status = HALTED;
        break;
      }
      if (status == PAUSED || status == HALTED) {
        break;
      }
    }
  }

  // Runs until it halts or an OUT instructions causes the CPU to pause.
  void runUntilOutput() {
    assert(status = PAUSED);
    status = RUNNING;
    for (;;) {
      decodeAndExecute();
      if (status == PAUSED) {
        // an IN instruction can pause the CPU
        break;
      }
      if (opcode == HLT) {
        assert(status == RUNNING);
        status = HALTED;
        break;
      }
      if (opcode == OUT) {
        status = PAUSED;
        break;
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
        if (hasInput()) {
          *r2 = consumeInput();
          printf("IN %d\n", *r2);
          pc += 2;
        } else {
          printf("IN <pending>\n");
          // pause without incrementing the pc, so that when
          // resuming, the IN instruction gets executed again.
          status = PAUSED;
        }
        break;
      case OUT: {
        printf("OUT %d\n", r0);
        pushOutput(r0);
        pc += 2;
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
        // the run loop should look at the opcode,
        // change the status, and break the loop.
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

  bool hasInput() const { return !_input.empty(); }

  int consumeInput() {
    assert(!_input.empty());
    int value = _input.front();
    _input.pop();
    return value;
  }

  void pushOutput(int value) { _output.push(value); }

  bool hasOutput() const { return !_output.empty(); }

  int consumeOutput() {
    assert(hasOutput());
    int value = _output.front();
    _output.pop();
    return value;
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

    status = PAUSED;

    _input = std::queue<int>();
    _output = std::queue<int>();
  }

  int pc;
  int opcode;

  int r0;
  int r1;
  int *r2;

  enum Status status;

 private:
  std::queue<int> _input;
  std::queue<int> _output;

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
    vm.runUntilOutput();
    if (vm.status == HALTED) {
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
  vm.runUntilOutput();
  return vm.consumeOutput();
}

int runProgramAndGetFirstOutput(std::string program, int input) {
  return runProgramAndGetFirstOutput(std::move(program),
                                     std::vector<int>({input}));
}
