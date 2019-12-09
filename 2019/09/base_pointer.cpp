#include <cstdio>
#include <vector>

#include "intcode.h"

int main() {
  Program data;
  Word code;
  while (scanf("%lld", &code)) {
    data.push_back(code);
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  CPU cpu(std::move(data));
  cpu.pushInput(1); // tests
  // cpu.pushInput(2);  // production
  cpu.run();

  return 0;
}
