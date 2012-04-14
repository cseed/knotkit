
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

triple<multivariate_laurentpoly<Z>,
       multivariate_laurentpoly<Z>,
       multivariate_laurentpoly<Z> >
square (knot_diagram &kd)
{
  cube<Z2> c (kd);
  mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
  
  chain_complex_simplifier<Z2> s (c.khC, d, 1);
  
  steenrod_square sq (c, d, s);
  mod_map<Z2> sq1 = sq.sq1 ();
  // display ("sq1:\n", sq1);
  
  mod_map<Z2> sq2 = sq.sq2 ();
  // display ("sq2:\n", sq2);
  
  assert (sq1.compose (sq1) == 0);
  assert (sq2.compose (sq2) + sq1.compose (sq2).compose (sq1) == 0);
  
  multivariate_laurentpoly<Z> P = s.new_C->free_poincare_polynomial ();
  
  ptr<const free_submodule<Z2> > sq1_im = sq1.image ();
  multivariate_laurentpoly<Z> sq1_P = sq1_im->free_poincare_polynomial ();
  
  ptr<const free_submodule<Z2> > sq2_im = sq2.image ();
  multivariate_laurentpoly<Z> sq2_P = sq2_im->free_poincare_polynomial ();
  
  return triple<multivariate_laurentpoly<Z>,
		multivariate_laurentpoly<Z>,
		multivariate_laurentpoly<Z> > (P, sq1_P, sq2_P);
}

void
compute_show_kh_sq (knot_desc desc
#if 0
		    ,
		    multivariate_laurentpoly<Z> orig_P,
		    multivariate_laurentpoly<Z> orig_sq1_P,
		    multivariate_laurentpoly<Z> orig_sq2_P
#endif
		    )
{
  knot_diagram kd = desc.diagram ();
  
  printf ("computing %s...\n", kd.name.c_str ());
  
  cube<Z2> c (kd);
  mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
  
  chain_complex_simplifier<Z2> s (c.khC, d, 1);
  
  steenrod_square sq (c, d, s);
  mod_map<Z2> sq1 = sq.sq1 ();
  mod_map<Z2> sq2 = sq.sq2 ();
  
  assert (sq1.compose (sq1) == 0);
  assert (sq2.compose (sq2) + sq1.compose (sq2).compose (sq1) == 0);
  
  multivariate_laurentpoly<Z> P = s.new_C->free_poincare_polynomial ();
  
  ptr<const free_submodule<Z2> > sq1_im = sq1.image ();
  multivariate_laurentpoly<Z> sq1_P = sq1_im->free_poincare_polynomial ();
  
  ptr<const free_submodule<Z2> > sq2_im = sq2.image ();
  multivariate_laurentpoly<Z> sq2_P = sq2_im->free_poincare_polynomial ();
  
  printf ("  P "); display (P);
  printf ("  sq1_P "); display (sq1_P);
  printf ("  sq2_P "); display (sq2_P);
  
#if 0
  assert (P == orig_P);
  assert (sq1_P == orig_sq1_P);
  assert (sq2_P == orig_sq2_P);
#endif
}

void
load (map<knot_desc,
	  triple<multivariate_laurentpoly<Z>,
		 multivariate_laurentpoly<Z>,
		 multivariate_laurentpoly<Z> > > &knot_kh_sq,
      knot_desc desc)
{
  char buf[1000];
  if (desc.t == knot_desc::TORUS)
    {
      sprintf (buf, "kh_sq/T.dat");
    }
  else
    {
      unsigned j0 = desc.j;
      
      switch (desc.t)
	{
	case knot_desc::ROLFSEN:
	  sprintf (buf, "kh_sq/%d_%d.dat", desc.i, j0);
	  break;

	case knot_desc::HTW:
	  sprintf (buf, "kh_sq/K%d_%d.dat", desc.i, j0);
	  break;

	case knot_desc::MT:
	  sprintf (buf, "kh_sq/L%d_%d.dat", desc.i, j0);
	  break;

	default: abort ();
	}
    }
  
  printf ("loading %s...\n", buf);
  
  reader r (buf);
  map<knot_desc,
      triple<multivariate_laurentpoly<Z>,
	     multivariate_laurentpoly<Z>,
	     multivariate_laurentpoly<Z> > > m (r);
  for (map<knot_desc,
	   triple<multivariate_laurentpoly<Z>,
		  multivariate_laurentpoly<Z>,
		  multivariate_laurentpoly<Z> > >::const_iter i = m; i; i ++)
    {
      // ??? check computations agree!!
      if (! (knot_kh_sq % i.key ()))
	{
#if 0
	  assert (i.key ().t == desc.t);
	  assert (i.key ().i == desc.i);
	  assert (desc.j <= i.key ().j);
	  assert (i.key ().j <= desc.j + 4000);
	  
	  if (i.key ().t == knot_desc::MT
	      && i.key ().i == 11
	      && (i.key ().j == 862
		  || i.key ().j == 865))
	    compute_show_kh_sq (i.key (),
				i.val ().first,
				i.val ().second,
				i.val ().third);
#endif
	  
	  knot_kh_sq.push (i.key (), i.val ());
	}
    }
  
  printf ("done.\n");
}

