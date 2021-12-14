#include <algorithm>
#include <cstdio>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include "../geom.h"
#include "../lib.h"

using Polymer = std::list<char>;
using Pair = std::pair<char, char>;
std::unordered_map<Pair, char> rules;

void dump(const Polymer& polymer) {
  for (auto p : polymer) putchar(p);
  putchar('\n');
}

void dumpStats(const Polymer& polymer) {
  int freq[26] = {0};
  for (auto p : polymer) {
    freq[(int)p - 'A'] += 1;
  }
  std::vector<std::pair<int, char>> stats;
  for (int i = 0; i < 26; i++) {
    if (freq[i] > 0) stats.emplace_back(freq[i], 'A' + i);
  }
  std::sort(stats.begin(), stats.end());
  assert(stats.size() >= 2);
  const int most = stats[stats.size() - 1].first;
  const int least = stats[0].first;
  printf("%d - %d = %d\n", most, least, most - least);
}

Polymer polymer;

void solve() {
  for (int i = 0; i < 10; i++) {
    std::list<char> new_polymer;
    auto it = polymer.begin();
    auto next = it;
    next++;
    for (;; ++it, ++next) {
      if (next == polymer.end()) {
        new_polymer.push_back(*it);
        break;
      } else {
        Pair rule(*it, *next);
        if (char* c = lookup(rules, rule)) {
          new_polymer.push_back(*it);
          new_polymer.push_back(*c);
        } else {
          new_polymer.push_back(*it);
        }
      }
    }
    polymer = std::move(new_polymer);
    printf("After step %d: ", i + 1);
    if (i < 6) {
      dump(polymer);
    } else {
      puts("...");
    }
  }
  dumpStats(polymer);
}

int main() {
  for (char c;;) {
    c = getchar();
  process_c:
    if (c >= 'A' && c <= 'Z') {
      polymer.push_back(c);
    } else if (c == '\n') {
      c = getchar();
      if (c == '\n')
        break;
      else
        goto process_c;
    }
  }

  for (char c;;) {
    char a = getchar();
    if (a == EOF) break;
    char b = getchar();

    // clang-format off
    c = getchar(); assert(c == ' ');
    c = getchar(); assert(c == '-');
    c = getchar(); assert(c == '>');
    c = getchar(); assert(c == ' ');
    // clang-format on

    c = getchar();
    rules.emplace(Pair(a, b), c);
    c = getchar();
    if (c == '\n') continue;
    if (c == EOF) break;
  }

  solve();
  return 0;
}
