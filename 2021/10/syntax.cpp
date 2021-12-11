#include <cassert>
#include <cstdio>
#include <vector>

// Stack {{{
// use vector for the .clear() that std::stack doesn't have
std::vector<int> stack;

bool empty() { return stack.empty(); }
void clear() { stack.clear(); }
void push(char c) { stack.push_back(c); }

char pop() {
  if (stack.empty()) return '\0';
  const char c = stack.back();
  stack.pop_back();
  return c;
}
// }}}

// clang-format off
char opener(char c) {
  switch (c) {
    case ')': return '(';
    case ']': return '[';
    case '}': return '{';
    case '>': return '<';
  }
  return '\0';
}

int corr_value(char c) {  // value when c is a corruption char
  switch (c) {
    case ')': return 3;
    case ']': return 57;
    case '}': return 1197;
    case '>': return 25137;
  }
  return 0;
}

int comp_value(char c) { // value when c is closed in a completion
  switch (c) {
    case '(': return 1;
    case '[': return 2;
    case '{': return 3;
    case '<': return 4;
  }
  return 0;
}
// clang-format on

int main() {
  int global_corr_score = 0;
  std::vector<long long> all_comp_scores;

  char c, t;
  long long comp_score;
  for (;;) {
    for (;;) {
      c = getchar();
      switch (c) {
        case '(':
        case '[':
        case '{':
        case '<':
          push(c);
          break;
        case ')':
        case ']':
        case '}':
        case '>':
          t = pop();
          if (t != opener(c)) {
            global_corr_score += corr_value(c);
            for (;;) {
              c = getchar();
              if (c == '\n') break;
            }
            clear();
            goto next_line;
          }
          break;
        case '\n':
          if (!empty()) {
            comp_score = 0;
            do {
              t = pop();
              comp_score = comp_score * 5 + comp_value(t);
            } while (!empty());
            all_comp_scores.push_back(comp_score);
          }
          goto next_line;
        default:
          assert(false && "unexpected char");
          return EXIT_FAILURE;
        case EOF:
          goto end;
      }
    }
    assert(false && "unreachable");
  next_line:
    continue;
  end:
    break;
  }
  assert(!all_comp_scores.empty());

  printf("syntax corruption score: %d\n", global_corr_score);
  sort(all_comp_scores.begin(), all_comp_scores.end());
  printf("syntax completion score: %lld\n",
         all_comp_scores[all_comp_scores.size() / 2]);

  return 0;
}
