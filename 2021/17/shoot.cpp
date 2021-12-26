#include <algorithm>
#include <cassert>
#include <cstdio>
#include <utility>

int xmin;
int xmax;
int ymin;
int ymax;

std::pair<int, int> findBestVelocity() {
  int num_possible_velocities = 0;
  int global_highest_posy = 0;
  std::pair<int, int> ret;

  for (int init_x = 0; init_x <= 200; init_x++) {
    for (int init_y = -200; init_y <= 200; init_y++) {
      int x = init_x;
      int y = init_y;
      int posx = 0;
      int posy = 0;
      int highest_posy = 0;
      for (int step = 0;; step++) {
        posx += x;
        posy += y;
        highest_posy = std::max(highest_posy, posy);

        if (posx >= xmin && posx <= xmax) {
          if (posy >= ymin && posy <= ymax) {
            printf("%d,%d\n", init_x, init_y);
            num_possible_velocities += 1;
            if (highest_posy > global_highest_posy) {
              ret = std::make_pair(init_x, init_y);
              global_highest_posy = highest_posy;
            }
            goto next;
          }
        }
        if (posx > xmax) goto next;
        if (posy < ymin) goto next;

        if (x > 0) {
          x -= 1;
        } else if (x < 0) {
          x += 1;
        }
        y -= 1;
      }
    next:
      continue;
    }
  }

  printf("possible shooting velocities: %d\n", num_possible_velocities);
  printf("highest y reached: %d\n", global_highest_posy);
  return ret;
}

int main() {
  scanf("target area: x=%d..%d, y=%d..%d\n", &xmin, &xmax, &ymin, &ymax);
  printf("target area: x=%d..%d, y=%d..%d\n", xmin, xmax, ymin, ymax);
  int x, y;
  std::tie(x, y) = findBestVelocity();
  printf("selected shooting velocity: %d,%d\n", x, y);
  return 0;
}
