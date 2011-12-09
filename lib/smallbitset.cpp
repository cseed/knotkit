
#include <lib/lib.h>

smallbitset::smallbitset (reader &r)
{
  uint64 t;
  read (r, t);
  n = t >> 58;
  x = t;
  
  assert (n <= max_size);
}

#if 0
smallbitset::smallbitset (const unsignedset &t)
  : n(t.size ()),
    x(0)
{
  assert (n <= max_size);
  for (unsignedset_const_iter i = t; i; i ++)
    push (i.val ());
}
#endif

void
smallbitset::write_self (writer &w) const
{
  write (w, x | (((uint64)n) << 58));
}

void
smallbitset::show_self () const
{
  printf ("%d:", n);
  for (unsigned i = n; i >= 1; i --)
    printf ("%c", operator % (i) ? '1' : '0');
}

void
smallbitset::display_self () const
{
  show_self ();
  newline ();
}
