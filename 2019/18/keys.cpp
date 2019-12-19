#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "lib.h"

struct MemoizationKey {
  MemoizationKey(uint32_t keychain, Vec pos) : keychain(keychain), pos(pos) {}

  bool operator==(const MemoizationKey &other) const {
    return keychain == other.keychain && pos == other.pos;
  }

  uint32_t keychain;
  Vec pos;
};

namespace std {

template <> struct hash<MemoizationKey> {
  size_t operator()(const MemoizationKey &s) const {
    size_t h = 0;
    hash_combine(h, s.keychain);
    hash_combine(h, s.pos);
    return h;
  }
};

} // namespace std

bool contains(const std::unordered_set<Vec> &s, Vec v) {
  return s.find(v) != s.end();
}

#define NORTH Vec(0, -1)
#define SOUTH Vec(0, 1)
#define WEST Vec(-1, 0)
#define EAST Vec(1, 0)

Vec cardinals[] = {
    NORTH, EAST, SOUTH, WEST,
};

struct NearKey {
  NearKey(char key, int dist, Vec pos) : key(key), dist(dist), pos(pos) {}

  char key;
  int dist;
  Vec pos;
};

#define MAXN 100

struct Vault {
  Vault() {
    memset(_keys_to_collect, 0, sizeof(_keys_to_collect));
    memset(_map, 0, sizeof(_map));
  }

  void set(int x, int y, char c) {
    assert(x < MAXN && y < MAXN);

    _max_x = std::max(_max_x, x);
    _max_y = std::max(_max_y, y);

    if (c == '@' && _start[0].isNull()) {
      _start[0] = Vec(x, y);
    }

    _map[y][x] = c;
  }

  void set(Vec pos, char c) { set(pos.x, pos.y, c); }

  int cell(int x, int y) const {
    if (x < 0 || x > _max_x || y < 0 || y > _max_y) {
      return '#';
    }
    return _map[y][x];
  }

  int cell(Vec pos) const { return cell(pos.x, pos.y); }

  void render() const {
    for (int y = 0; y <= _max_y; y++) {
      for (int x = 0; x <= _max_x; x++) {
        putchar(cell(x, y));
      }
      putchar('\n');
    }
    putchar('\n');

    for (int bot = 0; bot < _num_bots; bot++) {
      printf("keys_to_collect[%d]: %d\n", bot,
             __builtin_popcount(_keys_to_collect[bot]));
    }
  }

  uint32_t findReachableKeys(Vec from) const {
    uint32_t ret = 0;

    std::queue<Vec> q;
    q.push(from);
    std::unordered_set<Vec> visited;
    visited.insert(from);

    while (!q.empty()) {
      auto v = q.front();
      q.pop();

      char c = cell(v);
      if (c >= 'a' && c <= 'z') {
        ret = keychainWith(ret, c);
      }

      for (auto &dir : cardinals) {
        Vec w = v + dir;
        c = cell(w);
        if (c == '#') {
          continue;
        }
        if (contains(visited, w)) {
          continue;
        }
        visited.insert(w);

        if (c == '.' || c == '@') {
          q.push(w);
        } else if (c >= 'a' && c <= 'z') {
          q.push(w);
        } else if (c >= 'A' && c <= 'Z') {
          q.push(w);
        }
      }
    }

    return ret;
  }

  uint32_t keychainWith(uint32_t keychain, char key) const {
    return keychain | (0x1 << (key - 'a'));
  }

  bool containsKey(uint32_t keychain, char key) const {
    return keychain & (0x1 << (key - 'a'));
  }

