
#include <knotkit.h>

polynomial<Z2>
polynomial<Z2>::operator * (polynomial<Z2> p) const
{
  set<unsigned> r_coeffs;
  
  for (set_const_iter<unsigned> i = coeffs; i; i ++)
    for (set_const_iter<unsigned> j = p.coeffs; j; j ++)
      r_coeffs.toggle (i.val () + j.val ());
  
  return polynomial<Z2> (r_coeffs);
}

pair<polynomial<Z2>, polynomial<Z2> > 
polynomial<Z2>::divide_with_remainder (polynomial d) const
{
  assert (d != 0);
  
  polynomial<Z2> r (COPY, *this);
  polynomial<Z2> q;
  
  unsigned d_leading_exponent = d.coeffs.tail ();
  for (;;)
    {
      if (r == 0)
	break;
      
      unsigned r_leading_exponent = r.coeffs.tail ();
      if (r_leading_exponent < d_leading_exponent)
	break;
      
      for (set_const_iter<unsigned> i = d.coeffs; i; i ++)
	r.coeffs.toggle (r_leading_exponent - d_leading_exponent + i.val ());
      q.coeffs.toggle (r_leading_exponent - d_leading_exponent);
    }
  
  assert (r == 0 || r.degree () < d.degree ());
  // assert (*this == q*d + r);
  
  return pair<polynomial<Z2>, polynomial<Z2> > (q, r);
}

polynomial<Z2>
polynomial<Z2>::mod (polynomial d) const
{
  assert (d != 0);
  
  polynomial<Z2> r (COPY, *this);
  
  unsigned d_leading_exponent = d.coeffs.tail ();
  for (;;)
    {
      if (r == 0)
	break;
      
      unsigned r_leading_exponent = r.coeffs.tail ();
      if (r_leading_exponent < d_leading_exponent)
	break;
      
      for (set_const_iter<unsigned> i = d.coeffs; i; i ++)
	r.coeffs.toggle (r_leading_exponent - d_leading_exponent + i.val ());
    }
  
  assert (r == 0 || r.degree () < d.degree ());
  
  return r;
}

bool
polynomial<Z2>::divides (polynomial d) const
{
  return mod (d) == 0;
}
  
polynomial<Z2>
polynomial<Z2>::divide_exact (polynomial d) const
{
  pair<polynomial<Z2>, polynomial<Z2> > qr = divide_with_remainder (d);
  assert (qr.second == 0);
  return qr.first;
}
  
polynomial<Z2>
polynomial<Z2>::gcd (polynomial b) const
{
  // ??? can return *this
  polynomial a = *this;
  
  while (b != 0)
    {
      pair<polynomial<Z2>, polynomial<Z2> > a_qr = a.divide_with_remainder (b);
      a = b;
      b = a_qr.second;
    }
  return a;
}

void
polynomial<Z2>::show_self () const
{
  unsigned first = 1;
  for (set_const_iter<unsigned> i = coeffs; i; i ++)
    {
      unsigned e = i.val ();
      
      if (first)
	first = 0;
      else
	printf (" + ");
      
      if (e == 0)
	printf ("1");
      else
	{
	  if (e == 1)
	    printf ("x");
	  else
	    printf ("x^%d", e);
	}
    }
  if (first)
    printf ("0");
}
