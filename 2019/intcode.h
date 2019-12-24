#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

enum Opcode {
  ADD = 1,
  MUL = 2,
  IN = 3,
  OUT = 4,
  JMP_IF_TRUE = 5,
  JMP_IF_FALSE = 6,
  LT = 7,
  EQ = 8,
  UBP = 9,  // update base pointer
  HLT = 99
};

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

  Word peek() {
    assert(hasData());
    return _fifo.front();
  }

  void produce(Word word) { _fifo.push(word); }

  void produce(const std::string &ascii) {
    for (char c : ascii) {
      _fifo.push(c);
    }
  }

  size_t size() const { return _fifo.size(); }
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
  PAUSED = 0,
  RUNNING = 1,
  PENDING_IN = 2,
  HALTED = 3,
};

struct CPU {
  CPU() { clearState(); }

  explicit CPU(Program program) {
    clearState();
    loadProgram(std::move(program));
  }

  explicit CPU(const std::string &program) : CPU(parseProgram(program)) {}

  void loadProgram(Program program) {
    // loadProgram() does not clear the machine state, only registers
    clearRegisters();
    _mem = std::move(program);
  }

  bool halted() const { return status == HALTED; }
  bool paused() const { return status == PAUSED || status == PENDING_IN; }

  // All methods below (before decodeAndExecute()) have
  //
  //   Pre-conditions:  halted() || paused()
  //   Post-conditions: halted() || paused()
  //
  // where
  //
  //   paused() = (status == PAUSED || status == PENDING_IN)

  // Runs eagerly until it halts or IN instruction
  // is executed and the input queue is empty.
  void run() {
    if (halted()) {
      return;
    }
    assert(paused());
    status = RUNNING;
    for (;;) {
      decodeAndExecute();
      if (status == PENDING_IN) {
        break;
      }
      if (op == HLT) {
        status = HALTED;
        break;
      }
    }
  }

  // Runs until it halts or an OUT instructions causes the CPU to pause.
  void runUntilOutput() {
    if (halted()) {
      return;
    }
    assert(paused());
    status = RUNNING;
    for (;;) {
      decodeAndExecute();
      if (status == PENDING_IN) {
        break;
      }
      if (op == OUT) {
        status = PAUSED;  // pause on OUT
        break;
      }
      if (op == HLT) {
        status = HALTED;
        break;
      }
    }
  }

  void runUntilIO() {
    if (halted()) {
      return;
    }
    assert(paused());
    status = RUNNING;
    for (;;) {
      decodeAndExecute();
      if (status == PENDING_IN) {
        break;
      }
      if (op == OUT || op == IN) {
        status = PAUSED;  // pause on IN/OUT
        break;
      }
      if (op == HLT) {
        status = HALTED;
        break;
      }
    }
  }

  // Run a single instruction, and pause again.
  void tick() {
    if (halted()) {
      return;
    }
    assert(paused());

    status = RUNNING;
    decodeAndExecute();
    if (status == PENDING_IN) {
      return;
    }
    if (op == HLT) {
      status = HALTED;
      return;
    }

    status = PAUSED;
  }

  // Try to consume to_consume values from the input buffer by executing
  // the minimum amount of instructions necessary.  Returns the number of
  // consumed values.
  int consumeInput(int to_consume) {
    const int initial_input_size = (int)_input.size();

    int target_input_size = initial_input_size - to_consume;
    if (target_input_size < 0) {
      target_input_size = 0;
    }

    while (paused() && _input.hasData() && _input.size() != target_input_size) {
      tick();
    }

    return initial_input_size - (int)_input.size();
  }

  // Consume a Day23 network packet from the input buffer.
  [[nodiscard]] bool consumePacket() {
    if (halted()) {
      return false;
    }
    assert(paused());

    if (_input.size() == 0) {
      // nothing to do
      return false;
    }

    if (_input.size() == 1) {
      assert(_input.peek() == -1 && "expected a no-packet indicator");
      return consumeInput(1) == 1;
    }

    return consumeInput(2) == 2;
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
        if (_input.hasData()) {
          *r2 = _input.consume();
          // printf("IN %lld\n", *r2);
          pc += 2;
        } else {
          // printf("IN <pending>\n");
          // pause without incrementing the pc, so that when
          // resuming, the IN instruction gets executed again.
          status = PENDING_IN;
        }
        break;
      case OUT: {
        // printf("OUT %lld\n", r0);
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
        // printf("HLT\n");
        pc += 1;
        // the run loop should look at the opcode,
        // change the status, and break the loop.
        break;

      default:
        fprintf(stderr, "intcode: bad op: %d\n", op);
        assert(false && "intcode: bad op");
        exit(1);
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
    assert(addr >= 0);
    Word val = 0;
    if (addr >= _mem.size()) {
      val = _extra_mem[addr];
    } else {
      val = _mem[addr];
    }
    return val;
  }

  Word *derefDest(Word addr) {
    assert(addr >= 0);
    Word *val = nullptr;
    if (addr >= _mem.size()) {
      val = &_extra_mem[addr];
    } else {
      val = &_mem[addr];
    }
    return val;
  }

  void pushInput(Word word) { _input.produce(word); }
  void pushInput(const std::string &ascii) { _input.produce(ascii); }
  bool hasInput() const { return _input.hasData(); }
  // consumeInput() can only happen by executing an IN instruction

  void pushOutput(Word word) { _output.produce(word); }
  bool hasOutput() const { return _output.hasData(); }
  Word consumeOutput() { return _output.consume(); }

  const Device &output() const { return _output; }

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
  int op;

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
