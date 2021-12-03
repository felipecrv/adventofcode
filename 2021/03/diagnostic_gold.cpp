#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;
using ull = unsigned long long;

ull str2num(const string &s) {
  ull ret = 0;
  for (size_t i = 0; i < s.size(); i++) {
    ret |= (s[s.size() - 1 - i] - '0') << i;
  }
  return ret;
}

const string *findMatch(const vector<string> &all_readings,
                        const string &prefix) {
  for (size_t i = 0; i < all_readings.size(); i++) {
    const string_view s(all_readings[i]);
    if (s.starts_with(prefix)) {
      return &all_readings[i];
    }
  }
  return nullptr;
}

char nextCharToMatch(const vector<string> &all_readings, const string &prefix,
                     bool more_frequent, int *out_num_candidates) {
  *out_num_candidates = 0;
  int pop_count = 0;
  for (size_t i = 0; i < all_readings.size(); i++) {
    string_view s(all_readings[i]);
    if (s.starts_with(prefix)) {
      *out_num_candidates += 1;
      // This sometimes read the '\0' of s.  I rely on input having no
      // duplicates to not go further than that.
      pop_count += s[prefix.size()] == '1';
    }
  }
  bool match_1 = pop_count >= *out_num_candidates - pop_count;
  match_1 = !(match_1 ^ more_frequent);
  return match_1 ? '1' : '0';
}

ull findRating(const vector<string> &all_readings, bool more_frequent) {
  string prefix = "";
  for (;;) {
    int num_candidates = 0;
    const char c =
        nextCharToMatch(all_readings, prefix, more_frequent, &num_candidates);
    if (num_candidates > 1) {
      prefix += c;
      printf("prefix: %s\n", prefix.c_str());
    } else {
      break;
    }
  }

  auto *match = findMatch(all_readings, prefix);
  assert(match && "there will always be a match here");
  printf(" match: %s\n", match->c_str());
  return str2num(*match);
}

int main() {
  vector<string> all_readings;
  char reading_buffer[64 + 1];
  while (scanf("%64s", reading_buffer) == 1) {
    all_readings.emplace_back(reading_buffer);
  }

  const ull o2_gen_rating = findRating(all_readings, true);
  const ull co2_scrub_rating = findRating(all_readings, false);
  printf("%lld * %lld = %lld\n", o2_gen_rating, co2_scrub_rating,
         o2_gen_rating * co2_scrub_rating);

  return 0;
}
