#include <array>
#include <cassert>
#include <cstdio>
#include <queue>
#include <string>
#include <vector>

#include "../geom.h"

using namespace std;

std::vector<std::string> table;
int visited[256][256];

Pt dirs[4] = {
    Pt(0, -1),
    Pt(1, 0),
    Pt(0, 1),
    Pt(-1, 0),
};

int value(Pt p) { return table[p.y][p.x] - '0'; }
void resetVisited() { memset(visited, 0, sizeof(visited)); }
bool isVisited(Pt p) { return visited[p.y][p.x]; }
void markAsVisited(Pt p) { visited[p.y][p.x] = 1; }

int expandBasin(Pt center) {
  resetVisited();

  queue<Pt> q;
  q.push(center);
  int size = 0;

  while (!q.empty()) {
    const Pt v = q.front();
    q.pop();
    size += 1;

    for (Pt dir : dirs) {
      const Pt w = v + dir;
      const int value_w = value(w);
      if (value_w < 9 && value_w > value(v) && !isVisited(w)) {
        q.push(w);
        markAsVisited(w);
      }
    }
  }

  return size;
}

std::vector<int> basins;

int main() {
  char buf[256];
  while (scanf("%255s", buf) == 1) {
    table.emplace_back(buf);
    // printf("%s\n", table.back().c_str());
  }

  int risk_sum = 0;
  const int w = table[0].size();
  const int h = table.size();
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      Pt p(x, y);
      const int center_val = value(p);
      bool low_center = true;
      for (Pt dir : dirs) {
        Pt neigh = p + dir;
        if (!neigh.withinBox(w, h)) {
          continue;
        }
        if (value(neigh) <= center_val) {
          low_center = false;
          break;
        }
      }
      if (low_center) {
        risk_sum += center_val + 1;
        basins.push_back(expandBasin(p));
      }
    }
  }
  printf("risk sum: %d\n", risk_sum);

  int biggest_basins_product = 1;
  std::sort(basins.begin(), basins.end());
  for (int i = basins.size() - 3; i < basins.size(); i++) {
    biggest_basins_product *= basins[i];
  }
  printf("product: %d\n", biggest_basins_product);

  return 0;
}
