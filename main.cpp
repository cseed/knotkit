
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
	      R q = y.div (x);
	      assert (y == q * x);
	    }
	  
	  if (x != 0 || y != 0)
	    {
	      triple<R, R, R> t = x.extended_gcd (y);
	      assert (t.first == t.second*x + t.third*y);
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

void
check (const dt_code &dt)
{
  if (dt.num_components () > 1)
    {
      knot_diagram kd (dt);
      kd.marked_edge = 1;
      show (kd); newline ();
      
      cube<Z2> c (kd, 1);
      mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
      sseq_builder b (c.khC, d);
      sseq ss = b.build_sseq ();
      
      unsigned n_comps = kd.num_components ();
      assert (n_comps == dt.num_components ());
      
      unsigned split = 1;
      for (unsigned k = 1; k < unsigned_2pow (n_comps) - 1; k ++)
	{
	  knot_diagram kd2 (SUBLINK,
			    smallbitset (n_comps, k),
			    kd);
	  kd2.marked_edge = 1;
	  unsigned n_comps2 = kd2.num_components ();
	  
	  assert (n_comps2 == unsigned_bitcount (k));
	  assert (n_comps2 > 0);
	  assert (n_comps2 < n_comps);
	  
	  cube<Z2> c2 (kd2, 1);
	  mod_map<Z2> d2 = c2.compute_d (1, 0, 0, 0, 0);
	  sseq_builder b2 (c2.khC, d2);
	  sseq ss2 = b2.build_sseq ();
	  
	  printf ("  k = %d, %d <=? %d\n",
		  k,
		  ss2.pages[1].total_rank (),
		  ss.pages[1].total_rank ());
	  if (ss2.pages[1].total_rank () > ss.pages[1].total_rank ())
	    printf ("    !! COUNTEREXAMPLE\n");
	  
	  if (unsigned_bitcount (k) == 1)
	    split *= ss2.pages[1].total_rank ();
	}
      
      printf ("  split %d <=? %d\n",
	      split,
	      ss.pages[1].total_rank ());
      if (split > ss.pages[1].total_rank ())
	printf ("    !! COUNTEREXAMPLE\n");
    }
}

int
main ()
{
  ptr<const explicit_module<Q> > A
    = new explicit_module<Q> (2, basedvector<Q, 1> (), basedvector<grading, 1> (2));
  ptr<const explicit_module<Q> > B
    = new explicit_module<Q> (3, basedvector<Q, 1> (), basedvector<grading, 1> (3));
  ptr<const explicit_module<Q> > C
    = new explicit_module<Q> (3, basedvector<Q, 1> (), basedvector<grading, 1> (3));
  ptr<const explicit_module<Q> > D
    = new explicit_module<Q> (2, basedvector<Q, 1> (), basedvector<grading, 1> (2));
  ptr<const explicit_module<Q> > E
    = new explicit_module<Q> (2, basedvector<Q, 1> (), basedvector<grading, 1> (3));
  ptr<const explicit_module<Q> > F
    = new explicit_module<Q> (2, basedvector<Q, 1> (), basedvector<grading, 1> (2));
  
  map_builder<Q> fb (A, B);
  fb[1].muladd (2, 1);
  fb[1].muladd (3, 2);
  fb[2].muladd (-5, 2);
  fb[2].muladd (4, 3);
  mod_map<Q> f (fb);
  display ("f:\n", f);
  
  map_builder<Q> gb (C, D);
  gb[1].muladd (1, 1);
  gb[2].muladd (3, 1);
  gb[2].muladd (-2, 2);
  gb[3].muladd (-6, 2);
  mod_map<Q> g (gb);
  display ("g:\n", g);
  
  display ("f oplus g:\n", f.add (g));
  
  map_builder<Q> hb (E, F);
  hb[1].muladd (3, 2);
  hb[2].muladd (-3, 1);
  mod_map<Q> h (hb);
  display ("h:\n", h);
  
  mod_map<Q> fg = f.tensor (g);
  display ("fg:\n", fg);
  
  ptr<const module<Q> > AB_C = (A->tensor (B))->tensor (C),
    A_BC = A->tensor (B->tensor (C));
  assert (AB_C == A_BC);
  
  assert ((f.tensor (g)).tensor (h) == f.tensor (g.tensor (h)));
  
  ptr<const hom_module<Q> > homAB = A->hom (B);
  linear_combination<Q> x = homAB->map_as_element (f);
  display ("x:\n", x);

#if 0 
  for (unsigned i = 1; i <= 14; i ++)
    {
      for (unsigned j = 1; j <= mt_links (i, 0); j ++)
	check (mt_link (i, 0, j));

      for (unsigned j = 1; j <= mt_links (i, 1); j ++)
	check (mt_link (i, 1, j));
    }
#endif
  
#if 0
  knot_diagram kd (rolfsen_knot (8, 19));
  cube<Z2> c (kd);
  sseq ss = compute_szabo_sseq (c);
  multivariate_laurentpoly<Z> ssp = ss.pages[1].poincare_polynomial (ss.bounds);
  
  display ("ssp: ", ssp);
  
  multivariate_laurentpoly<Z> p;
  p.muladdeq (5, multivariate_laurent_monomial (VARIABLE, 1, -2));
  p.muladdeq (-6, multivariate_laurent_monomial (VARIABLE, 2, 13));
  p.muladdeq (14, (multivariate_laurent_monomial (VARIABLE, 1, 5)
		   * multivariate_laurent_monomial (VARIABLE, 2, -6)));
  display ("p: ", p);
  
  display ("p*p: ", p*p);

  {
    writer w ("dump");
    write (w, p*p);
  }

  {
    reader r ("dump");
    multivariate_laurentpoly<Z> q (r);
    
    display ("q: ", q);
    
    assert (q == p*p);
  }
#endif
  
#if 0
  multivariate_laurentpoly<Z> p = -11;
  p.muladdeq (5, VARIABLE, 1);
  p.muladdeq (7, VARIABLE, 2);
  p.muladdeq (-3, VARIABLE, 3);

  multivariate_laurentpoly<Z> q = p*p + p + 23;
  multivariate_laurentpoly<Z> r = q*q - Z (7)*p + 81;
  
  multivariate_laurentpoly<Z> s = r - p*q + 10;

  display ("p:", p);
  display ("q:", q);
  display ("r:", r);
  display ("s:", s);
  
  map<multivariate_laurentpoly<Z>, std::string> m;
  m.push (p, "p");
  m.push (q, "q");
  m.push (r, "thisisr");
  
  assert (m % p);
  assert (m % q);
  assert (m % r);
  assert (! (m % s));
  
  assert (m(p) == "p");
  assert (m(q) == "q");
  assert (m(r) == "thisisr");
  
  std::string str ("This is a test.");
  
  {
    writer w ("test.dat");
    write (w, m);
  }
  
  reader rdr ("test.dat");
  map<multivariate_laurentpoly<Z>, std::string> m2 (rdr);
    
  assert (m == m2);
    
  assert (m2(p) == "p");
  assert (m2(q) == "q");
  assert (m2(r) == "thisisr");
#endif
  
#if 0
  test_ring<Z2> (2);
  test_ring<Z> (0);
  test_ring<Q> (0);
  test_ring<Zp<2> > (2);
  test_ring<Zp<3> > (3);
  test_ring<Zp<5> > (5);
  test_ring<Zp<7> > (7);
  
  test_field<Q> ();
  test_field<Zp<7> > ();
  test_field<Zp<5> > ();
  test_field<Zp<3> > ();
  test_field<Z2> ();
  test_field<Zp<2> > ();
#endif  
}
