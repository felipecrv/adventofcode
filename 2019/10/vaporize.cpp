// BROKEN SOLUTION

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "lib.h"

using namespace std;

#define MAXN 1000
char _map[MAXN][MAXN];
int height = 0;
int width = 0;

void render() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      putchar(_map[y][x]);
    }
    putchar('\n');
  }
  putchar('\n');
}

vector<Vec> asteroidsFromMap() {
  vector<Vec> asteroids;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      char c = _map[y][x];
      if (c == '#' || isalpha(c)) {
        asteroids.emplace_back(x, y);
      }
    }
  }
  return asteroids;
}

double toAngle(Vec v) { return atan2(v.x, -v.y); }

Vec findBestAsteroid(const vector<Vec> &asteroids, int &out_can_see) {
  assert(asteroids.size() > 1);

  out_can_see = 0;

  Vec ret;
  vector<Vec> sorted;
  unordered_set<double> unique_angles;
  for (auto &center : asteroids) {
    sorted.clear();
    unique_angles.clear();

    for (auto &a : asteroids) {
      if (a != center) {
        sorted.emplace_back(a - center);
        unique_angles.insert(toAngle(a - center));
      }
    }

    // sort by (angle, distance^2) to center
    sort(sorted.begin(), sorted.end(), [](const Vec &a, const Vec &b) {
      Frac angle_a = toAngle(a);
      Frac angle_b = toAngle(b);

      if (angle_a < angle_b) {
        return true;
      }
      if (angle_a == angle_b) {
        return a.lengthSquared() < b.lengthSquared();
      }
      return false;
    });

    // calculate how many can be seen from center
    double cur = toAngle(sorted[0]);
    int can_see = 1;
    for (int i = 1; i < sorted.size(); i++) {
      double angle = toAngle(sorted[i]);
      if (cur == angle)
        continue;
      cur = angle;
      can_see += 1;
    }

    if (can_see > out_can_see) {
      out_can_see = can_see;
      ret = center;
    }
    /* if (unique_angles.size() > out_can_see) { */
    /*   out_can_see = unique_angles.size(); */
    /*   ret = center; */
    /* } */
  }

  return ret;
}

/* bool canSee(const vector<Vec> &asteroids, Vec from, Vec target) { */
/*   Line line(from, target); */

/*   int min_x = min(from.x, target.x); */
/*   int min_y = min(from.y, target.y); */
/*   int max_x = max(from.x, target.x); */
/*   int max_y = max(from.y, target.y); */

/*   bool blocked = false; */
/*   Vec _min = Vec(min_x, min_y); */
/*   Vec _max = Vec(max_x, max_y); */
/*   for (auto blocker : asteroids) { */
/*     if (blocker > _max || blocker < _min) { */
/*       continue; */
/*     } */
/*     if (blocker == from || blocker == target) { */
/*       continue; */
/*     } */
/*     if (line.contains(blocker)) { */
/*       // dump(from, target, blocker); */
/*       blocked = true; */

/*       char letter_blocker = _map[blocker.y][blocker.x]; */
/*       char &letter_target = _map[target.y][target.x]; */
/*       if (letter_blocker != '#') { */
/*         letter_target = tolower(letter_blocker); */
/*       } */
/*       break; */
/*     } */
/*   } */

/*   return !blocked; */
/* } */

int main() {
  int y = 0;
  int x = 0;
  for (;;) {
    char c = getchar();
    if (c == EOF) {
      break;
    }
    if (c == '.' || c == '#' || isalpha(c)) {
      _map[y][x] = c;
      x += 1;
    } else if (c == '\n') {
      y += 1;
      assert(width == x || width == 0);
      width = x;
      x = 0;
    }
  }
  height = y;

  render();

  const auto all_asteroids = asteroidsFromMap();

  int can_see = 0;
  Vec best_asteroid = findBestAsteroid(all_asteroids, can_see);

  printf("best_asteroid %d,%d can see %d\n\n", best_asteroid.x, best_asteroid.y,
         can_see);
  _map[best_asteroid.y][best_asteroid.x] = 'X';

  // Vec center(11, 19); // for answer
  // Vec center(11, 13); // for big map
  // Vec center(8, 3);

  return 0;
}
