#include <cstdio>

int main() {
  int last_sample;
  int sample;
  int increases = 0;
  scanf("%d\n", &last_sample);
  while (scanf("%d", &sample) == 1) {
    if (sample > last_sample) increases++;
    last_sample = sample;
  }
  printf("%d\n", increases);
  return 0;
}
