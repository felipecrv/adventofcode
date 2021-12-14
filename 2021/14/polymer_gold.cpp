#include <cstdio>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../lib.h"

using Pair = std::pair<char, char>;
using Int = long long;

std::string initial;
std::unordered_map<Pair, char> rules;
std::unordered_map<Pair, long long> pairs;

long long pairFreq(const Pair &p) {
  const long long *f = lookup(pairs, p);
  return f ? *f : 0;
}

void count() {
  int freq[26] = {0};
  for (auto &[p, f] : pairs) {
    freq[(int)p.first - 'A'] += f;
  }
  freq[(int)initial[initial.size() - 1] - 'A'] += 1;
  std::vector<std::pair<long long, char>> counts;
  for (int i = 0; i < 26; i++) {
    if (freq[i] > 0) counts.emplace_back(freq[i], i + 'A');
  }
  std::sort(counts.begin(), counts.end());
  const long long least = counts[0].first;
  const long long most = counts[counts.size() - 1].first;
  printf("%lld - %lld = %lld\n", most, least, most - least);
}

void solve() {
  for (int i = 0; i + 1 < initial.size(); i++) {
    pairs[Pair(initial[i], initial[i + 1])] += 1;
  }

  for (int i = 1; i <= 40; i++) {
    auto new_pairs = pairs;
    for (auto &[rule, c] : rules) {
      auto f = pairFreq(rule);
      if (f > 0) {
        new_pairs[rule] -= f;
        new_pairs[Pair(rule.first, c)] += f;
        new_pairs[Pair(c, rule.second)] += f;
      }
    }
    pairs = std::move(new_pairs);
    if (i == 10) {
      count();
    }
  }
  count();
}

int main() {
  for (char c;;) {
    c = getchar();
  process_c:
    if (c >= 'A' && c <= 'Z') {
      initial.push_back(c);
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
