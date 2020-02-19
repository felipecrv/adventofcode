#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "lib.h"

using namespace std;

#define SIZE 10007
#define BIG_SIZE 119315717514047

using Int = __int128;

enum Kind { DEAL_INCREMENT, NEW_STACK, CUT } Kind;

Int sum(Int x, Int y, Int size) {
  assert(x >= 0);
  assert(y >= 0);
  assert(x < size);
  assert(y < size);
  return (x + y) % size;
}

Int product(Int x, Int y, Int size) {
  assert(x >= 0);
  assert(y >= 0);
  assert(x < size);
  assert(y < size);
  return (x * y) % size;
}

Int power(unordered_map<Int, Int> &memo, Int exponent, Int size) {
  if (auto *p = lookup(memo, exponent)) {
    return *p;
  }
  auto &p = memo[exponent];

  Int lhs = power(memo, exponent / 2, size);
  Int rhs = power(memo, exponent - (exponent / 2), size);
  return p = product(lhs, rhs, size);
}

Int power(Int base, Int exponent, Int size) {
  Int sign = 1;
  if (base < 0) {
    base = -base;
    if (exponent % 2 == 1) {
      sign = -1;
    }
  }

  assert(base < size);
  assert(exponent < std::numeric_limits<long long>::max());

  unordered_map<Int, Int> memo;
  memo[0] = 1;
  memo[1] = base;  // sign will be fixed here
  return sign * power(memo, exponent, size);
}

// C function for extended Euclidean Algorithm
Int gcdExtended(Int a, Int b, Int *out_x, Int *out_y);

// Function to find modulo inverse of a
Int multiplicativeInverse(Int a, Int m) {
  Int x;
  Int y;
  Int g = gcdExtended(a, m, &x, &y);
  if (g == 1) {
    // m is added to handle negative x
    return (x % m + m) % m;
  }
  assert(false && "inverse doesn't exist");
  return 0;
}

// C function for extended Euclidean Algorithm
Int gcdExtended(Int a, Int b, Int *x, Int *y) {
  // Base Case
  if (a == 0) {
    *x = 0;
    *y = 1;
    return b;
  }

  Int x1, y1;  // To store results of recursive call
  Int gcd = gcdExtended(b % a, a, &x1, &y1);

  // Update x and y using results of recursive
  // call
  *x = y1 - (b / a) * x1;
  *y = x1;

  return gcd;
}

struct Operation {
  Operation(Int m, Int a) : m(m), a(a) {}
  Operation() : Operation(1, 0) {}

  static Operation Invert(Int size) {
    Operation op(1, 0);
    op.invert();
    return op;
  }
  static Operation Add(Int by) { return Operation(1, by); }
  static Operation Multiply(Int by) { return Operation(by, 0); }

  void invert() { inverted = !inverted; }

  void combine(Operation op, Int size) {
    if (op.inverted) {
      assert(op.m == 1);
      assert(op.a == 0);
      invert();
    }

    // m' = m * op.m
    m = product(m, op.m, size);
    if (inverted) {
      // a' = (s - 1) - op.a
      //      - op.m * [(s - 1) - a]
      auto fst = (size - 1) - op.a;
      auto lst = product(op.m, (size - 1) - a, size);
      a = fst - lst;
      if (a < 0) {
        a += size;
      }
      assert(a >= 0);
      assert(a < size);
    } else {
      // a' = a * op.m + op.a
      a = sum(product(a, op.m, size), op.a, size);
    }
  }

  void repeatRecursive(Int k, Int size) {
    assert(k >= 1);
    if (k == 1) {
      return;
    }

    if (k % 2 == 1) {
      Operation self = *this;
      repeatRecursive(k - 1, size);
      combine(self, size);
      return;
    }

    repeatRecursive(k / 2, size);
    combine(*this, size);
    return;
  }

  Operation repeatRecursiveMemoized(unordered_map<Int, Operation> &memo, Int k,
                                    Int size) {
    assert(k >= 1);
    if (auto *op = lookup(memo, k)) {
      return *op;
    }
    auto &op = memo[k];

    if (k % 2 == 1) {
      Operation repeated = repeatRecursiveMemoized(memo, k - 1, size);
      Operation one = repeatRecursiveMemoized(memo, 1, size);
      repeated.combine(one, size);
      return op = repeated;
    }

    Operation repeated = repeatRecursiveMemoized(memo, k / 2, size);
    repeated.combine(repeated, size);
    return op = repeated;
  }

