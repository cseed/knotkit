
#include <knotkit.h>

// test for ring

template<class R> void
test_ring (int p)
{
  R zero (0);
  R one (1);
  R minus_one (-1);
  
  assert (zero == 0);
  assert (zero | zero);
  assert (one | zero);
  assert (minus_one | zero);
  assert (! (zero | one));
  assert (! (zero | minus_one));
  
  assert (one.is_unit ());
  assert (minus_one.is_unit ());
  assert (one.recip () == one);
  assert (minus_one.recip () == minus_one);
  
  if (p)
    assert (R (p) == 0);
  
  if (p != 2)
    assert (one != minus_one);
  
  int n = (p
	   ? std::min (p, 20)
	   : 20);
  for (int i = -n; i <= n; i ++)
    {
      R x (i);
      if (x.is_unit ())
	{
	  assert (x * x.recip () == one);
	  assert (x.recip () * x == one);
	  assert (x.recip ().recip () == x);
	}
      
      assert (one | x);
      assert (minus_one | x);
      
      if (x != 0)
	{
	  assert (x | zero);
	  assert (! (zero | x));
	}
      
      for (int j = -n; j <= n; j ++)
	{
	  R y (j);
	  
	  assert (- (-x) == x);
	  assert (x + y == y + x);
	  assert (x * y == y * x);
	  
	  if (x != 0 && x | y)
	    {
	      R q = y.divide_exact (x);
	      assert (y == q * x);
	    }
	  
	  if (x != 0 || y != 0)
	    {
	      tuple<R, R, R> t = x.extended_gcd (y);
	      assert (get<0> (t) == get<1> (t)*x + get<2> (t)*y);
	    }
	  
	  for (int k = -n; k <= n; k ++)
	    {
	      R z (k);
	      
	      assert ((x + y) + z == x + (y + z));
	      assert ((x * y) * z == x * (y * z));
	      assert (x*(y + z) == x*y + x*z);
	      assert ((x + y)*z == x*z + y*z);
	    }
	}
    }
}

template<class F> void
test_field ()
{
  for (unsigned i = 1; i <= 8; i ++)
    for (unsigned j = 1; j <= rolfsen_crossing_knots (i); j ++)
      {
	knot_diagram kd (rolfsen_knot (i, j));
	
	show (kd); newline ();
	
	cube<F> c (kd);
	mod_map<F> d = c.compute_d (1, 0, 0, 0, 0);
	
	assert (d.compose (d) == 0);
	
	ptr<const quotient_module<F> > H = d.homology ();
	display ("H:\n", *H);
	
	chain_complex_simplifier<F> s (c.khC, d, 1);
	display ("s.new_C:\n", *s.new_C);
	
	assert (H->dim () == s.new_C->dim ());
      }
}

bool
rank_lte (multivariate_laurentpoly<Z> p,
	  multivariate_laurentpoly<Z> q)
{
  for (map<multivariate_laurent_monomial, Z>::const_iter i = p.coeffs; i; i ++)
    {
      Z a = i.val ();
      Z b = q.coeffs(i.key (), Z (0));
      assert (a != 0 && b != 0);
      
      if (a > b)
	return 0;
    }
  return 1;
}

