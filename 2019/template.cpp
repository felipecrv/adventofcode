#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

#define MAXN 1000
int data[MAXN];

int main() {
  int len = 0;
  while (scanf("%d", &data[len++])) {
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    } else {
      assert(c == ',');
    }
  }

  return 0;
}
