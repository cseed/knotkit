
#include <algebra/algebra.h>

unsigned
unsigned_gcd (unsigned a, unsigned b)
{
  while (b != 0)
    {
      unsigned t = a % b;
      a = b;
      b = t;
    }
  return a;
}

unsigned
int_gcd (int a, int b)
{
  return unsigned_gcd ((unsigned)std::abs (a),
		       (unsigned)std::abs (b));
}

uint64
uint64_gcd (uint64 a, uint64 b)
{
  while (b != 0)
    {
      uint64 t = a % b;
      a = b;
      b = t;
    }
  return a;
}

uint64
int64_gcd (int64 a, int64 b)
{
  return uint64_gcd ((uint64)std::abs (a),
		     (uint64)std::abs (b));
}

static triple<unsigned, int, int>
extended_gcd_1 (unsigned a, unsigned b)
{
  if (b == 0)
    return triple<unsigned, int, int> (a, 1, 0);
  
  unsigned t = a % b;
  if (!t)
    return triple<unsigned, int, int> (b, 0, 1);
  else
    {
      triple<unsigned, int, int> s = extended_gcd (b, t);
      
      unsigned d = s.first;
      int x = s.third,
	y = s.second - s.third * (a / b);
      
      assert ((int)d == (int)a*x + (int)b*y);
      
      return triple<unsigned, int, int> (d, x, y);
    }
}

triple<unsigned, int, int>
extended_gcd (int a, int b)
{
  triple<unsigned, int, int> t = extended_gcd_1 (std::abs (a),
						 std::abs (b));
  unsigned d = t.first;
  int x = t.second,
    y = t.third;
  if (a < 0)
    x *= -1;
  if (b < 0)
    y *= -1;
  
  assert ((int)d == a*x + b*y);
  
  return triple<unsigned, int, int> (d, x, y);
}

unsigned
unsigned_lcm (unsigned a, unsigned b)
{
  return (a / unsigned_gcd (a, b)) * b;
}

uint64
uint64_lcm (uint64 a, uint64 b)
{
  return (a / uint64_gcd (a, b)) * b;
}

unsigned int_lcm (int a, int b)
{
  return unsigned_lcm ((unsigned)std::abs (a),
		       (unsigned)std::abs (b));
}

uint64 int64_lcm (int64 a, int64 b)
{
  return uint64_lcm ((uint64)std::abs (a),
		     (uint64)std::abs (b));
}
