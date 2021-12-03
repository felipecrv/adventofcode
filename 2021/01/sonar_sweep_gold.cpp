#include <cstdio>
#include <vector>

int main() {
  std::vector<int> samples;
  int sample;
  while (scanf("%d", &sample) == 1) {
    samples.push_back(sample);
  }

  std::vector<int> slides;
  for (int i = 2; i < samples.size(); i++) {
    slides.push_back(samples[i - 2] + samples[i - 1] + samples[i]);
  }

  int increases = 0;
  for (int i = 1; i < slides.size(); i++) {
    if (slides[i] > slides[i - 1]) increases++;
  }
  printf("%d\n", increases);

  return 0;
}

