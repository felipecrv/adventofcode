#include <cstdio>
#include <cstdlib>
#include <cstring>

long long gcd(long long a, long long b) {
  if (a == 0) return b;
  return gcd(b % a, a);
}

long long lcm(long long a, long long b) { return (a * b) / gcd(a, b); }

struct Component {
  Component() {
    memset(pos, 0, sizeof(pos));
    memset(v, 0, sizeof(v));
  }

  int pos[4];
  int v[4];

  void step() {
    // apply gravity to change velocity
    for (int i = 0; i < 4; i++) {
      for (int j = i + 1; j < 4; j++) {
        if (pos[i] < pos[j]) {
          v[i] += 1;
          v[j] -= 1;
        } else if (pos[i] > pos[j]) {
          v[i] -= 1;
          v[j] += 1;
        }
      }
    }
    // apply velocity to change position
    for (int i = 0; i < 4; i++) {
      pos[i] += v[i];
    }
  }

  bool operator==(const Component &o) const {
    for (int i = 0; i < 4; i++) {
      if (pos[i] != o.pos[i] || v[i] != o.v[i]) {
        return false;
      }
    }
    return true;
  }
};

struct Moons {
  Component x;
  Component y;
  Component z;

  void step() {
    x.step();
    y.step();
    z.step();
  }

  int pot(int c) const { return abs(x.pos[c]) + abs(y.pos[c]) + abs(z.pos[c]); }
  int kin(int c) const { return abs(x.v[c]) + abs(y.v[c]) + abs(z.v[c]); }
  int e(int c) const { return pot(c) * kin(c); }
  int totalEnergy() const { return e(0) + e(1) + e(2) + e(3); }
};

long long cycleLength(Component x) {
  auto initial = x;

  for (long long steps = 1;; steps++) {
    x.step();
    if (x == initial) {
      return steps;
    }
  }
}

int main() {
  Moons moons;
  int m = 0;
  while (scanf("<x=%d, y=%d, z=%d>\n", &moons.x.pos[m], &moons.y.pos[m],
               &moons.z.pos[m]) > 0) {
    m++;
  }

  auto alpha = cycleLength(moons.x);
  auto beta = cycleLength(moons.y);
  auto gamma = cycleLength(moons.z);
  long long ret = lcm(alpha, lcm(beta, gamma));
  printf("cycle: %lld\n", ret);

  int max_steps = 1000;
  for (int i = 1; i <= max_steps; i++) {
    moons.step();
  }
  printf("total energy after %d steps: %d\n", max_steps, moons.totalEnergy());

  return 0;
}