  void repeatRecursiveMemoized(Int k, Int size) {
    unordered_map<Int, Operation> memo;
    memo[1] = *this;
    *this = repeatRecursiveMemoized(memo, k, size);
  }

  void repeatNonInverted(Int k, Int size) {
    assert(k >= 1);
    assert(!inverted);

    Int raised_to_k_minus_1 = power(m, k - 1, size);
    a = sum(product(a, raised_to_k_minus_1, size),  // a * m^(k - 1) +
            product((k - 1) % size, a, size),       // (k - 1) * a
            size);
    m = product(raised_to_k_minus_1, m, size);  // m^k
  }

  void repeat(Int k, Int size) {
    assert(k >= 1);
    if (k == 1) {
      return;
    }

    if (k % 2 == 1) {
      Operation self = *this;
      repeat(k - 1, size);
      combine(self, size);
      return;
    }

    combine(*this, size);  // combine with itself to remove potential inversions
    repeatNonInverted(k / 2, size);
  }

  Int apply(Int x, Int size) {
    Int i = sum(product(m, x, size), a, size);
    if (inverted) {
      assert(i < size);
      return size - 1 - i;
    }
    return i;
  }

  static Int _unapply(Int m, Int a, Int y, Int size) {
    y -= a;
    if (y < size) {
      y += size;
    }
    assert(y < size);
    assert(m < size);

    return product(multiplicativeInverse(m, size), y, size);
  }

  Int unapply(Int y, Int size) {
    if (inverted) {
      return _unapply(m, a, size - y - 1, size);
    }
    return _unapply(m, a, y, size);
  }

  bool inverted = false;
  Int m = 1;
  Int a = 0;
};

struct Deal {
  Deal(enum Kind kind, int val) : kind(kind), val(val) {}

  Int cut(Int size) const {
    assert(kind == CUT);
    if (val < 0) {
      return size + val;
    }
    return val;
  }

  Int simulate(Int i, Int size) {
    if (kind == NEW_STACK) {
      return size - i - 1;
    }
    if (kind == CUT) {
      return (i + size - cut(size)) % size;
    }
    if (kind == DEAL_INCREMENT) {
      return (i * val) % size;
    }
    return 0;
  }

  // convert to an Operation
  Operation operation(Int size) const {
    if (kind == NEW_STACK) {
      return Operation::Invert(size);
    }
    if (kind == CUT) {
      return Operation::Add(size - cut(size));
    }
    if (kind == DEAL_INCREMENT) {
      return Operation::Multiply(val);
    }
    assert(false);
    return Operation(1, 0);
  }

  enum Kind kind;
  int val = 0;
};

vector<Deal> phases;

Int finalPositionNaive(Int x, Int size) {
  for (auto &phase : phases) {
    x = phase.simulate(x, size);
  }
  return x;
}

Int finalPositionNaive(Int x, Int size, Int k) {
  for (int i = 0; i < k; i++) {
    x = finalPositionNaive(x, size);
  }
  return x;
}

Operation combineAllPhases(Int size, Int k) {
  Operation op(1, 0);
  for (auto &phase : phases) {
    op.combine(phase.operation(size), size);
  }
  op.repeatRecursiveMemoized(k, size);

  /*Operation all = op;
  for (int i = 0; i < k - 1; i++) {
    op.combine(all, size);
  }
  */
  return op;
}

Int finalPosition(Int x, Int size) {
  Operation op = combineAllPhases(size, 1);
  return op.apply(x, size);
}

Int finalPosition(Int x, Int size, Int k) {
  Operation op = combineAllPhases(size, k);
  return op.apply(x, size);
}

Int startPositionOfCardAt(Int y, Int size, Int k) {
  Operation op = combineAllPhases(size, k);
  return op.unapply(y, size);
}

