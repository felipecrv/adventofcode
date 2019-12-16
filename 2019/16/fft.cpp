#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

int pattern(int i, int j) {
  switch (((j + 1) / (i + 1)) % 4) {
  case 0:
    return 0;
  case 1:
    return 1;
  case 2:
    return 0;
  case 3:
    return -1;
  }
  return 0;
}

void solve(const std::vector<int> &signal) {
  auto result = signal;
  auto tmp = result;
  const int len = (int)signal.size();

  std::vector<int> sum;
  sum.resize(len + 1);

  for (int phase = 1; phase <= 100; phase++) {
    // result = pattern X result (after phase)

    // SLOW
    // for (int i = 0; i < len; i++) {
    //   tmp[i] = 0;
    //   for (int j = 0; j < len; j++) {
    //     tmp[i] += pattern(i, j) * result[j];
    //   }
    // }

    sum[0] = 0;
    for (int i = 1; i <= len; i++) {
      sum[i] = sum[i - 1] + result[i - 1];
    }
    for (int i = 0; i < len; i++) {
      tmp[i] = 0;
      for (int j = 0, pat_len = 0; j < len; j += pat_len) {
        pat_len = (i + 1) - ((j + 1) % (i + 1));

        int pat = pattern(i, j);
        if (pat == 0) {
          continue;
        }

        const int partial_sum = sum[std::min(j + pat_len, len)] - sum[j];
        if (pat == 1) {
          tmp[i] += partial_sum;
        } else {
          tmp[i] -= partial_sum;
        }
      }
    }
    for (int i = 0; i < len; i++) {
      result[i] = abs(tmp[i]) % 10;
    }

    printf("result = pattern X result (after %d phase)\n", phase);
    if (phase == 100) {
      if (signal.size() < 1000) {
        for (int i = 0; i < len; i++) {
          putchar(result[i] + '0');
        }
        putchar('\n');
        putchar('\n');
      } else {
        /* clang-format off */
        int offset = signal[0] * 1000000
                   + signal[1] * 100000
                   + signal[2] * 10000
                   + signal[3] * 1000
                   + signal[4] * 100
                   + signal[5] * 10
                   + signal[6] * 1;
        /* clang-format on */
        printf("Result after offset (%d):\n", offset);
        for (int pos = 0; pos < 8; pos++) {
          putchar(result[offset + pos] + '0');
        }
        putchar('\n');
      }
    }
  }
}

int main() {
  std::vector<int> input;
  for (;;) {
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    }
    input.push_back(c - '0');
  }
  printf("Input length: %d\n", (int)input.size());

  solve(input);

  std::vector<int> large_signal;
  large_signal.reserve(10000 * input.size());
  puts("Reserved memory.");
  for (int i = 0; i < 10000; i++) {
    large_signal.insert(large_signal.end(), input.begin(), input.end());
  }
  puts("Built large signal.");
  solve(large_signal);

  return 0;
}
