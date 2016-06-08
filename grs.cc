#include <set>
#include <cstdint>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cinttypes>

#define ODD_PERIOD
#define USE_FIB
#define TEST_BITS 128


#define RATIO128 (((uint128_t)0x9e3779b97f4a7c15ull << 64) + 0xf39cc0605cedc833ull)
#define RATIO128_1 (((uint128_t)0x9e3779b97f4a7c15ull << 64) + 0xf39cc0605cedc833ull)

#define RATIO64 11400714819323198485ull
#define RATIO64_1 11400714819323198486ull

#define RATIO32 2654435769ul
#define RATIO32_1 2654435768ul

#define RATIO16 40499
#define RATIO16_1 40499

__extension__ typedef unsigned __int128 uint128_t;

#define UINTTEST_MAX (~(uinttest_t)0)

#define _TOKENPASTE3(x,y,z) x##y##z
#define TOKENPASTE3(x,y,z) _TOKENPASTE3(x,y,z)
#define _STRINGISE(x) #x
#define STRINGISE(x) _STRINGISE(x)

#if defined(ODD_PERIOD)
#define PERIOD_STR "2**" STRINGISE(TEST_BITS) "-1"
#define TEST_STEP TOKENPASTE3(RATIO, TEST_BITS, _1)
#else
#define PERIOD_STR "2**" STRINGISE(TEST_BITS)
#define TEST_STEP TOKENPASTE3(RATIO, TEST_BITS,)
#endif

#define TEST_TYPE TOKENPASTE3(uint, TEST_BITS, _t)

typedef TEST_TYPE uinttest_t;


static inline uinttest_t add(uinttest_t x, uinttest_t y) {
  uinttest_t z = x + y;
#if defined(ODD_PERIOD)
  if ((uinttest_t)(z + 1) <= x) {
    z = z + 1;
  }
#endif
  return z;
}


static inline uinttest_t mla(uinttest_t x, uinttest_t n, uinttest_t y) {
#if defined(ODD_PERIOD)
  while (n > 0) {
    if (n & 1) x = add(x, y);
    y = add(y, y);
    n >>= 1;
  }
#else
  x += n * y;
#endif
  return x;
}


static inline uinttest_t fibsearch(uinttest_t x0, uinttest_t step,
    uinttest_t window, uinttest_t stop, uinttest_t *n) {
  uinttest_t x = x0;
  uinttest_t prev = 1, curr = 1;

  while (curr <= stop) {
    x = mla(x0, curr, step);
    if (x <= window) {
      *n += curr;
      return x;
    }
    uinttest_t next = curr + prev;
    prev = curr;
    curr = next;
  }
  printf("Search failed.\n");
  x = x0;
  curr = 0;
  do {
    x = add(x, step);
    curr++;
  } while (x > window);
  *n += curr;
  return x;
}


char const *dirty_dec(uinttest_t x, int w) {
  static char tmp[8][128];
  static int o = 0;
  char *p = tmp[o & 7] + sizeof(tmp[0]);
  ++o;
  *--p = '\0';
  do {
    *--p = '0' + (x % 10);
    x /= 10;
    --w;
  } while (x > 0);
  while (w-- > 0) *--p = ' ';
  return p;
}


char const *dirty_hex(uinttest_t x, int w) {
  static char tmp[8][128];
  static int o = 0;
  char *p = tmp[o & 7] + sizeof(tmp[0]);
  int tic = 9;
  ++o;
  *--p = '\0';
  do {
    if (--tic <= 0) {
      *--p = '_';
      --w;
      tic = 8;
    }
    *--p = "0123456789abcdef"[x & 15];
    x >>= 4;
    --w;
  } while (x > 0);
  while (w-- > 0) *--p = ' ';
  return p;
}



int main(int argc, char *argv[]) {
  std::set<uinttest_t> visited;
  uinttest_t x = 0;
  uinttest_t window = UINTTEST_MAX;
  uinttest_t step = TEST_STEP;
  uinttest_t worst = UINTTEST_MAX;
  uinttest_t n = 1;
  uinttest_t old_n = 1;
  uinttest_t best_n = 0;
  long hits = 0;

  visited.insert(x);

  if (argc >= 2) step = strtoull(argv[1], NULL, 0);

  while (worst > 1) {
#if defined(USE_FIB)
    x = fibsearch(x, step, window, n >> (n < 10000 ? 0 : 8), &n);
#else
    do {
      x = add(x, step);
      n++;
    } while (x > window);
#endif
    hits++;

    auto r = visited.insert(x);
    if (r.second == false) {
      printf("OOPS!\n");
      break;
    }
    uinttest_t d, oldworst = worst;
    auto a = r.first, b = r.first;
    a++, b--;
    d = x - ((b != visited.end()) ? *b : 0);
    if (worst > d) worst = d;
    d = ((a != visited.end()) ? *a : UINTTEST_MAX) - x;
    if (worst > d) worst = d;

    if (worst != oldworst) {
      uinttest_t ideal = UINTTEST_MAX / n;
      printf("%8ld, %s, %4.2f, %.10f\n",
          hits, dirty_dec(n, 32), (float)old_n / n, (float)worst / ideal);
      if (!best_n && worst < (ideal >> 2)) {
        best_n = n - 1;
        printf("p=" PERIOD_STR ", k=%s (0x%s), n=%s (0x%s)\n",
            dirty_dec(step, 1), dirty_hex(step, 1),
            dirty_dec(best_n, 1), dirty_hex(best_n, 1));
#if 1
        break;
#endif
      }
      while ((window >> 16) > worst) window >>= 1;
      hits = 0;
      old_n = n;
    }
  }

  return 0;
}
