#include <algorithm>
#include <cstdio>
#include <vector>

#include "../geom.h"

using namespace std;

struct Segment {
  static constexpr double EPS = 1E-9;

  Segment(Pt a, Pt b) : a(a), b(b), dist(a.dist(b)) {}

  bool straight() const { return a.x == b.x || a.y == b.y; }

  bool contains(Pt p) const {
    const double d = a.dist(p) + b.dist(p);
    return d + EPS >= dist && d - EPS <= dist;
  }

  Pt a;
  Pt b;
  double dist;
};

int countDangerousPoints(const vector<Segment> &segments, Pt max_point) {
  const bool print_map = max_point.lengthSquared() < 50 * 50;

  int dangerous_points = 0;
  for (int y = 0; y <= max_point.y; y++) {
    for (int x = 0; x <= max_point.x; x++) {
      Pt pt(x, y);
      int hits = 0;
      for (auto &segment : segments) {
        if (segment.contains(pt)) {
          hits += 1;
          if (hits == 2) {
            dangerous_points += 1;
            break;
          }
        }
      }
      !print_map || (hits ? printf("%d", hits) : putchar('.'));
    }
    !print_map || putchar('\n');
  }
  return dangerous_points;
}

int main() {
  vector<Segment> straight_segments;
  vector<Segment> all_segments;

  int x1, y1, x2, y2;
  Pt max_point = Pt(0, 0);
  while (scanf("%d,%d -> %d,%d", &x1, &y1, &x2, &y2) == 4) {
    Pt a(x1, y1);
    Pt b(x2, y2);
    Pt bound(max(a.x, b.x), max(a.y, b.y));
    max_point = Pt(max(bound.x, max_point.x), max(bound.y, max_point.y));

    Segment seg(a, b);
    all_segments.push_back(seg);
    if (seg.straight()) {
      straight_segments.push_back(seg);
    }
  }

  printf("dangerous points: %d (straight segments only)\n",
         countDangerousPoints(straight_segments, max_point));
  printf("dangerous points: %d\n",
         countDangerousPoints(all_segments, max_point));

  return 0;
}
