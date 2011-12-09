
#include <gmp.h>

#include <lib/lib.h>

inline int recip (int x)
{
  assert (x == 1 || x == -1);
  return x;
}

template<class T> inline T recip (const T &x) { return x.recip (); }

template<class T> inline T
pow (const T &x, int d)
{
  if (d == 0)
    return 1;
  else if (d == 1)
    return x;
  else if (d < 0)
    return pow (recip (x), -d);
  else
    {
      T x2 = x * x;
      if (d & 1)
	return x * pow (x2, (d - 1) / 2);
      else
	return pow (x2, d / 2);
    }
}

unsigned unsigned_gcd (unsigned a, unsigned b);
uint64 uint64_gcd (uint64 a, uint64 b);

unsigned int_gcd (int a, int b);
uint64 int64_gcd (int64 a, int64 b);

unsigned unsigned_lcm (unsigned a, unsigned b);
uint64 uint64_lcm (uint64 a, uint64 b);
unsigned int_lcm (int a, int b);
uint64 int64_lcm (int64 a, int64 b);

// (d, x, y) = gcd (a, b) where x*a + y*b = d
triple<unsigned, int, int> extended_gcd (int a, int b);

template<class R> class linear_combination;
template<class R> class linear_combination_const_iter;

template<class R> class module;

#include <algebra/multivariate_polynomial.h>

#include <algebra/laurentpoly.h>

#include <algebra/grading.h>
#include <algebra/module.h>

#include <algebra/Z2.h>
#include <algebra/linear_combination.h>

#include <algebra/Z.h>
#include <algebra/Q.h>
#include <algebra/polynomial.h>

#include <algebra/fraction_field.h>
