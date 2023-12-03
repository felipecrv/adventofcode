#include <cassert>
#include <cstdint>
#include <cstdio>
#include <string>

int64_t reference_impl(const char *in) {
  bool first_digit_found = false;
  int64_t sum = 0;
  int last_digit = 0;

#define ON_DIGIT(digit)                       \
  last_digit = (digit);                       \
  sum += 10 * (digit) * !first_digit_found; \
  first_digit_found = true;

  for (int i = 0;; i++) {
    const char c = in[i];
    switch (c) {
      case '\0':
        goto done;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        ON_DIGIT(c - '0');
        break;
      case '\n':
        sum += last_digit;
        first_digit_found = false;
        break;
      case 'o':
        if (in[i + 1] == 'n' && in[i + 2] == 'e') {
          ON_DIGIT(1);
          i += 1;  // leave trailing 'e'
        }
        break;
      case 't':
        if (in[i + 1] == 'w') {
          if (in[i + 2] == 'o') {
            ON_DIGIT(2);
            i += 1;  // leave trailing 'o'
          }
        } else if (in[i + 1] == 'h') {
          if (in[i + 2] == 'r' && in[i + 3] == 'e' && in[i + 4] == 'e') {
            ON_DIGIT(3);
            i += 3;  // leave trailing 'e'
          }
        }
        break;
      case 'f':
        if (in[i + 1] == 'o') {
          if (in[i + 2] == 'u' && in[i + 3] == 'r') {
            ON_DIGIT(4);
            i += 3;
          }
        } else if (in[i + 1] == 'i') {
          if (in[i + 2] == 'v' && in[i + 3] == 'e') {
            ON_DIGIT(5);
            i += 2;  // leave trailing 'e'
          }
        }
        break;
      case 's':
        if (in[i + 1] == 'i') {
          if (in[i + 2] == 'x') {
            ON_DIGIT(6);
            i += 2;
          }
        } else if (in[i + 1] == 'e') {
          if (in[i + 2] == 'v' && in[i + 3] == 'e' && in[i + 4] == 'n') {
            ON_DIGIT(7);
            i += 3;  // leave trailing 'n'
          }
        }
        break;
      case 'e':
        if (in[i + 1] == 'i' && in[i + 2] == 'g' && in[i + 3] == 'h' &&
            in[i + 4] == 't') {
          ON_DIGIT(8);
          i += 3;  // leave trailing 't'
        }
        break;
      case 'n':
        if (in[i + 1] == 'i' && in[i + 2] == 'n' && in[i + 3] == 'e') {
          ON_DIGIT(9);
          i += 2;  // leave trailing 'e'
        }
        break;
    }
  }
#undef ON_DIGIT
done:
  return sum;
}

int run(const std::string &input) {
  const int64_t sum = reference_impl(input.c_str());
  printf("%lld\n", sum);
  return 0;
}

int main(int argc, char **argv) {
  std::string input;
  int ret = 0;
  int64_t file_size = 0;
  const char *err = "fopen";
  FILE *file = fopen(argv[1], "r");
  if (!file) {
    goto error;
  }
  if (fseek(file, 0, SEEK_END) != 0) {
    err = "fseek";
    goto error;
  }
  file_size = ftell(file);
  if (file_size < 0) {
    err = "ftell";
    goto error;
  }
  if (fseek(file, 0, SEEK_SET) != 0) {
    err = "fseek";
    goto error;
  }
  assert(file_size > 0);
  input.resize(file_size);
  if (fread(input.data(), 1, file_size, file) < 0) {
    err = "fread";
    goto error;
  }
  ret = run(input);
  goto cleanup;
error:
  perror(err);
  ret = 1;
cleanup:
  if (file) {
    fclose(file);
  }
  return ret;
}
