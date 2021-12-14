#include <cassert>
#include <cstdio>
#include <cstring>
#include <stack>
#include <string>
#include <vector>

const int MAXN = 32;
const int START = 0;
const int END = 1;
int n;                // num caves
int nadj[MAXN];       // num adj. caves
int adj[MAXN][MAXN];  // adj. caves
bool is_small[MAXN];
std::vector<std::string> name_of;

int id(const std::string &name) {
  int i = 0;
  for (; i < name_of.size(); i++) {
    if (name_of[i] == name) return i;
  }
  name_of.push_back(name);
  assert(i < MAXN);
  return i;
}

struct DFS {
  DFS(uint32_t is_visited, bool has_extra_visit, int through)
      : is_visited(is_visited),
        has_extra_visit(has_extra_visit),
        through(through) {}

  const uint32_t is_visited;
  const bool has_extra_visit;
  const int through;
};

std::vector<DFS> stack;

long long numOfPathsFromStartToEnd(bool has_extra_visit) {
  stack.emplace_back(/*is_visited*/ 0, has_extra_visit, /*through*/ START);

  long long num_paths = 0;
  while (!stack.empty()) {
    const auto s = stack.back();
    stack.pop_back();

    if (s.through == END) {
      num_paths += 1;
      continue;
    }

    const uint32_t is_visited =
        is_small[s.through] ? (s.is_visited | 1u << s.through) : s.is_visited;
    for (int i = 0; i < nadj[s.through]; i++) {
      const int next = adj[s.through][i];
      const bool visited_next = ((is_visited >> next) & 1u);
      if (!visited_next) {
        stack.emplace_back(is_visited, s.has_extra_visit, next);
      } else if (s.has_extra_visit && next != START) {
        stack.emplace_back(is_visited, false, next);
      }
    }
  }
  return num_paths;
}

int main() {
  memset(nadj, 0, sizeof(nadj));
  memset(is_small, 0, sizeof(is_small));

  is_small[id("start")] = true;
  is_small[id("end")] = true;
  assert(id("start") == START);
  assert(id("end") == END);

  char edge[64];
  while (scanf("%s\n", edge) != EOF) {
    char *sep = strchr(edge, '-');
    assert(sep);
    *sep = '\0';
    const char *name_a = edge;
    const char *name_b = sep + 1;
    const int a = id(name_a);
    const int b = id(name_b);
    adj[a][nadj[a]++] = b;
    adj[b][nadj[b]++] = a;
    is_small[a] = (*name_a >= 'a');
    is_small[b] = (*name_b >= 'a');
  }
  n = name_of.size();

  printf("      number of caves: %d\n", n);
  printf("number of valid paths: %lld\n",
         numOfPathsFromStartToEnd(/*has_extra_visit*/ false));
  printf("number of valid paths: %lld (w/ extra visit)\n",
         numOfPathsFromStartToEnd(/*has_extra_visit*/ true));

  return 0;
}
