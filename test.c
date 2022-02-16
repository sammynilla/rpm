
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "rpm.h"
// NOTE (sammynilla): PPM image format specification taken from 'man ppm'
// http://ailab.eecs.wsu.edu/wise/P1/PPM.html

#define WIDTH 512L
#define HEIGHT 512L

uint32_t
pack_colors(const uint8_t r, const uint8_t g, const uint8_t b) {
  const uint8_t a = 255;
  return (a<<24) + (b<<16) + (g<<8) + r;
}

int
main(void) {
  FILE *f;
  long x, y;
  static char rpm[RPM_SIZE(WIDTH, HEIGHT)];

  rpm_init(rpm, WIDTH, HEIGHT);

  // Overflow and bad size tests
  // TODO (sammynilla): Discover what the max size of PPM can be.
  assert(rpm_size(0, 1) == 0);

  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      uint8_t r = 255 * y / (float)HEIGHT;
      uint8_t g = 255 * x / (float)WIDTH;
      uint8_t b = 0;
      rpm_set(rpm, x, y, pack_colors(r, g, b));
    }
  }

  f = fopen("./out.ppm", "wb");
  fwrite(rpm, sizeof(rpm), 1, f);
  fclose(f);
}
