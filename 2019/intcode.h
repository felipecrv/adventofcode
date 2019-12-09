#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#define ADD 1
#define MUL 2
#define IN 3
#define OUT 4
#define JMP_IF_TRUE 5
#define JMP_IF_FALSE 6
#define LT 7
#define EQ 8
#define UBP 9  // update base pointer
#define HLT 99

using Word = long long;
using Program = std::vector<Word>;
using Buffer = std::vector<Word>;

struct Device {
  bool hasData() const { return !_fifo.empty(); }

  Word consume() {
    assert(hasData());
    Word word = _fifo.front();
    _fifo.pop();
    return word;
  }

  void produce(Word word) { _fifo.push(word); }

  void clear() { _fifo = std::queue<Word>(); }

 private:
  std::queue<Word> _fifo;
};

Program parseProgram(const std::string &ss) {
  Program program;

  const char *s = ss.c_str();
  bool done = false;
  for (Word code; !done && sscanf(s, "%lld", &code) == 1;) {
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

struct CPU {
  CPU(Program program) : _mem(std::move(program)) { clearState(); }

  explicit CPU(const std::string &program) : CPU(parseProgram(program)) {}

  // Runs eagerly until it halts or IN instruction is executed
  // and the input queue is empty.
  void run() {
    assert(status == PAUSED);
    status = RUNNING;
    for (;;) {
      decodeAndExecute();
      if (op == HLT) {
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
      if (op == HLT) {
        assert(status == RUNNING);
        status = HALTED;
        break;
      }
      if (op == OUT) {
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
    Word opcode = deref(pc);
    op = opcode % 100;
    switch (op) {
      case ADD:
      case MUL:
      case LT:
      case EQ:
        mode0 = (opcode / 100) % 10;
        mode1 = (opcode / 1000) % 10;
        mode2 = (opcode / 10000) % 10;

        fetchArg(mode0, deref(pc + 1), &r0);
        fetchArg(mode1, deref(pc + 2), &r1);
        fetchDestArg(mode2, deref(pc + 3), &r2);
        break;
      case IN:
        mode0 = (opcode / 100) % 10;

        fetchDestArg(mode0, deref(pc + 1), &r2);
        break;
      case OUT:
        mode0 = (opcode / 100) % 10;

        fetchArg(mode0, deref(pc + 1), &r0);
        break;
      case JMP_IF_TRUE:
      case JMP_IF_FALSE:
        mode0 = (opcode / 100) % 10;
        mode1 = (opcode / 1000) % 10;

        fetchArg(mode0, deref(pc + 1), &r0);
        fetchArg(mode1, deref(pc + 2), &r1);
        break;
      case UBP:
        mode0 = (opcode / 100) % 10;

        fetchArg(mode0, deref(pc + 1), &r0);
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
        if (hasInput()) {
          *r2 = consumeInput();
          printf("IN %lld\n", *r2);
          pc += 2;
        } else {
          printf("IN <pending>\n");
          // pause without incrementing the pc, so that when
          // resuming, the IN instruction gets executed again.
          status = PAUSED;
        }
        break;
      case OUT: {
        printf("OUT %lld\n", r0);
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
      case UBP:
        bp = bp + r0;
        pc += 2;
        break;
      case HLT:
        printf("HLT\n");
        pc += 1;
        // the run loop should look at the opcode,
        // change the status, and break the loop.
        break;
    }
  }

  void fetchArg(int mode, Word mem_cell_val, Word *out_reg) {
    if (mode == 0) {  // pos
      *out_reg = deref(mem_cell_val);
    } else if (mode == 1) {  // immediate
      *out_reg = mem_cell_val;
    } else if (mode == 2) {  // relative to bp
      *out_reg = deref(bp + mem_cell_val);
    } else {
      assert(false && "invalid mode");
    }
  }

  void fetchDestArg(int mode, Word mem_cell_val, Word **out_reg) {
    if (mode == 0) {
      *out_reg = derefDest(mem_cell_val);
    } else if (mode == 1) {
      assert(false && "dest param will never be in immediate mode");
    } else if (mode == 2) {
      *out_reg = derefDest(bp + mem_cell_val);
    } else {
      assert(false && "invalid mode");
    }
  }

  Word deref(Word addr) {
    // printf("deref[%lld]: ", addr);
    assert(addr >= 0);
    Word val = 0;
    if (addr >= _mem.size()) {
      val = _extra_mem[addr];
    } else {
      val = _mem[addr];
    }
    // printf("%lld\n", val);
    return val;
  }

  Word *derefDest(Word addr) {
    // printf("deref[%lld]*: ", addr);
    assert(addr >= 0);
    Word *val = nullptr;
    if (addr >= _mem.size()) {
      val = &_extra_mem[addr];
    } else {
      val = &_mem[addr];
    }
    // printf("%lld\n", *val);
    return val;
  }

  void pushInput(Word word) { _input.produce(word); }
  bool hasInput() const { return _input.hasData(); }
  Word consumeInput() { return _input.consume(); }

  void pushOutput(Word word) { _output.produce(word); }
  bool hasOutput() const { return _output.hasData(); }
  Word consumeOutput() { return _output.consume(); }

  void clearRegisters() {
    r0 = 0;
    r1 = 0;
    r2 = nullptr;
  }

  void clearState() {
    pc = 0;
    op = 0;

    bp = 0;
    clearRegisters();

    status = PAUSED;

    _input.clear();
    _output.clear();
  }

  int pc;
  Word op;

  Word bp;  // base pointer

  Word r0;
  Word r1;
  Word *r2;

  enum Status status;

 private:
  Device _input;
  Device _output;

  Buffer _mem;
  std::unordered_map<Word, Word> _extra_mem;
};

Program runProgramAndGetOutput(Program program, const Program &input) {
  CPU cpu(std::move(program));
  for (auto i : input) {
    cpu.pushInput(i);
  }

  Program output;
  for (;;) {
    cpu.runUntilOutput();
    if (cpu.status == HALTED) {
      break;
    }
    Word out = cpu.consumeOutput();
    output.push_back(out);
  }
  return output;
}

Buffer runProgramAndGetOutput(Program program, Word input) {
  return runProgramAndGetOutput(std::move(program), Buffer({input}));
}

Word runProgramAndGetFirstOutput(Program program, const Buffer &input) {
  CPU cpu(std::move(program));
  for (auto i : input) {
    cpu.pushInput(i);
  }
  cpu.runUntilOutput();
  return cpu.consumeOutput();
}

Word runProgramAndGetFirstOutput(Program program, Word input) {
  return runProgramAndGetFirstOutput(std::move(program), Buffer({input}));
}
