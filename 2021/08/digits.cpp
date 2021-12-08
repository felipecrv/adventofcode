#include <array>
#include <cassert>
#include <cstdio>
#include <vector>

using namespace std;

// clang-format off
//   0:      1:      2:      3:      4:
//  aaaa    ....    aaaa    aaaa    ....
// b    c  .    c  .    c  .    c  b    c
// b    c  .    c  .    c  .    c  b    c
//  ....    ....    dddd    dddd    dddd
// e    f  .    f  e    .  .    f  .    f
// e    f  .    f  e    .  .    f  .    f
//  gggg    ....    gggg    gggg    ....

//   5:      6:      7:      8:      9:
//  aaaa    aaaa    aaaa    aaaa    aaaa
// b    .  b    .  .    c  b    c  b    c
// b    .  b    .  .    c  b    c  b    c
//  dddd    dddd    ....    dddd    dddd
// .    f  e    f  .    f  e    f  .    f
// .    f  e    f  .    f  e    f  .    f
//  gggg    gggg    ....    gggg    gggg
const int digits[10][7] = {
  { 1,
  1,  1,
    0,
  1,  1,
    1
  },{0,
   0,  1,
     0,
   0,  1,
     0,
  },{1,
   0,  1,
     1,
   1,  0,
     1
  },{1,
   0,  1,
     1,
   0,  1,
     1
  },{0,
   1,  1,
     1,
   0,  1,
     0
  },{1,
   1,  0,
     1,
   0,  1,
     1
  },{1,
   1,  0,
     1,
   1,  1,
     1
  },{1,
   0,  1,
     0,
   0,  1,
     0
  },{1,
   1,  1,
     1,
   1,  1,
     1
  },{1,
   1,  1,
     1,
   0,  1,
     1
  }
};
// clang-format on

#define FOR_ALL_WIRES for (int w = 0; w < 7; w++)
#define FOR_ALL_DIGITS for (int d = 0; d < 10; d++)

using Wiring = array<int, 7>;  // wiring[wire] = segment

struct Display {
  explicit Display(const char *display_chars) {  // e.g. "fdgacbe"
    count = strlen(display_chars);

    memset(segments, false, sizeof(segments));
    for (int i = 0; i < count; i++) {
      segments[display_chars[i] - 'a'] = true;
    }
  }

  bool encodesDigit(const Wiring &wiring, int d) const {
    FOR_ALL_WIRES {
      if (segments[wiring[w]] != (bool)digits[d][w]) {
        return false;
      }
    }
    return true;
  }

  int encodedDigit(const Wiring &wiring) const {
    FOR_ALL_DIGITS {
      if (encodesDigit(wiring, d)) {
        return d;
      }
    }
    assert(false && "unreachable");
    return -1;
  }

  int count;
  bool segments[7];
};

vector<Display> displays;

bool isValidWiring(const Wiring &wiring) {
  for (auto &display : displays) {
    FOR_ALL_DIGITS {
      if (display.encodesDigit(wiring, d)) {
        goto next_display;
      }
    }
    return false;
  next_display:
    continue;
  }
  return true;
}

bool findValidWiring(Wiring &out_wiring) {
  out_wiring = {0, 1, 2, 3, 4, 5, 6};  // wiring[wire] = segment
  while (next_permutation(out_wiring.begin(), out_wiring.end())) {
    if (isValidWiring(out_wiring)) {
      return true;
    }
  }
  return false;
}

int main() {
  int num_easy_digits = 0;
  int sum_of_full_number = 0;

  char buf[32];
  while (scanf("%32s", buf) == 1) {
    if (buf[0] != '|') {
      displays.emplace_back(buf);
    } else {
      Wiring wiring;
      if (!findValidWiring(wiring)) {
        assert(false && "there should always be a valid wiring");
        return 1;
      }
      int full_number = 0;
      for (int i = 0; i < 4 && scanf("%32s", buf); i++) {
        Display display(buf);
        const int digit = display.encodedDigit(wiring);
        printf("%7s: %d\n", buf, digit);

        // count easy digits
        switch (digit) {
          case 1:
          case 4:
          case 7:
          case 8:
            num_easy_digits += 1;
            break;
        }

        // build the 4 digit decimal number
        full_number *= 10;
        full_number += digit;
      }
      sum_of_full_number += full_number;
      displays.clear();

      putchar('\n');
    }
  }

  printf("number of easy digits: %d\n", num_easy_digits);
  printf("  sum of full numbers: %d\n", sum_of_full_number);

  return 0;
}
