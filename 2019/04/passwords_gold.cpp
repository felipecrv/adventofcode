#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

bool is_valid(const char *s) {
  int count[10];
  memset(count, 0, sizeof(count));

  for (int j = 0; j < 6; j++) {
    // ensure sorted
    if (j > 0 && s[j] < s[j - 1]) {
      return false;
    }
    // count occurrences (all clustered together)
    int d = s[j] - '0';
    count[d] += 1;
  }

  bool has_duplicate = false;
  for (int j = 0; j < 6; j++) {
    int d = s[j] - '0';
    if (count[d] == 2) {
      has_duplicate = true;
    }
  }
  return has_duplicate;
}

int main() {
  // my tests
  assert(is_valid("136778"));
  assert(is_valid("122233"));
  assert(!is_valid("144445"));

  // from problem description
  assert(is_valid("112233"));
  assert(!is_valid("123444"));
  assert(is_valid("111122"));

  char as[10];
  char bs[10];

  scanf("%6s-%6s\n", as, bs);
  int a = atoi(as);
  int b = atoi(bs);

  printf("%06d-%06d\n", a, b);

  char is[10];
  int count = 0;
  for (int i = a; i <= b; i++) {
    sprintf(is, "%06d", i);
    printf("%s\n", is);
    bool valid = is_valid(is);
    if (valid) {
      count += 1;
    }
  }
  printf("%d\n", count);

  return 0;
}
