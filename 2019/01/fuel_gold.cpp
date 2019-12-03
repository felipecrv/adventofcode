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

int fuel_for_mass(int mass) {
  int fuel = mass / 3 - 2;
  if (fuel >= 0) {
    return fuel;
  }
  return 0;
}

int fuel_for_mass_recursive(int mass) {
  int fuel = fuel_for_mass(mass);
  if (fuel == 0) {
    return 0;
  }
  return fuel + fuel_for_mass_recursive(fuel);
}

int main() {
  long long fuel_sum = 0;
  int mass = 0;
  while (scanf("%d", &mass) == 1) {
    printf("%d\n", mass);
    int fuel = fuel_for_mass_recursive(mass);
    fuel_sum += fuel;
  }
  printf("%lld\n", fuel_sum);

  return 0;
}