  std::vector<NearKey> nearUncollectedKeys(uint32_t keychain, Vec from,
                                      int bot) const {
    std::unordered_map<Vec, int> dist;
    std::vector<NearKey> ret;

    std::queue<Vec> q;
    q.push(from);
    dist.insert(std::make_pair(from, 0));
    while (!q.empty()) {
      auto v = q.front();
      q.pop();

      char c = cell(v);
      if (c >= 'a' && c <= 'z') {
        if (!containsKey(keychain, c)) {
          // Found a uncollected key!
          ret.push_back(NearKey(c, dist[v], v));
          continue;
        }
      }

      for (auto &dir : cardinals) {
        Vec w = v + dir;
        c = cell(w);
        if (c == '#') {
          continue;
        }
        if (lookup(dist, w)) {
          continue; // visited
        }
        dist[w] = dist[v] + 1;

        if (c == '.' || c == '@') {
          q.push(w);
        } else if (c >= 'A' && c <= 'Z') {
          // another bot will find the key for door c if this the key that
          // opens this door is not part of the keys this bot has to collect
          char key = tolower(c);
          if ((_num_bots > 1 && !containsKey(_keys_to_collect[bot], key)) ||
              containsKey(keychain, key)) {
            q.push(w);
          }
        } else {
          assert(c >= 'a' && c <= 'z');
          q.push(w);
        }
      }
    }

    return ret;
  }

  // the minimum cost of collecting all keys reachable by `bot`
  // from `from` while holding all keys in `keychain`
  int minCostToAll(uint32_t keychain, Vec from, int bot) {
    if (keychain == _keys_to_collect[bot]) {
      // has all keys already
      return 0;
    }
    MemoizationKey memo_key(keychain, from);
    if (auto *memo = lookup(_memoized, memo_key)) {
      return *memo;
    }
    auto &memo = _memoized[memo_key];

    // find the nearest uncollected keys
    auto near_keys = nearUncollectedKeys(keychain, from, bot);

    int min = 0x3f3f3f3f;
    for (auto &near_key : near_keys) {
      int cost =
          near_key.dist +
          minCostToAll(keychainWith(keychain, near_key.key), near_key.pos, bot);
      min = std::min(min, cost);
    }

    return memo = min;
  }

  void splitInFour() {
    Vec center = _start[0];
    _start[0] = center + (NORTH + EAST);
    _start[1] = center + (SOUTH + EAST);
    _start[2] = center + (SOUTH + WEST);
    _start[3] = center + (NORTH + WEST);

    set(_start[0], '@');
    set(_start[1], '@');
    set(_start[2], '@');
    set(_start[3], '@');

    set(center, '#');
    set(center + NORTH, '#');
    set(center + EAST, '#');
    set(center + SOUTH, '#');
    set(center + WEST, '#');

    _num_bots = 4;
  }

  void explore() {
    for (int bot = 0; bot < _num_bots; bot++) {
      _keys_to_collect[bot] = findReachableKeys(_start[bot]);
    }
  }

  int collectAllKeys() {
    uint32_t keychain = 0;
    int min_total_cost = 0;
    for (int bot = 0; bot < _num_bots; bot++) {
      _memoized.clear();
      int min_cost_for_bot = minCostToAll(keychain, _start[bot], bot);
      printf("min_cost_for_bot %d\n", min_cost_for_bot);
      min_total_cost += min_cost_for_bot;
    }
    return min_total_cost;
  }

private:
  Vec _start[4];                // start position of the bot
  uint32_t _keys_to_collect[4]; // keys each bot has to collect
  int _num_bots = 1;

  int _max_x = 0;
  int _max_y = 0;
  int _map[MAXN][MAXN];

  std::unordered_map<MemoizationKey, int> _memoized;
};

Vault vault;

int main() {
  for (int y = 0, x = 0;;) {
    char c = getchar();
    if (c == '\n') {
      x = 0;
      y += 1;
      continue;
    }
    if (c == EOF) {
      break;
    }
    vault.set(x, y, c);
    x += 1;
  }

  vault.splitInFour(); // comment to get phase 1
  vault.explore();     // computes reachable keys

  vault.render();

  int cost = vault.collectAllKeys();
  printf("Walk %d cells to collect all keys\n", cost);

  return 0;
}
