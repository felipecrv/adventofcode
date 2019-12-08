#include <cstdio>
#include <vector>

#define W 25
#define H 6
#define LEN (W * H)

struct Image {
  int count_0 = 0;
  int count_1 = 0;
  int count_2 = 0;

  int sum() const { return count_0 + count_1 + count_2; }
};

int main() {
  std::vector<Image> images;
  Image *cur = nullptr;
  for (int i = 0;;) {
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    }

    if (!cur) {
      images.emplace_back();
      cur = &images[images.size() - 1];
      assert(i == 0);
    }

    if (c == '0') {
      cur->count_0 += 1;
    } else if (c == '1') {
      cur->count_1 += 1;
    } else if (c == '2') {
      cur->count_2 += 1;
    } else {
      assert(false);
    }

    if (++i == LEN) {
      cur = nullptr;
      i = 0;
    }
  }

  Image *min_zeroes = nullptr;
  for (int i = 0; i < images.size(); i++) {
    Image &image = images[i];
    assert(image.sum() == LEN);
    if (!min_zeroes) {
      min_zeroes = &image;
    } else {
      if (image.count_0 < min_zeroes->count_0) {
        min_zeroes = &image;
      }
    }
  }
  assert(min_zeroes);
  int min_product = min_zeroes->count_1 * min_zeroes->count_2;
  printf("count_1 * count_2 = %d\n", min_product);

  return 0;
}
