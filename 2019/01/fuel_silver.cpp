#include <algorithm>
#include <cstdio>

int main() {
  long long fuel_sum = 0;
  int mass = 0;
  while (scanf("%d", &mass) == 1) {
    int fuel = std::max(mass / 3 - 2, 0);
    fuel_sum += fuel;
  }
  printf("%lld\n", fuel_sum);

  return 0;
}
