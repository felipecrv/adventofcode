#include <cstdio>
#include <cstring>
#include <cstdlib>

void next_smallest_password(char *s) {
  for (int i = 5; i > 0; i--) {
    if (s[i] < s[i - 1]) {
      for (int j = i; j < 6; j++) {
        s[j] = s[i - 1];
      }
    }
  }
  int count[10] = {0};
  for (int j = 0; j < 6; j++) {
    count[s[j] - '0'] += 1;
    if (count[s[j] - '0'] > 1) {
      return;
    }
  }
  s[4] = s[5];
}

int main() {
  char as[10];
  char bs[10];
  char is[10];

  scanf("%6s-%6s\n", as, bs);

  int a = atoi(as);
  int b = atoi(bs);
  next_smallest_password(as);

  int count = 0;
  for (int i = a; i <= b;) {
    count++;
    i++;
    sprintf(is, "%06d", i);
    next_smallest_password(is);
    i = atoi(is);
  }
  printf("%d\n", count);

  return 0;
}
