#include <cassert>
#include <cstdio>
#include <numeric>
#include <vector>

using namespace std;

long long costToMoveAllCrabs(const vector<int> &initial_pos,
                             bool linear_consumption, int target_pos) {
  long long cost = 0;
  for (auto pos : initial_pos) {
    const long long dist = abs(target_pos - pos);
    cost += linear_consumption ? dist : (((dist + 1) * dist) / 2);
  }
  return cost;
}

void solve(
    const vector<int> &initial_pos,  // initial pos of each crab
    bool linear_consumption,     // 1 cost per space unit (liner) or range sum
    int min_pos, int max_pos) {  // range of possible solutions
  int best_pos = -1;
  long long min_cost = numeric_limits<long long>::max();
  for (int target_pos = min_pos; target_pos <= max_pos; target_pos++) {
    long long cost =
        costToMoveAllCrabs(initial_pos, linear_consumption, target_pos);
    if (cost < min_cost) {
      best_pos = target_pos;
      min_cost = cost;
    }
  }
  printf("best position %d costs %lld\n", best_pos, min_cost);
}

int main() {
  vector<int> initial_pos;
  int min_pos = 0;
  int max_pos = 0;
  int p;
  while (scanf("%d", &p) == 1) {
    char c = getchar();
    assert(c == ',' || c == '\n' || c == EOF);
    initial_pos.push_back(p);
    min_pos = min(min_pos, p);
    max_pos = max(max_pos, p);
  }
  printf("crabs are distributed in the [%d, %d] range\n", min_pos, max_pos);

  puts("part 1:");
  solve(initial_pos, true, min_pos, max_pos);
  puts("part 2:");
  solve(initial_pos, false, min_pos, max_pos);

  return 0;
}
