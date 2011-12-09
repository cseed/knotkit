
#include <algebra/algebra.h>

void
Q::reduce ()
{
  unsigned c = unsigned_gcd ((unsigned)std::abs (n), d);
  n /= (int)c;
  d /= c;
}

void
Q::show_self () const
{
  if (d == 1)
    printf ("%d", n);
  else
    printf ("%d/%u", n, d);
}
