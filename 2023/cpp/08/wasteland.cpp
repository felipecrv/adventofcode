#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "lib.h"

char instructions[300];
int after_all_ins[1000];
int num_v = 0;

std::unordered_map<std::string, int> vertex;
std::unordered_map<int, std::pair<int, int>> next_by_dir;

static int intern_vertex(const char *name) {
  const int *v = lookup(vertex, name);
  if (v) return *v;
  return vertex.emplace(name, num_v++).first->second;
}

const char *name_of(int v) {
  for (auto &p : vertex) {
    if (p.second == v) return p.first.c_str();
  }
  return "???";
}

std::pair<int, int> num_cycles_to_any_dest(int v, std::vector<int> dests) {
  int num = 0;
  for (;;) {
    if (num > 0 && std::find(dests.begin(), dests.end(), v) != dests.end()) {
      return {v, num};
    }
    v = after_all_ins[v];
    num += 1;
  }
  return {v, -1};
}

long long count_cycles_to_dest(const std::vector<int> &sources,
                            const std::vector<int> &dests) {
  std::vector<int> num_cycles;
  for (int v : sources) {
    auto w = num_cycles_to_any_dest(v, dests);
    printf("%s -> %s = %d\n", name_of(v), name_of(w.first), w.second);
    num_cycles.push_back(w.second);
  }
  if (dests.size() > 1) {
    puts("");
    for (int v : dests) {
      auto w = num_cycles_to_any_dest(v, dests);
      printf("%s -> %s = %d\n", name_of(v), name_of(w.first), w.second);
    }
  }
  long long the_lcm = num_cycles[0];
  for (int i = 0; i < num_cycles.size(); i++) {
    the_lcm = lcm(the_lcm, num_cycles[i]);
  }
  return the_lcm;
}

int main() {
  scanf("%s\n\n", instructions);
  const int num_ins = strlen(instructions);

  // For part 1
  std::vector<int> source;
  std::vector<int> dest;

  // For part 2
  std::vector<int> sources;
  std::vector<int> dests;

  char id[16], lhs[16], rhs[16];
  while (scanf("%s = (%s %s", id, lhs, rhs) == 3) {
    id[3] = lhs[3] = rhs[3] = '\0';
    const int v = intern_vertex(id);
    const int left = intern_vertex(lhs);
    const int righ = intern_vertex(rhs);
    next_by_dir.insert({v, {left, righ}});
    // Populate the source and destination vertices for part 1 and 2.
    if (strcmp(id, "AAA") == 0) {
      source.push_back(v);
    } else if (strcmp(id, "ZZZ") == 0) {
      dest.push_back(v);
    }
    if (id[2] == 'A') {
      sources.push_back(v);
    } else if (id[2] == 'Z') {
      dests.push_back(v);
    }
  }
  printf("# instructions: %d\n", num_ins);
  // Cache the vertex after following all instructions from every vertex.
  for (int v = 0; v < num_v; v++) {
    int w = v;
    for (int i = 0; i < num_ins; i++) {
      const char c = instructions[i];
      const auto *next = lookup(next_by_dir, v);
      w = c == 'L' ? next->first : next->second;
    }
    after_all_ins[v] = w;
  }

  puts("Part 1");
  auto ans = count_cycles_to_dest(source, dest) * num_ins;
  printf("Answer: %lld\n", ans);

  puts("\nPart 2");
  ans = count_cycles_to_dest(sources, dests) * num_ins;
  printf("Answer: %lld\n", ans);
  return 0;
}
