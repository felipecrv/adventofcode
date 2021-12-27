#include <algorithm>
#include <cassert>
#include <cstdio>
#include <optional>
#include <queue>
#include <unordered_set>
#include <vector>

#include "../lib.h"

class Pos {
  friend int main();  // for reading

 private:
  int comp[3];

 public:
  struct Hasher {
    size_t operator()(const Pos& pos) const {
      int64_t i = 0;
      i = static_cast<int16_t>(pos.comp[0]);
      i = (i << 16) | static_cast<int16_t>(pos.comp[1]);
      i = (i << 16) | static_cast<int16_t>(pos.comp[2]);
      return std::hash<int64_t>{}(i);
    }
  };

  Pos() : Pos(0, 0, 0) {}

  Pos(int a, int b, int c) {
    comp[0] = a;
    comp[1] = b;
    comp[2] = c;
  }

  Pos operator-(const Pos& pos) const {
    Pos ret = *this;
    ret -= pos;
    return ret;
  }

  Pos operator-=(const Pos& pos) {
    comp[0] -= pos.comp[0];
    comp[1] -= pos.comp[1];
    comp[2] -= pos.comp[2];
    return *this;
  }

  Pos operator+(const Pos& pos) const {
    Pos ret = *this;
    ret += pos;
    return ret;
  }

  Pos operator+=(const Pos& pos) {
    comp[0] += pos.comp[0];
    comp[1] += pos.comp[1];
    comp[2] += pos.comp[2];
    return *this;
  }

  bool operator==(const Pos& pos) const {
    return memcmp(comp, pos.comp, sizeof(comp)) == 0;
  }

  int manhattanDist(const Pos& o) const {
    return abs(comp[0] - o.comp[0]) +  //
           abs(comp[1] - o.comp[1]) +  //
           abs(comp[2] - o.comp[2]);   //
  }

  void dump() const { printf("{%d,%d,%d}\n", comp[0], comp[1], comp[2]); }

  // x = 0..47
  Pos xformed(int x) const {
    assert(x >= 0 && x < 48);
    // clang-format off
    constexpr int comp_perm[6][3] = {
        {0, 1, 2},
        {0, 2, 1},
        {1, 0, 2},
        {1, 2, 0},
        {2, 0, 1},
        {2, 1, 0},
    };
    constexpr int comp_sign[8][3] = {
        { 1,  1,  1},
        { 1,  1, -1},
        { 1, -1,  1},
        { 1, -1, -1},
        {-1,  1,  1},
        {-1,  1, -1},
        {-1, -1,  1},
        {-1, -1, -1},
    };
    // clang-format on

    Pos ret;
    const int* perm = comp_perm[x / 8];
    ret.comp[0] = comp[perm[0]];
    ret.comp[1] = comp[perm[1]];
    ret.comp[2] = comp[perm[2]];
    const int* sign = comp_sign[x % 8];
    ret.comp[0] *= sign[0];
    ret.comp[1] *= sign[1];
    ret.comp[2] *= sign[2];
    return ret;
  }
};

using PositionSet = std::unordered_set<Pos, Pos::Hasher>;

class Scanner {
 private:
  std::optional<Pos> _pos;
  PositionSet _readings;

 public:
  PositionSet::const_iterator begin() const { return _readings.begin(); }
  PositionSet::const_iterator end() const { return _readings.end(); }

  void addReading(const Pos& r) { _readings.insert(r); }

  bool contains(const Pos& pos) const { return ::contains(_readings, pos); }

  int intersectsAt(const Scanner& t,
                   int at_least,  // min # of readings that should align
                   const Pos& t_pos, int x) const {
    for (const auto& t_reading : t._readings) {
      const Pos translated = t_pos + t_reading.xformed(x);
      if (contains(translated)) {
        if (--at_least <= 0) return true;
      }
    }
    return false;
  }

  [[nodiscard]] bool canIntersect(
      const Scanner& t,
      Pos& out_t_pos,      // t_pos that can make t...
      int& out_x) const {  // ...fully intersect with x transformation
    const int NUM_COMMON_READINGS_REQ = 12;
    int s_checked = 0;
    for (auto& s_reading : *this) {
      if (s_checked + NUM_COMMON_READINGS_REQ > _readings.size()) return false;
      s_checked += 1;

      for (auto& t_reading : t) {
        for (int x = 0; x < 48; x++) {
          // t_pos + t_reading = s_pos + s_reading
          // t_pos + t_reading = (0,0) + s_reading
          // t_pos = s_reading - t_reading
          const Pos t_pos = s_reading - t_reading.xformed(x);
          if (intersectsAt(t, /*at_least*/ NUM_COMMON_READINGS_REQ, t_pos, x)) {
            out_t_pos = t_pos;
            out_x = x;
            return true;
          }
        }
      }
    }
    return false;
  }

  void fixate(int x, const Pos& pos) {
    assert(!_pos.has_value());
    _pos = pos;
    PositionSet tmp;
    for (auto& r : _readings) {
      tmp.insert(pos + r.xformed(x));
    }
    _readings = std::move(tmp);
  }

  bool fixated() const { return _pos.has_value(); }

  int manhattanDist(const Scanner& t) {
    assert(_pos.has_value());
    assert(t._pos.has_value());
    return _pos->manhattanDist(*t._pos);
  }
};

std::vector<Scanner> scanners;

void fixateAllScanners() {
  // scanner 0 is fixated at the origin with the identity xform
  scanners[0].fixate(0, Pos(0, 0, 0));
  // the queue will all fixated scanners
  std::queue<int> q;
  q.push(0);

  while (!q.empty()) {
    const int s = q.front();
    q.pop();

    for (int t = 0; t < scanners.size(); t++) {
      if (scanners[t].fixated()) continue;
      assert(t != s);

      Pos t_pos;
      int x;
      if (scanners[s].canIntersect(scanners[t], t_pos, x)) {
        printf("align %d %d (xform=%d)\n", s, t, x);
        t_pos.dump();
        scanners[t].fixate(x, t_pos);
        q.push(t);
      }
    }
  }
}

PositionSet collectAllBeacons() {
  PositionSet beacon_set;
  for (auto& s : scanners) {
    assert(s.fixated());
    beacon_set.insert(s.begin(), s.end());
  }
  return beacon_set;
}

int main() {
  for (int s = 0;; s++) {
    int input_s;
    if (scanf("--- scanner %d ---\n", &input_s) < 0) {
      break;
    }
    Scanner scanner;
    for (;;) {
      Pos r;
      scanf("%d,%d,%d", &r.comp[0], &r.comp[1], &r.comp[2]);
      scanner.addReading(r);
      char c = getchar();
      if (c == '\n' || c == EOF) {
        c = getchar();
        if (c == '\n' || c == EOF) {
          scanners.push_back(scanner);
          break;
        }
        ungetc(c, stdin);
      }
    }
  }

  fixateAllScanners();

  const auto beacons = collectAllBeacons();
  printf("%zu beacons mapped\n", beacons.size());

  int max_manhattan_dist = 0;
  for (int i = 0; i < scanners.size(); i++) {
    for (int j = i + 1; j < scanners.size(); j++) {
      const int d = scanners[i].manhattanDist(scanners[j]);
      max_manhattan_dist = std::max(max_manhattan_dist, d);
    }
  }
  printf("maximum manhattan distance between scanners is %d\n",
         max_manhattan_dist);

  return 0;
}
