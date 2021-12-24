#include <cstdio>
#include <queue>
#include <unordered_set>

#include "../geom.h"
#include "../lib.h"

constexpr int INF = std::numeric_limits<int>::max();

constexpr int MAXD = 100;
int grid[MAXD * 5][MAXD * 5];
int m;
int n;

int grid_path[MAXD * 5][MAXD * 5];

void dump() {
  for (int y = 0; y < m; y++) {
    for (int x = 0; x < n; x++) {
      printf("%d", grid[y][x]);
    }
    putchar('\n');
  }
  putchar('\n');
}

void expand() {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      if (i == 0 && j == 0) continue;
      for (int y = 0; y < m; y++) {
        for (int x = 0; x < n; x++) {
          int new_risk = grid[y][x] + i + j;
          if (new_risk > 9) new_risk -= 9;
          grid[i * m + y][j * n + x] = new_risk;
        }
      }
    }
  }
  m *= 5;
  n *= 5;
}

class RiskPos {
 public:
  RiskPos(int risk, Pt pos) noexcept : risk(risk), pos(pos) {}

  bool operator<(const RiskPos &other) const {
    return (risk > other.risk) || (risk == other.risk && pos < other.pos);
  }

  int risk;
  Pt pos;
};

int riskOfLeastRiskyPath() {
  const Pt dirs[4] = {
      Pt(0, -1),
      Pt(1, 0),
      Pt(0, 1),
      Pt(-1, 0),
  };

  std::priority_queue<RiskPos> pq;
  std::unordered_map<Pt, Pt> parent;
  std::unordered_map<Pt, int> min_risk;

  auto minRisk = [&min_risk](Pt pt) -> int {
    return lookupOr(min_risk, pt, INF);
  };

  // Init
  Pt origin(0, 0);
  parent[origin] = origin;
  for (int y = 0; y < m; y++) {
    for (int x = 0; x < n; x++) {
      Pt p(x, y);
      const int risk = (p == origin) ? grid[origin.y][origin.x] : INF;
      min_risk[p] = risk;
      pq.push(RiskPos(risk, origin));
    }
  }

  // Minimize all paths from source
  while (!pq.empty()) {
    const RiskPos v = pq.top();
    pq.pop();
    const int min_risk_v = minRisk(v.pos);
    for (Pt dir : dirs) {
      const Pt w = v.pos + dir;
      if (!w.withinBox(n, m)) continue;

      int min_risk_w = minRisk(w);
      assert(min_risk_w > 0);
      if (min_risk_v + grid[w.y][w.x] < min_risk_w) {
        min_risk_w = min_risk_v + grid[w.y][w.x];
        min_risk[w] = min_risk_w;
        pq.push(RiskPos(min_risk_w, w));
        assert(w != v.pos);
        parent[w] = v.pos;
      }
    }
  }

  const Pt dest(n - 1, m - 1);
  /*
  memset(grid_path, -1, sizeof(grid_path));
  for (Pt p = dest; p != Pt(0, 0); p = parent[p]) {
    grid_path[p.y][p.x] = grid[p.y][p.x];
  }
  for (int y = 0; y < m; y++) {
    for (int x = 0; x < n; x++) {
      int risk = grid_path[y][x];
      if (risk == -1) {
        putchar(' ');
      } else {
        printf("%d", risk);
      }
    }
    putchar('\n');
  }
  putchar('\n');
  */

  return minRisk(dest) - grid[origin.y][origin.x];
}

int main() {
  char c;
  int y = 0;
  int x = 0;
  for (;;) {
    c = getchar();
  process_c:
    if (c == '\n') {
      y += 1;
      c = getchar();
      if (c == EOF || c == '\n') break;
      x = 0;
      goto process_c;
    }
    if (c == EOF) {
      break;
    }
    grid[y][x] = c - '0';
    x += 1;
  }
  m = y;
  n = x;
  assert(m > 0 && m <= MAXD);
  assert(n > 0 && n <= MAXD);

  // dump();

  int min_risk = riskOfLeastRiskyPath();
  printf("  min risk on simple map: %d\n", min_risk);

  expand();
  // dump();

  min_risk = riskOfLeastRiskyPath();
  printf("min risk on expanded map: %d\n", min_risk);

  return 0;
}