int
main ()
{
#if 0
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
  
#if 0
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
  
#if 1
  compute_show_kh_sq (knot_desc (knot_desc::ROLFSEN, 8, 19));
  
  map<knot_desc,
      triple<multivariate_laurentpoly<Z>,
	     multivariate_laurentpoly<Z>,
	     multivariate_laurentpoly<Z> > > knot_kh_sq;
  
  for (unsigned i = 1; i <= 10; i ++)
    for (unsigned j = 1; j <= rolfsen_crossing_knots (i); j += 4000)
      {
	load (knot_kh_sq, knot_desc (knot_desc::ROLFSEN, i, j));
      }
  
  for (unsigned i = 1; i <= 12; i ++)
    for (unsigned j = 1; j <= htw_knots (i); j += 4000)
      {
	load (knot_kh_sq, knot_desc (knot_desc::HTW, i, j));
      }
  
  for (unsigned i = 1; i <= 13; i ++)
    for (unsigned j = 1; j <= mt_links (i); j += 4000)
      {
	load (knot_kh_sq, knot_desc (knot_desc::MT, i, j));
      }
  
  map<multivariate_laurentpoly<Z>,
      triple<knot_desc,
	     multivariate_laurentpoly<Z>,
	     multivariate_laurentpoly<Z> > > m;
  
  multivariate_laurentpoly<Z> groups;
  
  for (map<knot_desc,
	   triple<multivariate_laurentpoly<Z>,
		  multivariate_laurentpoly<Z>,
		  multivariate_laurentpoly<Z> > >::const_iter i = knot_kh_sq; i; i ++)
    {
      multivariate_laurentpoly<Z> P = i.val ().first,
	P_sq1 = i.val ().second,
	P_sq2 = i.val ().third;
      
      pair<triple<knot_desc,
		  multivariate_laurentpoly<Z>,
		  multivariate_laurentpoly<Z> > &,
	   bool> p = m.find (P);
      if (p.second)
	{
	  groups += P;
	  
	  if (p.first.second != P_sq1
	      || p.first.third != P_sq2)
	    {
	      printf ("DIFFER  ");
	      display (P);
	      
	      printf ("  %s\n", p.first.first.name ().c_str ());
	      printf ("    sq1 "); display (p.first.second);
	      printf ("    sq2 "); display (p.first.third);

	      printf ("  %s\n", i.key ().name ().c_str ());
	      printf ("    sq1 "); display (P_sq1);
	      printf ("    sq2 "); display (P_sq2);
	    }
	}
      else
	{
	  p.first.first = i.key ();
	  p.first.second = P_sq1;
	  p.first.third = P_sq2;
	}
    }
  
  printf ("|groups| = %d\n", groups.card ());
  
  printf ("done.\n");
#endif
  
#if 0
  reader r ("kh_knot_map.dat");
  map<multivariate_laurentpoly<Z>,
      set<triple<unsigned, int, unsigned> > > kh_knot_map (r);
  
  for (map<multivariate_laurentpoly<Z>,
	   set<triple<unsigned, int, unsigned> > >::const_iter i = kh_knot_map; i; i ++)
    {
      if (i.val ().card () == 1)
	continue;
      
      printf ("group\n");
      bool first = 1;
      multivariate_laurentpoly<Z> P, sq1_P, sq2_P;
      
      for (set_const_iter<triple<unsigned, int, unsigned> > j = i.val (); j; j ++)
	{
	  knot_diagram kd (htw_knot (j.val ().first,
				     j.val ().second,
				     j.val ().third));
	  printf ("  "); show (kd); newline ();
	  
	  triple<multivariate_laurentpoly<Z>,
		 multivariate_laurentpoly<Z>,
		 multivariate_laurentpoly<Z> > t = square (kd);
#if 0
	  display ("t.first = ", t.first);
	  display ("i.key () = ", i.key ());
	  assert (t.first == i.key ());
#endif
	  
	  if (first)
	    {
	      P = t.first;
	      sq1_P = t.second;
	      sq2_P = t.third;
	      first = 0;
	    }
	  else
	    {
	      assert (P == t.first);
	      if (sq1_P != t.second)
		printf ("  prev sq1_P != sq1_P\n");
	      if (sq2_P != t.third)
		printf ("  prev sq2_P != sq2_P\n");
	    }
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
