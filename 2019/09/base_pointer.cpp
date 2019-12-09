#include <cstdio>
#include <vector>

#include "intcode.h"

int main() {
  std::vector<Word> data;
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

  VM vm(std::move(data));
  vm.pushInput(1); // tests
  // vm.pushInput(2);  // production
  vm.run();

  return 0;
}
