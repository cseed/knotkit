
#include <lib/lib.h>

unsigned unsigned_set (const ullmanset<1> &s)
{
  assert (s.size () <= unsigned_bits);
  
  unsigned v = 0;
  for (ullmanset_const_iter<1> i = s; i; i ++)
    v = unsigned_bitset (v, i.val ());
  return v;
}

unsigned
unsigned_set (const bitset &s)
{
  assert (s.size () <= unsigned_bits);
  
  unsigned v = 0;
  for (bitset_const_iter i = s; i; i ++)
    v = unsigned_bitset (v, i.val ());
  return v;
}

unsigned
unsigned_set (const set<unsigned> &s)
{
  unsigned v = 0;
  for (set_const_iter<unsigned> i = s; i; i ++)
    v = unsigned_bitset (v, i.val ());
  return v;
}

/* bitset */

bitset::bitset (const ullmanset<1> &t)
  : n(0)
{
  *this = bitset (t.size ());
  for (ullmanset_const_iter<1> i = t; i; i ++)
    push (i.val ());
}

bitset &
bitset::operator = (const ullmanset<1> &t)
{
  *this = bitset (t.size ());
  for (ullmanset_const_iter<1> i = t; i; i ++)
    push (i.val ());
  return *this;
}

smallbitset::smallbitset (const ullmanset<1> &t)
  : n(t.size ()), x(0)
{
  assert (n <= 58);
  
  for (ullmanset_const_iter<1> i = t; i; i ++)
    push (i.val ());
}