int main() {
  int r;
  for (;;) {
    char s[10];
    r = scanf("%s", s);
    if (r != 1) {
      break;
    }
    if (strcmp(s, "deal") == 0) {
      r = scanf(" %s", s);
      assert(r == 1);
      if (strcmp(s, "with") == 0) {
        int increment = 0;
        r = scanf(" increment %d", &increment);
        assert(r == 1);
        phases.emplace_back(DEAL_INCREMENT, increment);
      } else {
        r = scanf(" into new stack");
        assert(r == 0);
        phases.emplace_back(NEW_STACK, 0);
      }
    } else if (strcmp(s, "cut") == 0) {
      int cut = 0;
      r = scanf("%d", &cut);
      assert(r == 1);
      phases.emplace_back(CUT, cut);
    }
  }

  for (auto &phase : phases) {
    if (phase.kind == DEAL_INCREMENT) {
      printf("deal with increment %d\n", phase.val);
    } else if (phase.kind == NEW_STACK) {
      printf("deal into new stack\n");
    } else if (phase.kind == CUT) {
      printf("cut %d\n", phase.val);
    }
  }

  Int final_pos_naive_small_deck = finalPositionNaive(2019, SIZE);
  printf("final of 2019 in the small deck: %lld\n",
         (long long)final_pos_naive_small_deck);
  // 5540

  Int final_pos_naive_big_deck = finalPositionNaive(2019, BIG_SIZE);
  printf("final of 2019 in the big deck: %lld\n",
         (long long)final_pos_naive_big_deck);
  // 73435930699064

  assert(power(-1, 0, 10) == 1);
  assert(power(-1, 1, 10) == -1);
  assert(power(-1, 2, 10) == 1);
  assert(power(-2, 0, 10) == 1);
  assert(power(-2, 1, 10) == -2);
  assert(power(-2, 2, 10) == 4);

  assert(power(3, 2, 100) == 9);
  assert(power(3, 3, 100) == 27);
  assert(power(3, 4, 100) == 81);
  assert(power(3, 8, 100) == (81 * 81) % 100);
  assert(power(3, 9, 100) == (81 * 81 * 3) % 100);
  assert(power(3, 10, 37) == (81 * 81 * 3 * 3) % 37);

  /* clang-format off */
  assert(finalPositionNaive(2019, BIG_SIZE, 1) == finalPosition(2019, BIG_SIZE, 1));
  assert(finalPositionNaive(2019, BIG_SIZE, 2) == finalPosition(2019, BIG_SIZE, 2));
  assert(finalPositionNaive(2019, BIG_SIZE, 3) == finalPosition(2019, BIG_SIZE, 3));
  assert(finalPositionNaive(2019, BIG_SIZE, 4) == finalPosition(2019, BIG_SIZE, 4));
  assert(finalPositionNaive(2019, BIG_SIZE, 5) == finalPosition(2019, BIG_SIZE, 5));
  assert(finalPositionNaive(2019, BIG_SIZE, 6) == finalPosition(2019, BIG_SIZE, 6));
  // assert(finalPositionNaive(2019, BIG_SIZE, 7) == finalPosition(2019, BIG_SIZE, 7));
  // assert(finalPositionNaive(2019, BIG_SIZE, 8) == finalPosition(2019, BIG_SIZE, 8));
  // assert(finalPositionNaive(2019, BIG_SIZE, 10) == finalPosition(2019, BIG_SIZE, 10));
  // assert(finalPositionNaive(2019, BIG_SIZE, 15) == finalPosition(2019, BIG_SIZE, 15));
  // assert(finalPositionNaive(2019, BIG_SIZE, 25) == finalPosition(2019, BIG_SIZE, 25));
  // assert(finalPositionNaive(2019, BIG_SIZE, 50) == finalPosition(2019, BIG_SIZE, 50));
  // assert(finalPositionNaive(2019, BIG_SIZE, 100) == finalPosition(2019, BIG_SIZE, 100));
  // assert(finalPositionNaive(2019, BIG_SIZE, 1000) == finalPosition(2019, BIG_SIZE, 1000));
  // assert(finalPositionNaive(2019, BIG_SIZE, 10000000) == finalPosition(2019, BIG_SIZE, 10000000));
  /* clang-format on */

  Int start_pos = startPositionOfCardAt(2020, BIG_SIZE, 101741582076661);
  printf("Start position of card at 2020 in the big deck: %lld\n",
         (long long)start_pos);

  // Int pos = solve0(2019, SIZE);
  // Int pos = solve1(2019, SIZE);
  // Int pos = solve2(2019, BIG_SIZE);
  // Int pos = solve3(2019, BIG_SIZE);
  // Int pos = solve4(2019, BIG_SIZE, 3);

  // 5118515659511278640 too high
  // 119255314568184 too high
  return 0;
}
