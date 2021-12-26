#include <algorithm>
#include <cassert>
#include <cstdio>
#include <memory>
#include <utility>
#include <vector>

class Pair {
 public:
  std::unique_ptr<Pair> left;
  std::unique_ptr<Pair> right;
  int value;

  Pair* parent = nullptr;

  Pair(std::unique_ptr<Pair>&& l, std::unique_ptr<Pair>&& r)
      : left(std::move(l)), right(std::move(r)), value(0) {
    left->parent = this;
    right->parent = this;
  }

  explicit Pair(int value) : left(nullptr), right(nullptr), value(value) {}

  bool isValue() const { return !left; }

  long long magnitude() const {
    return isValue() ? value : 3 * left->magnitude() + 2 * right->magnitude();
  }

  bool reduce() {
    bool reduced = false;
    while (explode() || split()) reduced = true;
    return reduced;
  }

  bool split() {
    return (left && left->split()) || (right && right->split()) ||
           (value > 9 && parent->split(this));
  }

  bool split(const Pair* referential) {
    printf("split: ");
    referential->dump();

    const int v = referential->value;
    auto p = std::make_unique<Pair>(std::make_unique<Pair>(v / 2),
                                    std::make_unique<Pair>(v - (v / 2)));
    p->parent = this;
    if (referential == left.get()) {
      left = std::move(p);
    } else {
      right = std::move(p);
    }
    return true;
  }

  bool explode() { return explode(0); }

  bool explode(int level) {
    if (level == 4 && left) {
      printf("explode: ");
      dump();

      assert(left->isValue() && right->isValue());
      assert(parent);
      parent->sendLeft(this, left->value);
      parent->sendRight(this, right->value);
      parent->clear(this);
      return true;
    }
    return (left && left->explode(level + 1)) ||
           (right && right->explode(level + 1));
  }

  void sendLeft(const Pair* referential, int value) {
    if (referential == right.get()) {
      left->sendRighmost(value);
    } else if (parent) {
      assert(left.get() == referential);
      parent->sendLeft(this, value);
    }
  }

  void sendRight(const Pair* referential, int value) {
    if (referential == left.get()) {
      right->sendLeftmost(value);
    } else if (parent) {
      parent->sendRight(this, value);
    }
  }

  void sendLeftmost(int value) {
    if (left) {
      left->sendLeftmost(value);
    } else {
      this->value += value;
    }
  }

  void sendRighmost(int value) {
    if (right) {
      right->sendRighmost(value);
    } else {
      this->value += value;
    }
  }

  void clear(const Pair* referential) {
    auto zero = std::make_unique<Pair>(0);
    zero->parent = this;
    if (left.get() == referential) {
      left = std::move(zero);
    } else {
      assert(referential == right.get());
      right = std::move(zero);
    }
  }

  void dump() const {
    dump(parent);
    putchar('\n');
  }

  void dump(const Pair* p) const {
    assert(parent == p);
    if (left && right) {
      putchar('[');
      left->dump(this);
      putchar(',');
      right->dump(this);
      putchar(']');
    } else {
      printf("%d", value);
    }
  }

  std::unique_ptr<Pair> clone() {
    if (isValue()) {
      return std::make_unique<Pair>(value);
    }
    auto ret = std::make_unique<Pair>(left->clone(), right->clone());
    return ret;
  }
};

std::unique_ptr<Pair> sum(std::unique_ptr<Pair>&& a,
                          std::unique_ptr<Pair>&& b) {
  a->dump(nullptr);
  printf(" +\n");
  b->dump(nullptr);
  printf(" =\n");
  auto s = std::make_unique<Pair>(std::move(a), std::move(b));
  s->reduce();
  s->dump();
  return s;
}

std::unique_ptr<Pair> parsePair() {
  char c = getchar();
  if (c == '[') {
    auto left = parsePair();
    c = getchar();
    assert(c == ',');
    auto right = parsePair();
    c = getchar();
    assert(c == ']');
    auto pair = std::make_unique<Pair>(std::move(left), std::move(right));
    return pair;
  }

  assert(c >= '0' && c <= '9');
  return std::make_unique<Pair>(c - '0');
}

int main() {
  std::vector<std::unique_ptr<Pair>> numbers;

  for (;;) {
    char c = getchar();
    if (c == EOF) {
      break;
    }
    ungetc(c, stdin);

    auto p = parsePair();
    c = getchar();
    if (c == '\n') {
      p->dump();
      if (p->reduce()) {
        p->dump();
      }
      numbers.push_back(std::move(p));
      putchar('\n');
      continue;
    }
    if (c == EOF) {
      break;
    }
  }

  auto s = numbers[0]->clone();
  for (int i = 1; i < numbers.size(); i++) {
    s = sum(std::move(s), numbers[i]->clone());
    putchar('\n');
  }
  printf("\nsum: ");
  s->dump();

  printf("magnitude: %lld\n", s->magnitude());

  long long max_magnitude = 0;
  for (int i = 0; i < numbers.size(); i++) {
    for (int j = 0; j < numbers.size(); j++) {
      if (i == j) continue;
      auto s = sum(numbers[i]->clone(), numbers[j]->clone());
      max_magnitude = std::max(max_magnitude, s->magnitude());
    }
  }
  printf("maximum magnitude of pair sum: %lld\n", max_magnitude);

  return 0;
}
