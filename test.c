
#define _NO_CRT_STDIO_INLINE
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "uc_rpm.h"

uint32_t
pack_colors(const uint8_t r, const uint8_t g, const uint8_t b) {
  const uint8_t a = 255;
  return (a<<24) + (b<<16) + (g<<8) + r;
}

int
main(void) {
  enum { WIDTH = 512L, HEIGHT = 512L };

  FILE *f;
  static char rpm[UC_RPM_SIZE(WIDTH, HEIGHT)];

  uc_rpm_init(rpm, WIDTH, HEIGHT);

  /* TODO (sammynilla): Discover what the max size of PPM can be. */
  assert(rpm_size(0, 1) == 0);

  {
    long x, y;
    for (y = 0; y < HEIGHT; y++) {
      for (x = 0; x < WIDTH; x++) {
        uint8_t r = (uint8_t)(255 * y / HEIGHT);
        uint8_t g = (uint8_t)(255 * x / WIDTH);
        uint8_t b = (uint8_t)(120);
        uc_rpm_set(rpm, x, y, pack_colors(r, g, b));
      }
    }
  }

  f = fopen("./out.ppm", "wb");
  fwrite(rpm, sizeof(rpm), 1, f);
  fclose(f);

  return 0;
}
