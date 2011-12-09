
#include <lib/lib.h>

bool
bitset::is_empty () const
{
  unsigned nw = v.size ();
  for (unsigned i = 0; i < nw; i ++)
    {
      if (v[i] != 0)
	return 0;
    }
  return 1;
}

unsigned
bitset::card () const
{
  unsigned c = 0;
  unsigned nw = v.size ();
  for (unsigned i = 0; i < nw; i ++)
    c += word_bitcount (v[i]);
  
  assert (c <= n);
  
  return c;
}

unsigned
bitset::head () const
{
  bitset_const_iter i = *this;
  assert (i);
  return i.val ();
}

unsigned
bitset::pop ()
{
  unsigned i = head ();
  operator -= (i);
  return i;
}

bitset &
bitset::operator |= (const bitset &bs)
{
  assert (n == bs.n);
  assert (v.size () == bs.v.size ());
  int nw = v.size ();
  for (int i = 0; i < nw; i ++)
    v[i] |= bs.v[i];
  return *this;
}

bitset &
bitset::operator &= (const bitset &bs)
{
  assert (n == bs.n);
  assert (v.size () == bs.v.size ());
  int nw = v.size ();
  for (int i = 0; i < nw; i ++)
    v[i] &= bs.v[i];
  return *this;
}

bitset &
bitset::operator ^= (const bitset &bs)
{
  assert (n == bs.n);
  assert (v.size () == bs.v.size ());
  int nw = v.size ();
  for (int i = 0; i < nw; i ++)
    v[i] ^= bs.v[i];
  return *this;
}

bool 
bitset::operator == (const bitset &bs) const
{
  assert (n == bs.n);
  assert (v.size () == bs.v.size ());
  int nw = v.size ();
  for (int i = 0; i < nw; i ++)
    {
      if (v[i] != bs.v[i])
	return 0;
    }
  return 1;
}

void
bitset_const_iter::find_next ()
{
  unsigned wn = v.size ();
  
  while (!w)
    {
      wi ++;
      if (wi < wn)
	w = v[wi];
      else
	break;
    }
  if (wi < wn)
    b = word_ffs (w);
}
