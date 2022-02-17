
#ifndef RPM_H
#define RPM_H

// NOTE (sammynilla): PPM image format specification taken from 'man ppm'
// http://ailab.eecs.wsu.edu/wise/P1/PPM.html

#define RPM_SIZE(w, h) ((h) * ((w) * 3) + (3) + (10 + 1) + (10 + 1) + (4))

static unsigned long
rpm_size(const long width, const long height) {
  enum { MAGIC_NUMBER = 3, SIZE_DATA = 11, MAX_VAL = 4 };
  const int header = (MAGIC_NUMBER + (SIZE_DATA * 2) + MAX_VAL);
  // TODO (sammynilla): Add in value overflow sanitizing checks.
  int query = (width < 1) || (height < 1); // avoid spectre mitigation warning.
  if (query)
    return 0; // Illegal size

  return height * (width * 3) + header;
}

/* static int*/
/* digits_by_log10(unsigned long v) {*/
/*   int r;*/
/*   /* NOTE (sammynilla): Below are some notes from my reading:*/
/*    * Conditions using integer division are slower than simple comparisons.*/
/*    * This might seem a bit silly, but it's an obvious implementation.*/
/*    */
/*   r = (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :*/
/*       (v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 :*/
/*       (v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;*/

/*   return r + 1;*/
/* }*/

static void 
rpm_set(void *buf, long x, long y, unsigned long color) {
  enum { MAGIC_NUMBER = 3, SIZE_DATA = 11, MAX_VAL = 4 };
  const int header = (MAGIC_NUMBER + (SIZE_DATA * 2) + MAX_VAL);
  unsigned char *p = (unsigned char *)buf;

  p = p + header + y * (512 * 3) + x * 3;
  /* NOTE (sammynilla): RGB based format
   * RAWBITS ppm requires values from 0 to MAXVALS (255 in our implementation).
   */
  p[0] = (color >>  0) & 255;
  p[1] = (color >>  8) & 255;
  p[2] = (color >> 16) & 255;
}

/* NOTE (sammynilla): rpm_init()
 * This function is responsible for initialization the ppm file's header.
 * The expected buffer should be zero-initialized before use.
 */
static void
rpm_init(void *buf, long width, long height) {
  enum { 
    ASCII_0 = 0x30, ASCII_2 = 0x32, ASCII_5 = 0x35, ASCII_6 = 0x36,
    ASCII_P = 0x50, ASCII_COMMA = 0x2C, ASCII_NEW_LINE = 0x0A,
  };
  enum { BASE_MOD = 1, MAX_DIGIT = 10, DIGIT_TO_ASCII = 48 };
  unsigned char *p = (unsigned char *)buf;

  // MAGIC NUMBER
  *p++ = ASCII_P;
  *p++ = ASCII_6;
  *p++ = ASCII_NEW_LINE;

  // WIDTH,HEIGHT
  {
    const char sep[2] = { ASCII_COMMA, ASCII_NEW_LINE };
    unsigned char ascii[MAX_DIGIT];
    unsigned char *d;
    int i, j;
    for (i = 0; i < 2; ++i) {
      unsigned long mod = BASE_MOD;
      unsigned long ret = 0;
      unsigned long s = i ? height : width;
      const int digit_count =
        (s >= 1000000000) ? 10 : (s >= 100000000) ? 9 : (s >= 10000000) ? 8 :
        (s >= 1000000) ? 7 : (s >= 100000) ? 6 : (s >= 10000) ? 5 :
        (s >= 1000) ? 4 : (s >= 100) ? 3 : (s >= 10) ? 2 : 1;
      for (j = 0; j < MAX_DIGIT - digit_count; ++j) { ascii[j] = ASCII_0; }
      for (j = 0; j < digit_count; ++j) {
        s -= ret;
        mod *= 10;
        ret = s % mod;
        ascii[MAX_DIGIT-(j+1)] = (char)(ret / (mod * 0.1)) + DIGIT_TO_ASCII;
      }
      d = ascii;
      while (*d) { *p++ = *d++; }
      *p++ = sep[i];
    }
  }

  // MAXVAL
  *p++ = ASCII_2;
  *p++ = ASCII_5;
  *p++ = ASCII_5;
  *p++ = ASCII_NEW_LINE;
}

#endif /* RPM_H */
