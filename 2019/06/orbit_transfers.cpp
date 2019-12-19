#include <cstdio>
#include <cstring>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>

#include "lib.h"

using namespace std;

unordered_map<string, string> parent;

int count(const string &k) {
  if (const auto *p = lookup(parent, k)) {
    return 1 + count(*p);
  }
  return 0;
}

int dist_to_dest(const string &from, string dest) {
  string *par = &dest;
  for (int par_dist = 0; par; par = lookup(parent, *par), par_dist++) {
    if (*par == from) {
      return par_dist;
    }
  }
  return -1;
}

int main() {
  char a[5];
  char b[5];
  while (scanf("%3s)%3s\n", a, b) == 2) {
    parent[b] = a;
  }

  int count_sum = 0;
  for (const auto &p : parent) {
    count_sum += count(p.first);
  }
  printf("count: %d\n", count_sum);

  // minimum num of transfers (part 2)
  int min_dist = numeric_limits<int>::max();
  int par_dist = 1;
  for (auto *par = lookup(parent, "YOU"); par; par = lookup(parent, *par)) {
    int dist = dist_to_dest(*par, "SAN");
    if (dist >= 0) {
      dist += par_dist;
      min_dist = min(min_dist, dist);
    }
    par_dist++;
  }
  printf("min_transfers: %d\n", min_dist - 2);

  return 0;
}
