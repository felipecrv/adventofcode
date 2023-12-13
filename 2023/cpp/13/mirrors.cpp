#include <cassert>
#include <cstdio>
#include <optional>

// clang-format off
struct Vec {
  int x, y;
  bool operator==(const Vec &other) const { return x == other.x && y == other.y; }
};

struct XAxis { int size; };
struct YAxis { int size; };
// clang-format on

// A tri-state "optional": 0, 1, or multiple.
class Reflection {
 private:
  std::optional<int> value_;
  bool multiple_ = false;

 public:
  [[nodiscard]] bool assign(int x) {
    if (!value_.has_value()) {
      value_ = x;
      return true;
    }
    multiple_ = true;
    return false;
  }

  int value_or(int v) const { return value().value_or(v); }

  std::optional<int> value() const {
    assert(!multiple_);
    return value_;
  }

  bool has_single_value() const { return !multiple_ && value_.has_value(); }
};

struct Grid {
  static const int MAX_N = 100;
  int n = 0, m = 0;
  char map[MAX_N][MAX_N];

  void Clear() { n = m = 0; }

  static bool ParseGrid(Grid *grid) {
    grid->Clear();
    int x = 0;
    int y = 0;
    for (;;) {
      char c = getchar();
      switch (c) {
        case '\n':
          if (x == 0) {
            goto done_parsing;
          }
          if (y == 0 && grid->m == 0) {
            grid->m = x;
          }
          x = 0;
          y += 1;
          break;
        case '#':
        case '.':
          grid->map[y][x] = c;
          x += 1;
          break;
        case EOF:
          if (y == 0) {
            return false;
          }
          goto done_parsing;
        default:
          assert(false);
      }
    }
  done_parsing:
    grid->n = y;
    return true;
  }

  char operator[](Vec pos) const { return map[pos.y][pos.x]; }

  bool Equals(Vec pos0, Vec pos1, std::optional<Vec> smudge) const {
    assert(pos0 != pos1);
    bool equals = (*this)[pos0] == (*this)[pos1];
    return equals ^
           (smudge.has_value() && (pos0 == *smudge || pos1 == *smudge));
  }

  bool IsReflection(XAxis, int x, std::optional<Vec> smudge) const {
    bool reflect = true;
    for (int y = 0; y < n && reflect; y++) {
      for (int i = 0; i < x && x + i < m && reflect; i++) {
        Vec left{x - i - 1, y};
        Vec right{x + i, y};
        reflect = Equals(left, right, smudge);
      }
    }
    return reflect;
  }

  bool IsReflection(YAxis, int y, std::optional<Vec> smudge) const {
    bool reflect = true;
    for (int x = 0; x < m && reflect; x++) {
      for (int i = 0; i < y && y + i < n && reflect; i++) {
        Vec up{x, y - i - 1};
        Vec down{x, y + i};
        reflect = Equals(up, down, smudge);
      }
    }
    return reflect;
  }

  template <class Axis>
  Reflection FindReflection(Axis axis, std::optional<Vec> smudge,
                            std::optional<int> skipping) const {
    Reflection ret;
    for (int z = 1; z < axis.size; z++) {
      if ((!skipping.has_value() || *skipping != z) &&
          IsReflection(axis, z, smudge)) {
        if (!ret.assign(z)) return ret;
      }
    }
    return ret;
  }

  std::pair<int, int> Summarize() const {
    XAxis x_axis{m};
    YAxis y_axis{n};
    auto orig_refl_x = FindReflection(x_axis, {}, {});
    auto orig_refl_y = FindReflection(y_axis, {}, {});
    // printf("  Without smudge: x: %2d, y: %2d\n", orig_refl_x.value_or(-1),
    //        orig_refl_y.value_or(-1));
    int orig_summary = orig_refl_x.value_or(0) + 100 * orig_refl_y.value_or(0);

    for (int y = 0; y < n; y++) {
      for (int x = 0; x < m; x++) {
        Vec smudge{x, y};
        auto refl_x = FindReflection(x_axis, smudge, orig_refl_x.value());
        auto refl_y = FindReflection(y_axis, smudge, orig_refl_y.value());
        if (refl_x.has_single_value() || refl_y.has_single_value()) {
          // printf("     With smudge: x: %2d, y: %2d\n", refl_x.value_or(-1),
          //        refl_y.value_or(-1));
          int smudge_summary = refl_x.value_or(0) + 100 * refl_y.value_or(0);
          return {orig_summary, smudge_summary};
        }
      }
    }
    return {orig_summary, -1};
  }
};

int main() {
  long long sum1 = 0;
  long long sum2 = 0;
  Grid grid;
  while (Grid::ParseGrid(&grid)) {
    auto summary = grid.Summarize();
    sum1 += summary.first;
    sum2 += summary.second;
  }
  printf("Part 1: %lld\n", sum1);
  printf("Part 2: %lld\n", sum2);
  return 0;
}
