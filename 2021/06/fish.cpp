#include <cassert>
#include <cstdio>
#include <iterator>
#include <numeric>

using namespace std;

int main() {
  long long population[9] = {0};  // population on each timer value
  int timer;
  while (scanf("%d", &timer) == 1) {
    population[timer] += 1;
    char c = getchar();
    assert(c == ',' || c == '\n' || c == EOF);
  }

  for (int day = 1; day <= 256; day++) {
    const long long ready = population[0];
    for (int timer = 1; timer < 9; timer++) {
      population[timer - 1] = population[timer];
    }
    population[8] = ready;
    population[6] += ready;

    if (day == 80 || day == 256) {
      printf("After %d days: %lld\n", day,
             accumulate(begin(population), end(population), 0ll));
    }
  }

  return 0;
}
