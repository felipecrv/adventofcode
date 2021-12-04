#include <cassert>
#include <cstdio>
#include <string>
#include <unordered_set>
#include <vector>

#include "../lib.h"

using namespace std;

bool parseNumList(vector<int> &out) {
  int v = 0;
  for (;;) {
    char c = getchar();
    switch (c) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        v *= 10;
        v += c - '0';
        break;
      case ',':
        out.push_back(v);
        v = 0;
        break;
      case '\n':
        out.push_back(v);
        return true;
      default:
        break;
    }
  }
  assert(false && "invalid input");
  return false;
}

static const int BOARD_SIZE = 25;

class BoardPtr {
 public:
  BoardPtr(int board_idx, const vector<int> &all_nums,
           vector<int> &all_marks) noexcept {
    assert(all_marks.size() == all_nums.size());
    assert((board_idx + 1) * BOARD_SIZE <= all_nums.size());
    nums = all_nums.data() + (board_idx * BOARD_SIZE);
    marks = all_marks.data() + (board_idx * BOARD_SIZE);
  }

  void mark(int called) {
    for (int i = 0; i < BOARD_SIZE; i++) {
      if (nums[i] == called) {
        marks[i] = true;
      }
    }
  }

  bool winning() const {
    for (int i = 0; i < 5; i++) {
      if (rowMarked(i) || colMarked(i)) return true;
    }
    return false;
  }

  bool winsAfterMark(int n) {
    mark(n);
    return winning();
  }

  int score(int called) const {
    int unmarked_sum = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
      if (!marks[i]) {
        unmarked_sum += nums[i];
      }
    }
    return unmarked_sum * called;
  }

 private:
  bool rowMarked(int r) const {
    return marks[r * 5 + 0] &&  //
           marks[r * 5 + 1] &&  //
           marks[r * 5 + 2] &&  //
           marks[r * 5 + 3] &&  //
           marks[r * 5 + 4];
  }

  bool colMarked(int c) const {
    return marks[0 + c] &&   //
           marks[5 + c] &&   //
           marks[10 + c] &&  //
           marks[15 + c] &&  //
           marks[20 + c];
  }

 private:
  const int *nums;
  int *marks;
};

int main() {
  // \d+ (,\d+)* \n
  vector<int> pool;
  bool parsed = parseNumList(pool);
  assert(parsed);

  // \n
  char c = getchar();
  assert(c == '\n');

  // the numbers from all boards are stored in a flat array
  vector<int> all_nums;
  int num = 0;
  while (scanf("%d", &num) == 1) {
    all_nums.push_back(num);
  }
  assert(all_nums.size() % BOARD_SIZE == 0);

  // allocate a flat array for the marks
  vector<int> all_marks;
  all_marks.resize(all_nums.size());

  // prepare the array of board ptrs
  vector<BoardPtr> boards;
  const int num_boards = all_nums.size() / BOARD_SIZE;
  boards.reserve(num_boards);
  for (int b = 0; b < num_boards; b++) {
    boards.emplace_back(b, all_nums, all_marks);
  }

  // simulate
  unordered_set<int> winning_boards;
  for (const int called : pool) {
    for (int b = 0; b < boards.size(); b++) {
      if (!contains(winning_boards, b) && boards[b].winsAfterMark(called)) {
        const int score = boards[b].score(called);
        printf("Board %d wins with score %d after drawing %d\n", b, score,
               called);
        winning_boards.insert(b);
      }
    }
  }

  return 0;
}
