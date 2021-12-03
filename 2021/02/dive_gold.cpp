#include <cassert>
#include <cstdio>
#include <string>

int main() {
  char command[41];
  long long value;
  long long x = 0, y = 0;
  long long aim = 0;
  std::string s;
  while (scanf("%40s %lld", command, &value) == 2) {
    s = command;
    if (s == "forward") {
      x += value;
      y += aim * value;
    } else if (s == "up") {
      // y -= value;
      aim -= value;
    } else if (s == "down") {
      // y += value;
      aim += value;
    } else if (s == "backward") {
      x -= value;
    } else {
      assert(false);
    }
  }
  printf("%lld\n", x * y);
  return 0;
}
