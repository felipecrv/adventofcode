#include <cstdio>

#define W 25
#define H 6
#define LEN (W * H)

struct Image {
  Image() {
    for (int i = 0; i < LEN; i++) {
      pixbuf[i] = 2;
    }
  }

  int pixbuf[LEN];
};

int main() {
  Image image;
  for (int i = 0;; i = (i + 1) % LEN) {
    char c = getchar();
    if (c == EOF || c == '\n') {
      break;
    }
    auto &pixel = image.pixbuf[i];
    if (pixel == 2) {
      pixel = c - '0';
    }
  }

  for (int i = 0; i < H; i++) {
    for (int j = 0; j < W; j++) {
      int p = image.pixbuf[i * W + j];
      if (p == 0) {
        putchar(' ');
      } else {
        putchar('#');
      }
    }
    putchar('\n');
  }

  return 0;
}
