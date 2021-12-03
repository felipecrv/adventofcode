#include <cstdio>
#include <cstring>

int main() {
  char reading[64 + 1];
  int freq[64] = {0};
  int num_readings = 0;
  int num_bits;
  while (scanf("%64s", reading) == 1) {
    num_readings += 1;
    num_bits = strlen(reading);
    for (int i = 0; i < num_bits; i++) {
      const char c = reading[num_bits - 1 - i];
      freq[i] += c - '0';
    }
  }

  long long gamma = 0;
  long long epsilon = 0;
  for (int i = 0; i < num_bits; i++) {
    const int winner = (freq[i] > num_readings - freq[i]) ? 0x1 : 0x0;
    gamma |= (winner << i);
    epsilon |= (!winner << i);
  }

  printf("%lld * %lld = %lld\n", gamma, epsilon, gamma * epsilon);
  return 0;
}
