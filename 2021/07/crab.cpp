#include <cassert>
#include <cstdio>
#include <numeric>
#include <vector>

using namespace std;

long long fuel(const vector<int> &initial_pos, bool linear_consumption,
               int target_pos) {
  long long cost = 0;
  for (auto pos : initial_pos) {
    const long long dist = abs(target_pos - pos);
    cost += linear_consumption ? dist : (((dist + 1) * dist) / 2);
  }
  return cost;
}

void solve(const vector<int> &initial_pos, bool linear_consumption,
           int max_pos) {
  int best_pos = -1;
  long long min_cost = numeric_limits<long long>::max();
  for (int target_pos = 0; target_pos <= max_pos; target_pos++) {
    long long cost = fuel(initial_pos, linear_consumption, target_pos);
    if (cost < min_cost) {
      best_pos = target_pos;
      min_cost = cost;
    }
  }
  printf("best position %d costs %lld\n", best_pos, min_cost);
}

int main() {
  vector<int> initial_pos;
  int max_pos = 0;
  int p;
  while (scanf("%d", &p) == 1) {
    char c = getchar();
    assert(c == ',' || c == '\n' || c == EOF);
    initial_pos.push_back(p);
    max_pos = max(max_pos, p);
  }
  printf("crabs around [0, %d]\n", max_pos);

  puts("part 1:");
  solve(initial_pos, true, max_pos);
  puts("part 2:");
  solve(initial_pos, false, max_pos);

  return 0;
}
