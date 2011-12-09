
#include <algebra/algebra.h>

grading
grading::mirror_grading (unsigned nplus, unsigned nminus, bool torsion) const
{
  return grading (torsion ? (1-h) : -h,
		  -q);
}

void
grading::show_self () const
{
  printf ("(%d, %d)", h, q);
}

void
grading::display_self () const
{
  printf ("(%d, %d)\n", h, q);
}

