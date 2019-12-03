#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

int main() {
  long long fuel_sum = 0;
  int mass = 0;
  while (scanf("%d", &mass) == 1) {
    printf("%d\n", mass);
    int fuel = std::max(mass / 3 - 2, 0);
    fuel_sum += fuel;
  }
  printf("%lld\n", fuel_sum);

  return 0;
}