int
main ()
{
  for (int a = 1; a >= 0; a --)
    for (unsigned i = 1; i <= 9; i ++)
      for (unsigned j = 1; j <= htw_knots (i, a); j ++)
	{
	  knot_diagram kd (htw_knot (i, a, j));
	  show (kd); newline ();
	  
	  cube<Z2> c (kd);
	  mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
  
	  chain_complex_simplifier<Z2> s (c.khC, d, 1);
  
	  steenrod_square sq (c, d, s);
	  mod_map<Z2> sq1 = sq.sq1 ();
	  // display ("sq1:\n", sq1);

	  mod_map<Z2> sq2 = sq.sq2 ();
	  if (a)
	    assert (sq2 == 0);
	  else
	    display ("sq2:\n", sq2);
  
	  assert (sq1.compose (sq1) == 0);
	  assert (sq2.compose (sq2) + sq1.compose (sq2).compose (sq1) == 0);
	}
  
#if 0
  typedef Z2 R;
  
  for (unsigned i = 12; i <= 12; i ++)
    for (unsigned j = 1; j <= htw_knots (i, 0); j ++)
      {
	knot_diagram kd (htw_knot (i, 0, j));
	show (kd); newline ();
	
	cube<R> c (kd);
	mod_map<R> d = c.compute_d (1, 0, 0, 0, 0);
	
	chain_complex_simplifier<R> s (c.khC, d, 1);
	// assert (s.new_d == 0);
	
	printf ("|s.new_C| = %d\n", s.new_C->dim ());
      }
#endif
  
#if 1
  map<multivariate_laurentpoly<Z>,
      set<triple<unsigned, int, unsigned> > > kh_knot_map;
  
  for (int a = 1; a >= 0; a --)
    for (unsigned i = 1; i <= 12; i ++)
      for (unsigned j = 1; j <= htw_knots (i, a); j ++)
	{
	  knot_diagram kd (htw_knot (i, a, j));
	  kd.marked_edge = 1;
	  show (kd); newline ();
	  
	  cube<Z2> c (kd, 1);
	  mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
	  sseq_builder b (c.khC, d);
	  sseq ss = b.build_sseq ();
	  
	  multivariate_laurentpoly<Z> P = ss.pages[1].poincare_polynomial (ss.bounds);
	  kh_knot_map[P].push (triple<unsigned, int, unsigned> (i, a, j));
	}
  
  {
    writer w ("kh_knot_map.dat");
    write (w, kh_knot_map);
  }
#endif
  
#if 0
  reader r ("kh_knot_map.dat");
  map<multivariate_laurentpoly<Z>,
      set<triple<unsigned, int, unsigned> > > kh_knot_map (r);
  
  for (map<multivariate_laurentpoly<Z>,
	   set<triple<unsigned, int, unsigned> > >::const_iter i = kh_knot_map; i; i ++)
    {
      printf ("P = "); display (i.key ());
      for (set_const_iter<triple<unsigned, int, unsigned> > j = i.val (); j; j ++)
	{
	  knot_diagram kd (htw_knot (j.val ().first,
				     j.val ().second,
				     j.val ().third));
	  printf ("  ");
	  show (kd);
	  newline ();
	}
    }
#endif
  
#if 0
  typedef Z2 F;
  typedef fraction_field<polynomial<F> > R;

  for (unsigned i = 1; i <= 10; i ++)
    for (unsigned j = 1; j <= htw_knots (i, 0); j ++)
      {
	knot_diagram kd (htw_knot (i, 0, j));
	kd.marked_edge = 1;
	
	show (kd); newline ();
	
	spanning_tree_complex<F> st (kd);
  
	mod_map<R> d2 = st.twisted_d2 ();
	assert (d2.compose (d2) == 0);
	
	mod_map<R> d2U1 = st.twisted_d2Un (1);
	// mod_map<R> d2U1 = st.twisted_d2U1_test ();
  
	assert (d2.compose (d2U1) + d2U1.compose (d2) == 0);
	
	mod_map<R> d2U2 = st.twisted_d2Un (2);
	assert (d2.compose (d2U2) + d2U2.compose (d2) + d2U1.compose (d2U1) == 0);
  
	mod_map<R> d2U3 = st.twisted_d2Un (3);
	assert (d2.compose (d2U3) + d2U3.compose (d2)
		+ d2U2.compose (d2U1) + d2U1.compose (d2U2) == 0);

	mod_map<R> d2U4 = st.twisted_d2Un (4);
	assert (d2.compose (d2U4) + d2U4.compose (d2)
		+ d2U3.compose (d2U1) + d2U1.compose (d2U3)
		+ d2U2.compose (d2U2) == 0);

	mod_map<R> d2U5 = st.twisted_d2Un (5);
	assert (d2.compose (d2U5) + d2U5.compose (d2)
		+ d2U4.compose (d2U1) + d2U1.compose (d2U4)
		+ d2U3.compose (d2U2) + d2U2.compose (d2U3) == 0);
	
	mod_map<R> d2U6 = st.twisted_d2Un (6);
	assert (d2.compose (d2U6) + d2U6.compose (d2)
		+ d2U5.compose (d2U1) + d2U1.compose (d2U5)
		+ d2U4.compose (d2U2) + d2U2.compose (d2U4)
		+ d2U3.compose (d2U3) == 0);
      }
#endif
}
