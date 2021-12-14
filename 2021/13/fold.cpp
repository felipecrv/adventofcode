#include <algorithm>
#include <cstdio>
#include <unordered_set>

#include "../geom.h"
#include "../lib.h"

struct DotSet {
 private:
  std::unordered_set<Pt> dots;
  Pt max;

 public:
  void insert(Pt dot) {
    max.x = std::max(max.x, dot.x);
    max.y = std::max(max.y, dot.y);
    dots.insert(dot);
  }

  void dump() const {
    for (int y = 0; y <= max.y; y++) {
      for (int x = 0; x <= max.x; x++) {
        putchar(contains(dots, Pt(x, y)) ? '#' : '.');
      }
      putchar('\n');
    }
    putchar('\n');
  }

  void fold(char axis, int fold_pos) {
    const int comp = (axis == 'y');
    std::unordered_set<Pt> dots_after_fold;
    dots_after_fold.reserve(dots.size());
    for (auto dot : dots) {
      dot.comp[comp] = fold_pos - abs(fold_pos - dot.comp[comp]);
      dots_after_fold.insert(dot);
    }
    max.comp[comp] = fold_pos;
    dots = std::move(dots_after_fold);
  }

  int numVisible() const { return dots.size(); }
};

int main() {
  DotSet dotset;
  char c;
  int comp = 0;
  Pt dot(-1, -1);
  for (;;) {
    c = getchar();
  process_c:
    if (c >= '0' && c <= '9') {
      comp *= 10;
      comp += c - '0';
    } else if (c == ',' || c == '\n') {
      if (dot.x == -1) {
        dot.x = comp;
      } else {
        dot.y = comp;
        dotset.insert(dot);
        dot.x = -1;  // get ready to parse x next time
      }
      comp = 0;

      if (c == '\n') {
        c = getchar();
        if (c == '\n') {
          break;
        } else {
          goto process_c;
        }
      }
    } else {
      assert(false && "unexpected char");
    }
  }

  // dotset.dump();

  char buf1[8];
  char buf2[8];
  char axis;
  int fold_pos;
  for (int i = 0; scanf("%s %s %c=%d", buf1, buf2, &axis, &fold_pos) == 4;
       i++) {
    dotset.fold(axis, fold_pos);
    // printf("fold along %c=%d:\n", axis, fold_pos);
    // dotset.dump();
    if (i == 0) {
      printf("visible dots after first fold: %d\n\n", dotset.numVisible());
    }
  }
  puts("final configuration:");
  dotset.dump();

  return 0;
}
