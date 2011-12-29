
#include <lib/lib.h>

#include <stdarg.h>

unsigned
unsigned_pack (unsigned n, unsigned x, unsigned z)
{
  assert (n <= 32);
  assert ((x >> n) == 0);
  
  static const unsigned pack4[256] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 1, 3, 5, 7, 9, 11, 13, 15, 1, 3, 5, 7, 9, 11, 13, 15, 2, 3, 6, 7, 10, 11, 14, 15, 2, 3, 6, 7, 10, 11, 14, 15, 3, 7, 11, 15, 3, 7, 11, 15, 3, 7, 11, 15, 3, 7, 11, 15, 4, 5, 6, 7, 12, 13, 14, 15, 4, 5, 6, 7, 12, 13, 14, 15, 5, 7, 13, 15, 5, 7, 13, 15, 5, 7, 13, 15, 5, 7, 13, 15, 6, 7, 14, 15, 6, 7, 14, 15, 6, 7, 14, 15, 6, 7, 14, 15, 7, 15, 7, 15, 7, 15, 7, 15, 7, 15, 7, 15, 7, 15, 7, 15, 8, 9, 10, 11, 12, 13, 14, 15, 8, 9, 10, 11, 12, 13, 14, 15, 9, 11, 13, 15, 9, 11, 13, 15, 9, 11, 13, 15, 9, 11, 13, 15, 10, 11, 14, 15, 10, 11, 14, 15, 10, 11, 14, 15, 10, 11, 14, 15, 11, 15, 11, 15, 11, 15, 11, 15, 11, 15, 11, 15, 11, 15, 11, 15, 12, 13, 14, 15, 12, 13, 14, 15, 12, 13, 14, 15, 12, 13, 14, 15, 13, 15, 13, 15, 13, 15, 13, 15, 13, 15, 13, 15, 13, 15, 13, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, };
  static const unsigned zerocount4[16] = {4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0, };
  
  unsigned r = 0;
  for (unsigned i = 0; i < n; i += 4)
    {
      unsigned xnib = ((x >> i) & 0xf);
      r |= pack4[(xnib << 4) | (z & 0xf)] << i;
      z >>= zerocount4[xnib];
    }
  assert (z == 0);
  return r;
}

void
stderror (const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  
  vfprintf (stderr, fmt, ap);
  
  fprintf (stderr, ": ");
  perror (0);
  
  va_end (ap);
}

unsigned
fact (unsigned n)
{
  unsigned r = 1;
  for (unsigned i = n; i >= 2; i --)
    r *= i;
  return r;
}

unsigned
choose (unsigned n, unsigned k)
{
  if (n < k)
    return 0;
  
  unsigned r = 1;
  for (unsigned i = n; i >= k + 1; i --)
    r *= i;
  
  unsigned d = fact (n - k);
  assert (r % d == 0);
  return r / d;
}

int
alpha_to_int (char c)
{
  if ('a' <= c && c <= 'z')
    return c - 'a' + 1;
  else
    {
      assert ('A' <= c && c <= 'Z');
      return - (c - 'A' + 1);
    }
}

hash_t 
hash (const std::string &s)
{
  size_t n = s.length ();
  hash_t h = hash (n);
  for (unsigned i = 0; i < n; i ++)
    h = hash_combine (h, hash (s[i]));
  return h;
}
