#include <cstdio>
#include <cstring>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>

using namespace std;

unordered_map<string, int> name_to_id;
int next_id = 1;

#define MAXN 10000
int nadj[MAXN];
int adj[MAXN][MAXN];

const int COM = 1;

int count_table[MAXN];
int level[MAXN];
int parent[MAXN];

// count direct and indirect orbits from v
int count(int v) {
  auto &c = count_table[v];
  if (c != -1) {
    return c;
  }

  int sum = 0;
  for (int i = 0; i < nadj[v]; i++) {
    sum += 1 + count(adj[v][i]);
  }

  c = sum;
  return c;
}

void calculate_parents() {
  memset(parent, -1, sizeof(parent));

  queue<int> q;
  q.push(COM);
  parent[COM] = -1;

  while (!q.empty()) {
    int v = q.front();
    q.pop();

    for (int i = 0; i < nadj[v]; i++) {
      int w = adj[v][i];
      parent[w] = v;
      q.push(w);
    }
  }
}

int dist_to_santa(int from, int santa) {
  for (int par = santa, par_dist = 0; par >= 0; par = parent[par], par_dist++) {
    if (par == from) {
      return par_dist;
    }
  }
  return -1;
}

int main() {
  name_to_id["COM"] = COM;
  next_id = COM + 1;
  memset(nadj, 0, sizeof(nadj));

  char a[5];
  char b[5];
  while (scanf("%3s)%3s\n", a, b) == 2) {
    int id_a = name_to_id[a];
    int id_b = name_to_id[b];
    if (id_a == 0) {
      id_a = next_id++;
      name_to_id[a] = id_a;
    }
    if (id_b == 0) {
      id_b = next_id++;
      name_to_id[b] = id_b;
    }

    adj[id_a][nadj[id_a]++] = id_b;
  }

  // number of orbits and indirect orbits (part 1)
  memset(count_table, -1, sizeof(count_table));
  int sum = 0;
  for (int i = 1; i < next_id; i++) {
    sum += count(i);
  }
  printf("count: %d\n", sum);

  // minimum num of transfers (part 2)
  int YOU = name_to_id["YOU"];
  int SAN = name_to_id["SAN"];
  calculate_parents();
  int min_dist = numeric_limits<int>::max();
  for (int par = parent[YOU], par_dist = 1; par >= 0;
       par = parent[par], par_dist++) {
    int dist = dist_to_santa(par, SAN);
    if (dist >= 0) {
      dist += par_dist;
      min_dist = min(min_dist, dist);
    }
  }
  printf("min_transfers: %d\n", min_dist - 2);

  return 0;
}
