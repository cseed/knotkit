
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
  basedvector<int, 1> v (4);
  v[1] = 2;
  v[2] = -1;
  v[3] = 0xbf32813;
  v[4] = -352182;

  vector<int> sleb (512);
  for (unsigned i = 0; i < 512; i ++)
    sleb[i] = ((int)i << 23);

  printf ("sleb:\n");
  for (unsigned i = 0; i < 512; i ++)
    printf ("  %d\n", sleb[i]);
  
  vector<unsigned> uleb (512);
  for (unsigned i = 0; i < 512; i ++)
    uleb[i] = ((unsigned)i << 23);
  
  printf ("uleb:\n");
  for (unsigned i = 0; i < 512; i ++)
    printf ("  %u\n", uleb[i]);
  
  polynomial<Z> one (1);
  polynomial<Z> x (1, 1);
  polynomial<Z> bigp (Z (367521));
  
  polynomial<Z> t = x + bigp*one;
  polynomial<Z> p = (t*t*t + t*t + t + one)*(bigp*t*t + bigp*bigp*t + bigp*bigp*bigp)*(bigp*bigp*t*t*t + bigp*t*t + t + one);
  display ("p: ", p);
  
  {
    file_writer w ("test.dat");
    write (w, v);
    write (w, p);
    write (w, sleb);
    write (w, uleb);
  }
  
  {
    gzfile_writer w ("test.2.dat.gz");
    write (w, v);
    write (w, p);
    write (w, sleb);
    write (w, uleb);
  }
  
  system ("gzip -cd test.2.dat.gz > test.2.dat");
  
  {
    file_reader r ("test.dat");
    basedvector<int, 1> v2 (r);
    polynomial<Z> p2 (r);
    
    vector<int> sleb2 (r);
    printf ("sleb2:\n");
    for (unsigned i = 0; i < 512; i ++)
      printf ("  %d\n", sleb2[i]);
    
    vector<unsigned> uleb2 (r);
    assert (v == v2);
    assert (p == p2);
    assert (sleb == sleb2);
    assert (uleb == uleb2);
  }
  
  system ("gzip -c9 test.dat > test.dat.gz");
  
  {
    gzfile_reader r ("test.dat.gz");
    basedvector<int, 1> v2 (r);
    polynomial<Z> p2 (r);
    vector<int> sleb2 (r);
    vector<unsigned> uleb2 (r);
    assert (v == v2);
    assert (p == p2);
    assert (sleb == sleb2);
    assert (uleb == uleb2);
  }
  
#if 0
  
#if 0
  test_ring<Z> (0);
  test_ring<polynomial<Z> > (0);
  
  test_field<Z2> ();
  test_field<Zp<7> > ();
  test_field<Q> ();
  // test_field<fraction_field<Zp<7> > > ();
#endif
  
#if 0
  for (unsigned i = 1; i <= 10; i ++)
    for (unsigned j = 1; j <= rolfsen_crossing_knots (i); j ++)
      {
	knot_diagram kd (rolfsen_knot (i, j));
#endif
  for (unsigned i = 1; i <= 12; i ++)
    for (unsigned j = 1; j <= htw_knots (i, 0); j ++)
      {
	knot_diagram kd (htw_knot (i, 0, j));
	
	kd.marked_edge = 1;
	
	show (kd); newline ();
	
	spanning_tree_complex<Z2> c (kd);
  
	mod_map<fraction_field<polynomial<Z2> > > E2_d = c.twisted_d2 ();
	assert (E2_d.compose (E2_d) == 0);
	// display ("E2_d:\n", E2_d);
	
	chain_complex_simplifier<fraction_field<polynomial<Z2> > > E2_s (c.C, E2_d, 2);
	assert (E2_s.new_d == 0);
  
	multivariate_laurentpoly<Z> E3_p = E2_s.new_C->free_delta_poincare_polynomial ();
	printf ("E3_p = "); show (E3_p); newline ();
  
	mod_map<fraction_field<polynomial<Z2> > > tt_d = c.totally_twisted_kh_d ();
	assert (tt_d.compose (tt_d) == 0);
	// display ("tt_d:\n", tt_d);
	
	chain_complex_simplifier<fraction_field<polynomial<Z2> > > tt_s (c.C, tt_d, 2);
	assert (tt_s.new_d == 0);

	multivariate_laurentpoly<Z> tt_p = tt_s.new_C->free_delta_poincare_polynomial ();
	printf ("tt_p = "); show (tt_p); newline ();
	
	cube<Z2> kh_c (kd, 1);
	mod_map<Z2> kh_d = kh_c.compute_d (1, 0, 0, 0, 0);
	
	sseq_builder builder (kh_c.khC, kh_d);
	sseq ss = builder.build_sseq ();
	multivariate_laurentpoly<Z> kh_p = ss.pages[1].delta_poincare_polynomial (ss.bounds);
	printf ("kh_p = "); show (kh_p); newline ();
	
	if (tt_p != kh_p)
	  printf (" > tt_p != kh_p!!\n");
	
	if (! rank_lte (E3_p, tt_p))
	  printf (" > rank E2 > rank tt!!\n");
      }
#endif
}
