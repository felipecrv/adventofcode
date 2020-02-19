#include <unistd.h>
#include <cassert>
#include <cstdio>
#include <string>

#include "intcode.h"

struct Springdroid {
  explicit Springdroid(Program program) : _program(std::move(program)) {
    _cpu.loadProgram(_program);
  }

  void pushScript(const std::string &script) {
    long long last_output = 0;
    for (;;) {
      _cpu.runUntilOutput();
      if (_cpu.status == PENDING_IN) {
        printf("%s", script.c_str());
        _cpu.pushInput(script);
      }
      while (_cpu.hasOutput()) {
        long long c = _cpu.consumeOutput();
        putchar(c & 0xff);
        if (c == '\n' && last_output == '\n') {
          usleep(200 * 1000);
        }
        last_output = c;
      }
      if (_cpu.status == HALTED) {
        break;
      }
    }

    printf("Last output: %lld\n", last_output);
  }

 private:
  Program _program;
  CPU _cpu;
};

int main() {
  Program program;
  Word code;
  while (scanf("%lld", &code)) {
    program.push_back(code);
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  Springdroid droid(program);

  // Example
  // std::string script =
  //     "NOT A J\n"
  //     "NOT B T\n"
  //     "AND T J\n"
  //     "NOT C T\n"
  //     "AND T J\n"
  //     "AND D J\n"
  //     "WALK\n";

  // WALK program
  //
  // ~A or (D and (~B or ~C))
  // ~A or (~B and D) or (~C and D)
  //
  // std::string script =
  //     "NOT A J\n"
  //     "NOT B T\n"
  //     "AND D T\n"
  //     "OR T J\n"
  //     "NOT C T\n"
  //     "AND D T\n"
  //     "OR T J\n"
  //     "WALK\n";

  // RUN program
  //
  // (DE or DH) and ~(ABC)
  std::string script =
      "OR E J\n"
      "OR H J\n"
      "AND D J\n"
      "OR A T\n"
      "AND B T\n"
      "AND C T\n"
      "NOT T T\n"
      "AND T J\n"
      "RUN\n";

  droid.pushScript(script);

  return 0;
}
