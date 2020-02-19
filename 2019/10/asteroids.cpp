#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "lib.h"
#include "geom.h"

using namespace std;

#define MAXN 1000
char mapa[MAXN][MAXN];
int m = 0;
int n = 0;

#define EPSILON 1.0e-300

vector<Pt> asteroids;

void dump(Pt from, Pt target, Pt blocker) {
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      Pt it(j, i);
      if (it == from) {
        putchar('0');
      } else if (it == target) {
        putchar('X');
      } else if (it == blocker) {
        printf("█");
      } else {
        putchar(mapa[i][j]);
      }
    }
    putchar('\n');
  }
  putchar('\n');
}

void dumpMap() {
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      putchar(mapa[i][j]);
    }
    putchar('\n');
  }
  putchar('\n');
}

bool canSee(Pt from, Pt target) {
  Line line(from, target);

  int min_x = min(from.x, target.x);
  int min_y = min(from.y, target.y);
  int max_x = max(from.x, target.x);
  int max_y = max(from.y, target.y);

  bool blocked = false;
  Pt _min = Pt(min_x, min_y);
  Pt _max = Pt(max_x, max_y);
  for (auto asteroid : asteroids) {
    if (asteroid > _max || asteroid < _min) {
      continue;
    }
    if (asteroid == from || asteroid == target) {
      continue;
    }
    if (line.contains(asteroid)) {
      Pt blocker = asteroid;
      // dump(from, target, blocker);
      blocked = true;

      char letter_blocker = mapa[blocker.y][blocker.x];
      char &letter_target = mapa[target.y][target.x];
      if (letter_blocker != '#') {
        letter_target = tolower(letter_blocker);
      }
      break;
    }
  }

  return !blocked;
}

int main() {
  int i = 0;
  int j = 0;
  for (;;) {
    char c = getchar();
    if (c == '.' || c == '#' || isalpha(c)) {
      mapa[i][j] = c;
      if (c == '#' || isalpha(c)) {
        asteroids.push_back(Pt(j, i));
      }
      j++;
    } else if (c == '\n') {
      i++;
      assert(n == j || n == 0);
      n = j;
      j = 0;
    } else if (c == EOF) {
      break;
    }
  }
  m = i;

  // puts("asteroids");
  // for (auto a : asteroids) {
  //   printf("%d %d\n", a.x, a.y);
  // }

  vector<int> visible_count;
  for (int i = 0; i < asteroids.size(); i++) {
    Pt fixed = asteroids[i];
    int count = 0;  // how many can see fixed

    for (int j = 0; j < asteroids.size(); j++) {
      if (j == i) continue;

      Pt from = asteroids[j];
      if (canSee(fixed, from)) count++;
    }

    visible_count.push_back(count);
  }

  int max_visible = 0;
  Pt max_selected = asteroids[0];
  for (int i = 0; i < visible_count.size(); i++) {
    if (max_visible < visible_count[i]) {
      max_visible = visible_count[i];
      max_selected = asteroids[i];
    }
  }
  printf("%d,%d can reach %d\n", max_selected.x, max_selected.y, max_visible);

  return 0;
}
