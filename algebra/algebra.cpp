
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

static tuple<unsigned, int, int>
extended_gcd_1 (int a, int b)
{
  if (b == 0)
    return tuple<unsigned, int, int> (a, 1, 0);
  
  unsigned t = a % b;
  if (!t)
    return make_tuple (b, 0, 1);
  else
    {
      tuple<unsigned, int, int> s = unsigned_extended_gcd (b, t);
      
      unsigned d = get<0> (s);
      int x = get<2> (s),
	y = get<1> (s) - get<2> (s) * (a / b);
      
      assert ((int)d == a*x + b*y);
      
      return make_tuple (d, x, y);
    }
}

tuple<unsigned, int, int>
unsigned_extended_gcd (unsigned a, unsigned b)
{
  return extended_gcd_1 ((int)a, (int)b);
}

tuple<unsigned, int, int>
extended_gcd (int a, int b)
{
  tuple<unsigned, int, int> t = extended_gcd_1 (std::abs (a),
						std::abs (b));
  unsigned d = get<0> (t);
  int x = get<1> (t),
    y = get<2> (t);
  if (a < 0)
    x *= -1;
  if (b < 0)
    y *= -1;
  
  assert ((int)d == a*x + b*y);
  
  return make_tuple (d, x, y);
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
