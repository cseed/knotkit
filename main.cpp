
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

unsigned
homological_width (ptr<const module<Z2> > H)
{
  int maxd = -1000,
    mind = 1000;
  set<grading> gs = H->gradings ();
  for (set_const_iter<grading> gg = gs; gg; gg ++)
    {
      grading hq = gg.val ();
      int d = 2 * hq.h - hq.q;
      if (d < mind)
	mind = d;
      if (d > maxd)
	maxd = d;
    }
  int dwidth = maxd - mind;
  
  assert (is_even (dwidth));
  unsigned hwidth = (dwidth / 2) + 1;
  return hwidth;
}

basedvector<unsigned, 1> hwidth_knots;

void
load (map<knot_desc,
	  pair<mod_map<Z2>, mod_map<Z2> > > &knot_kh_sq,
      knot_desc desc)
{
  char buf[1000];
  if (desc.t == knot_desc::TORUS)
    {
      sprintf (buf, "knot_kh_sq/T.dat");
    }
  else
    {
      unsigned j0 = desc.j;
      
      switch (desc.t)
	{
	case knot_desc::ROLFSEN:
	  sprintf (buf, "knot_kh_sq/%d_%d.dat", desc.i, j0);
	  break;

	case knot_desc::HTW:
	  sprintf (buf, "knot_kh_sq/K%d_%d.dat", desc.i, j0);
	  break;

	case knot_desc::MT:
	  sprintf (buf, "knot_kh_sq/L%d_%d.dat", desc.i, j0);
	  break;

	default: abort ();
	}
    }
  
  struct stat stat_buf;
  if (stat (buf, &stat_buf) != 0)
    {
      if (errno == ENOENT)
	return;
      
      stderror ("stat: %s", buf);
      exit (EXIT_FAILURE);
    }
  
  printf ("loading %s...\n", buf);
  
  file_reader r (buf, 1);
  map<knot_desc,
      pair<mod_map<Z2>, mod_map<Z2> > > m (r);
  for (map<knot_desc,
	   pair<mod_map<Z2>, mod_map<Z2> > >::const_iter i = m; i; i ++)
    {
      mod_map<Z2> sq1 = i.val ().first;
      ptr<const module<Z2> > H = sq1.domain ();
      unsigned hwidth = homological_width (H);
      // hwidth_knots[hwidth] ++;
      
#if 0
      if (hwidth == 2)
	continue;
      if (i.key ().t == knot_desc::MT
	  && i.key ().diagram ().num_components () == 1)
	continue;
#endif
      
      knot_kh_sq.push (i.key (), i.val ());
    }
  
  printf ("done.\n");
}

void
load (map<knot_desc,
	  pair<mod_map<Z2>, mod_map<Z2> > > &knot_kh_sq,
      const char *file)
{
  char buf[1000];
  sprintf (buf, "knot_kh_sq/%s", file);
  
  printf ("loading %s...\n", buf);
  
  struct stat stat_buf;
  if (stat (buf, &stat_buf) != 0)
    {
      if (errno == ENOENT)
	{
	  printf ("%s does not exist.\n", buf);
	  return;
	}
      
      stderror ("stat: %s", buf);
      exit (EXIT_FAILURE);
    }
  
  file_reader r (buf, 1);
  map<knot_desc,
      pair<mod_map<Z2>, mod_map<Z2> > > m (r);
  for (map<knot_desc,
	   pair<mod_map<Z2>, mod_map<Z2> > >::const_iter i = m; i; i ++)
    {
#if 0
      mod_map<Z2> sq1 = i.val ().first;
      ptr<const module<Z2> > H = sq1.domain ();
      unsigned hwidth = homological_width (H);
      // hwidth_knots[hwidth] ++;
#endif
      
#if 0
      if (hwidth == 2)
	continue;
      if (i.key ().t == knot_desc::MT
	  && i.key ().diagram ().num_components () == 1)
	continue;
#endif
      
      knot_kh_sq.push (i.key (), i.val ());
    }
  
  printf ("done.\n");
}

static const int block_size = 100;

void
sage_show (FILE *fp,
	   std::string prefix,
	   map<grading, basedvector<unsigned, 1> > hq_gens,
	   ptr<const module<Z2> > H)
{
  fprintf (fp, "%s_hom={", prefix.c_str ());
  bool first = 1;
  for (map<grading, basedvector<unsigned, 1> >::const_iter i = hq_gens; i; i ++)
    {
      if (first)
	first = 0;
      else
	fprintf (fp, ", ");
      fprintf (fp, "(%d, %d): %d", i.key ().h, i.key ().q, i.val ().size ());
    }      
  fprintf (fp, "}\n");
}

void
sage_show (FILE *fp,
	   std::string prefix,
	   map<grading, basedvector<unsigned, 1> > hq_gens,
	   std::string name,
	   grading delta,
	   mod_map<Z2> f)
{
  fprintf (fp, "%s_%s={", prefix.c_str (), name.c_str ());
  
  bool first = 1;
  for (map<grading, basedvector<unsigned, 1> >::const_iter i = hq_gens; i; i ++)
    {
      grading from_hq = i.key ();
      basedvector<unsigned, 1> from_gens = i.val ();
      
      grading to_hq = from_hq + delta;
      if (hq_gens % to_hq)
	{
	  basedvector<unsigned, 1> to_gens = hq_gens(to_hq);
	  
	  if (first)
	    first = 0;
	  else
	    fprintf (fp, ", ");
	  fprintf (fp, "(%d, %d): [", from_hq.h, from_hq.q);
	  bool first2 = 1;
	  for (unsigned j = 1; j <= from_gens.size (); j ++)
	    {
	      unsigned gj = from_gens[j];
	      
	      if (first2)
		first2 = 0;
	      else
		fprintf (fp, ", ");
	      fprintf (fp, "(");
	      for (unsigned k = 1; k <= to_gens.size (); k ++)
		{
		  unsigned gk = to_gens[k];
		  
		  if (k > 1)
		    fprintf (fp, ", ");
		  if (f[gj](gk) == 1)
		    fprintf (fp, "1");
		  else
		    {
		      assert (f[gj](gk) == 0);
		      fprintf (fp, "0");
		    }
		}
	      fprintf (fp, ")");
	    }
	  fprintf (fp, "]");
	}
    }
  fprintf (fp, "}\n");
}

void
sage_show_khsq (FILE *fp,
		std::string prefix,
		ptr<const module<Z2> > H,
		mod_map<Z2> sq1,
		mod_map<Z2> sq2)
{
  unsigned n = H->dim ();
  
  map<grading, basedvector<unsigned, 1> > hq_gens;
  for (unsigned i = 1; i <= H->dim (); i ++)
    hq_gens[H->generator_grading (i)].append (i);
  
  unsigned t = 0;
  for (map<grading, basedvector<unsigned, 1> >::const_iter i = hq_gens; i; i ++)
    t += i.val ().size ();
  assert (t == n);
  
  sage_show (fp, prefix, hq_gens, H);
  sage_show (fp, prefix, hq_gens, "sq1", grading (1, 0), sq1);
  sage_show (fp, prefix, hq_gens, "sq2", grading (2, 0), sq2);
  fprintf (fp, "\n");
}

void
test_sage_show ()
{
  knot_diagram kd (mt_link (11, 0, 449));
  show (kd); newline ();
  
  planar_diagram pd (kd);
  pd.display_knottheory ();
  
  cube<Z2> c (kd);
  mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
  
  chain_complex_simplifier<Z2> s (c.khC, d, 1);
  
  steenrod_square sq (c, d, s);
  mod_map<Z2> sq1 = sq.sq1 ();
  mod_map<Z2> sq2 = sq.sq2 ();
  
  // display ("sq2:\n", sq2);
  
  sage_show_khsq (stdout, "L11n449", sq1.domain (), sq1, sq2);
}

void
dump_sage ()
{
  char buf[1000];
  
#if 0
  for (unsigned i = 10; i >= 1; i --)
    {
      if (i <= 10)
	{
	  for (unsigned j = 1; j <= rolfsen_crossing_knots (i); j += block_size)
	    {
	      load (knot_kh_sq, knot_desc (knot_desc::ROLFSEN, i, j));
	    }
	}
      ...
    }
#endif
  
  for (unsigned i = 12; i >= 1; i --)
    {
      map<knot_desc,
	  pair<mod_map<Z2>, mod_map<Z2> > > knot_kh_sq;
      
#if 0
      sprintf (buf, "K%d.sage", i);
      
      FILE *fp = fopen (buf, "w");
      if (fp == 0)
	{
	  stderror ("fopen: %s", buf);
	  exit (EXIT_FAILURE);
	}
      
      for (unsigned j = 1; j <= htw_knots (i); j += block_size)
	{
	  load (knot_kh_sq, knot_desc (knot_desc::HTW, i, j));
	}
      
      for (map<knot_desc,
	     pair<mod_map<Z2>, mod_map<Z2> > >::const_iter k = knot_kh_sq; k; k ++)
	{
	  sprintf (buf, "K%s", k.key ().name ().c_str ());
	  sage_show_khsq (fp, buf, k.val ().first.domain (), k.val ().first, k.val ().second);
	}
      
      fclose (fp);
#endif

#if 1
      sprintf (buf, "L%d.sage", i);
      
      FILE *fp = fopen (buf, "w");
      if (fp == 0)
	{
	  stderror ("fopen: %s", buf);
	  exit (EXIT_FAILURE);
	}
      
      if (i <= 13)
	{
	  for (unsigned j = 1; j <= mt_links (i); j += block_size)
	    {
	      load (knot_kh_sq, knot_desc (knot_desc::MT, i, j));
	    }
	}
      
      if (i == 14)
	{
	  for (unsigned j = 1; j <= mt_links (14, 0); j += block_size)
	    {
	      load (knot_kh_sq, knot_desc (knot_desc::MT, 14, mt_links (14, 1) + j));
	    }
	}
      
      for (map<knot_desc,
	     pair<mod_map<Z2>, mod_map<Z2> > >::const_iter k = knot_kh_sq; k; k ++)
	{
	  sprintf (buf, "K%s", k.key ().name ().c_str ());
	  sage_show_khsq (fp, buf, k.val ().first.domain (), k.val ().first, k.val ().second);
	}
      
      fclose (fp);
#endif
    }
}

void
convert_knot_sq ()
{
  FILE *fp = open_file ("L14.files", "r");
  char buf[1000];
  
  gzfile_writer w ("L14_sq.dat.gz");
  w.write_unsigned (76516);
  
  set<knot_desc> keys;
  while (fgets (buf, 1000, fp))
    {
      char *p = strchr (buf, '\n');
      if (p)
	{
	  assert (p[1] == 0);
	  *p = 0;
	}
      
      map<knot_desc,
	  pair<mod_map<Z2>, mod_map<Z2> > > knot_sq;
      load (knot_sq, buf);
      
      for (map<knot_desc,
	     pair<mod_map<Z2>, mod_map<Z2> > >::const_iter k = knot_sq; k; k ++)
	{
	  write (w, k.key ());
	  write (w, k.val ());
	}
    }
  
  // write (w, knot_sq);
}

#if 0
void
test_knot_sq ()
{
  const char *knot_sq_files[] = {
    "knot_sq/rolfsen_sq.dat.gz",
    "knot_sq/Ksmall_sq.dat.gz",
    "knot_sq/K11_sq.dat.gz",
    "knot_sq/K12_sq.dat.gz",
    "knot_sq/Lsmall_sq.dat.gz",
    "knot_sq/L11_sq.dat.gz",
    "knot_sq/L12_sq.dat.gz",
    0,
  };
  
  for (unsigned i = 0; knot_sq_files[i] != 0; i ++)
    {
      gzfile_reader r (knot_sq_files[i]);
      map<knot_desc,
	  pair<mod_map<Z2>, mod_map<Z2> > > knot_sq (r);
      printf ("|knot_sq| = %d\n", knot_sq.card ());
    }
}

pair<mod_map<Z2>, mod_map<Z2> > 
compute_kh_sq (const knot_desc &desc)
{
  abort ();
}
#endif

template<class R> pair<ptr<const module<R> >, mod_map<R> >
FU_complex (ptr<const module<R> > CF,
	    mod_map<R> dF,
	    mod_map<R> dU)
{
  unsigned n = CF->dim ();
  
  basedvector<grading, 1> hq (n * 2);
  for (unsigned i = 1; i <= n; i ++)
    hq[i] = hq[n + i] = CF->generator_grading (i);
  
  ptr<const module<R> > CFU = new explicit_module<R> (2 * n,
							basedvector<R, 1> (),
							hq);
  map_builder<R> b (CFU);
  for (unsigned i = 1; i <= n; i ++)
    {
      for (linear_combination_const_iter<R> j = dF[i]; j; j ++)
	{
	  b[i].muladd (j.val (), j.key ());
	  b[n + i].muladd (j.val (), n + j.key ());
	}
      for (linear_combination_const_iter<R> j = dU[i]; j; j ++)
	{
	  b[i].muladd (j.val (), n + j.key ());
	}
    }
  mod_map<R> dFU (b);
  // assert (dFU.compose (dFU) == 0);
  // dFU.check_sseq_graded ();
  return pair<ptr<const module<R> >, mod_map<R> > (CFU, dFU);
}

void
compute_twistedU ()
{
#if 0
  for (unsigned i = 1; i <= 11; i ++)
    for (unsigned j = 1; j <= htw_knots (i, 0); j ++)
      {
	knot_diagram kd (htw_knot (i, 0, j));
      ...}
#endif
  for (unsigned i = 1; i <= 10; i ++)
    for (unsigned j = 1; j <= mt_links (i, 0); j ++)
      {
	knot_diagram kd (mt_link (i, 0, j));
	
	kd.marked_edge = 1;
	show (kd); newline ();
	
	typedef spanning_tree_complex<Z2>::R R;
	
	spanning_tree_complex<Z2> c (kd);
	ptr<const module<R> > C = c.C;
	
	mod_map<R> d2 = c.twisted_d2 ();
	assert (d2.compose (d2) == 0);
	
	mod_map<R> d2U = c.twisted_d2Un (1);
	assert (d2.compose (d2U) == d2U.compose (d2));
	
	chain_complex_simplifier<R> s (C, d2, 2);
	assert (s.new_d == 0);
	printf ("|s.new_C| = %d\n", s.new_C->dim ());
	
	pair<ptr<const module<R> >, mod_map<R> > p = FU_complex (C, d2, d2U);
	ptr<const module<R> > CFU = p.first;
	mod_map<R> dFU = p.second;
	
	chain_complex_simplifier<R> s2 (CFU, dFU, 2);
	assert (s2.new_d == 0);
	printf ("|s2.new_C| = %d\n", s2.new_C->dim ());
	
	if (s2.new_C->dim () < 2 * s.new_C->dim ())
	  printf (" > EXAMPLE\n");
      }
}


void
test_forgetful_ss ()
{
  typedef fraction_field<polynomial<Z2> > R;
  
  for (unsigned i = 1; i <= 12; i ++)
    for (unsigned j = 1; j <= mt_links (i, 0); j ++)
      {
	knot_diagram kd (mt_link (i, 0, j));
	unsigned n = kd.num_components ();
	if (n < 2)
	  continue;
	
	show (kd); newline ();
	
	unionfind<1> u (kd.num_edges ());
	for (unsigned i = 1; i <= kd.n_crossings; i ++)
	  {
	    u.join (kd.ept_edge (kd.crossings[i][1]),
		    kd.ept_edge (kd.crossings[i][3]));
	    u.join (kd.ept_edge (kd.crossings[i][2]),
		    kd.ept_edge (kd.crossings[i][4]));
	  }
	
	printf ("%d components:\n", n);
	
	map<unsigned, unsigned> root_comp;
	unsigned t = 0;
	for (unsigned i = 1; i <= kd.num_edges (); i ++)
	  {
	    if (u.find (i) == i)
	      {
		++ t;
		root_comp.push (i, t);
	      }
	  }
	assert (t == n);
	
	printf ("  kd w: %d\n", kd.writhe ());
	
	multivariate_laurentpoly<Z> disj_P = 1;
	for (unsigned k = 1; k <= n; k ++)
	  {
	    knot_diagram comp (SUBLINK, smallbitset (n, unsigned_2pow (k - 1)), kd);
	    
	    unsigned w = 0;
	    for (unsigned i = 1; i <= kd.n_crossings; i ++)
	      {
		if (root_comp(u.find (kd.ept_edge (kd.crossings[i][1]))) == k
		    && root_comp(u.find (kd.ept_edge (kd.crossings[i][2]))) == k)
		  {
		    if (kd.is_to_ept (kd.crossings[i][1]) == kd.is_to_ept (kd.crossings[i][4]))
		      w ++;
		    else
		      w --;
		  }
	      }
	    
	    printf ("  % 2d w: %d\n", k, w);
	    
	    cube<R> c (comp);
	    mod_map<R> d = c.compute_d (1, 0, 0, 0, 0);
	  
	    chain_complex_simplifier<R> s (c.khC, d, 1);
	    assert (s.new_d == 0);
	    
	    multivariate_laurentpoly<Z> P = s.new_C->free_ell_poincare_polynomial ();
	    printf ("  % 2d P: ", k);
	    display (P);
	    
	    disj_P *= (P
		       * multivariate_laurentpoly<Z> (1, VARIABLE, 1, w)
		       );
	  }
	
	cube<R> c (kd);
	
#if 0
	for (unsigned i = 0; i < c.n_resolutions; i ++)
	  {
	    smallbitset state (kd.n_crossings, i);
	    smoothing s (kd, state);
	    s.show_self (kd, state);
	    newline ();
	  }
#endif
	
	mod_map<R> untwisted_d = c.compute_d (1, 0, 0, 0, 0);
	assert (untwisted_d.compose (untwisted_d) == 0);
	
	chain_complex_simplifier<R> s1 (c.khC, untwisted_d, 1);
	assert (s1.new_d == 0);
	
	multivariate_laurentpoly<Z> P1 = s1.new_C->free_ell_poincare_polynomial ();
	display ("  link P      : ", P1);
	
	display ("  disj_P (adj): ", disj_P);
	
	mod_map<R> d = untwisted_d;
	for (unsigned x = 1; x <= kd.n_crossings; x ++)
	  {
	    unsigned r1 = u.find (kd.ept_edge (kd.crossings[x][1])),
	      r2 = u.find (kd.ept_edge (kd.crossings[x][2]));
	    
	    d = d + c.compute_dinv (x)*(R (polynomial<Z2> (1, root_comp(r1))) + 1);
	    d = d + c.compute_dinv (x)*(R (polynomial<Z2> (1, root_comp(r2))) + 1);
	    
#if 0					
	    R hbar (polynomial<Z2> (1, 1));
	    R hbarp1 = hbar + 1;
	    
	    // if (u.find (kd.ept_edge (kd.crossings[x][1])) != u.find (kd.ept_edge (kd.crossings[x][2])))
	    
	    if (u.find (kd.ept_edge (kd.crossings[x][1])) != comp1_root)
	      d = d + c.compute_dinv (x)*hbarp1;
	    
	    if (u.find (kd.ept_edge (kd.crossings[x][2])) != comp1_root)
	      d = d + c.compute_dinv (x)*hbarp1;
#endif
	  }
	
#if 0
	mod_map<R> h = d.compose (d);
	display ("h:\n", h);
#endif
	
	assert (d.compose (d) == 0);
	
	// display ("d:\n", d);
	
	chain_complex_simplifier<R> s2 (c.khC, d, -1);
	assert (s2.new_d == 0);

	multivariate_laurentpoly<Z> P2 = (s2.new_C->free_ell_poincare_polynomial ()
					  * multivariate_laurentpoly<Z> (1, VARIABLE, 1, kd.writhe ())
					  );
	display ("  Einf P (adj): ", P2);
	
	if (disj_P == P2)
	  printf (" disj_P == Einf P (adj): YES!\n");
	else
	  printf (" disj_P != Einf P (adj): NO :-(!\n");
	fflush (stdout);
      }
}

void
compute_forgetful_torsion ()
{
  typedef polynomial<Zp<2> > R;
  
  for (unsigned i = 1; i <= 12; i ++)
    for (unsigned j = 1; j <= mt_links (i, 0); j ++)
      {
	knot_diagram kd (mt_link (i, 0, j));
	unsigned n = kd.num_components ();
	if (n != 2)
	  continue;
	
	show (kd); newline ();
	
	unionfind<1> u (kd.num_edges ());
	for (unsigned i = 1; i <= kd.n_crossings; i ++)
	  {
	    u.join (kd.ept_edge (kd.crossings[i][1]),
		    kd.ept_edge (kd.crossings[i][3]));
	    u.join (kd.ept_edge (kd.crossings[i][2]),
		    kd.ept_edge (kd.crossings[i][4]));
	  }
	
	map<unsigned, unsigned> root_comp;
	unsigned t = 0;
	for (unsigned i = 1; i <= kd.num_edges (); i ++)
	  {
	    if (u.find (i) == i)
	      {
		++ t;
		root_comp.push (i, t);
	      }
	  }
	assert (t == n);
	
	cube<R> c (kd);
#if 0
	for (unsigned i = 0; i < c.n_resolutions; i ++)
	  {
	    smallbitset state (kd.n_crossings, i);
	    smoothing s (kd, state);
	    s.show_self (kd, state);
	    newline ();
	  }
#endif
	
	mod_map<R> untwisted_d = c.compute_d (1, 0, 0, 0, 0);
	assert (untwisted_d.compose (untwisted_d) == 0);
	
	mod_map<R> d = untwisted_d;
	for (unsigned x = 1; x <= kd.n_crossings; x ++)
	  {
	    unsigned r1 = u.find (kd.ept_edge (kd.crossings[x][1])),
	      r2 = u.find (kd.ept_edge (kd.crossings[x][2]));
	    
	    if (r1 != r2)
	      d = d + c.compute_dinv (x)*(R (1, 1));
	  }
	
#if 0
	mod_map<R> h = d.compose (d);
	display ("h:\n", h);
#endif
	
	assert (d.compose (d) == 0);
	
	// display ("d:\n", d);
	
	chain_complex_simplifier<R> s (c.khC, d, 0);
	printf ("|s.new_C| = %d\n", s.new_C->dim ());
	display ("s.new_d:\n", s.new_d);
	
#if 0
	ptr<const free_submodule<R> > ker = s.new_d.kernel ();
	printf ("|ker| = %d\n", ker->dim ());
	
	ptr<const free_submodule<R> > im = s.new_d.image ();
	printf ("|im| = %d\n", im->dim ());
#endif
	
	ptr<const module<R> > H = s.new_d.homology ();
	display ("H:\n", *H);
	// printf ("dim H = %d, free rank H = %d\n", H->dim (), H->free_rank ());
      }
}

void
test_forgetful_signs ()
{
  typedef Q R;
  
#if 1
  for (unsigned i = 1; i <= 14; i ++)
    for (unsigned j = 1; j <= mt_links (i, 1); j ++)
      {
	knot_diagram kd (mt_link (i, 1, j));
#endif
#if 0
  for (unsigned i = 2; i <= 16; i ++)
    for (unsigned j = i; j <= 16; j ++)
      {
	knot_diagram kd (torus_knot (i, j));
	assert (kd.n_crossings == (i - 1) * j
		&& kd.n_crossings <= i * (j - 1));
	if (kd.n_crossings > 13)
	  continue;
      }
#endif
	
	unsigned n = kd.num_components ();
	if (n < 2)
	  continue;
	assert (n < 97);
	
	show (kd); newline ();
	
	unionfind<1> u (kd.num_edges ());
	for (unsigned i = 1; i <= kd.n_crossings; i ++)
	  {
	    u.join (kd.ept_edge (kd.crossings[i][1]),
		    kd.ept_edge (kd.crossings[i][3]));
	    u.join (kd.ept_edge (kd.crossings[i][2]),
		    kd.ept_edge (kd.crossings[i][4]));
	  }
	
	map<unsigned, unsigned> root_comp;
	unsigned t = 0;
	for (unsigned i = 1; i <= kd.num_edges (); i ++)
	  {
	    if (u.find (i) == i)
	      {
		++ t;
		root_comp.push (i, t);
	      }
	  }
	assert (t == n);
	
	basedvector<R, 1> comp_weight (n);
	for (unsigned i = 1; i <= n; i ++)
	  comp_weight[i] = R ((int)i);
	
	map<unsigned, R> crossing_over_sign;
	
	// crossings
	set<unsigned> pending;
	set<unsigned> finished;
	
	crossing_over_sign.push (1, R (1));
	pending.push (1);
  
	while (pending.card () > 0)
	  {
	    unsigned x = pending.pop ();
	    finished.push (x);
      
	    R s = crossing_over_sign(x);
      
	    for (unsigned j = 1; j <= 4; j ++)
	      {
		unsigned p = kd.crossings[x][j];
		R t = kd.is_over_ept (p) ? s : -s;  // sign of (x, p)
	  
		unsigned q = kd.edge_other_ept (p);
		unsigned x2 = kd.ept_crossing[q];
	  
		R u = kd.is_over_ept (q) ? -t : t;
	  
		if (crossing_over_sign % x2)
		  assert (crossing_over_sign(x2) == u);
		else
		  crossing_over_sign.push (x2, u);
	  
		if (! (finished % x2))
		  pending += x2;
	      }
	  }
	assert (finished.card () == kd.n_crossings);
	
#if 0
	printf ("crossing_over_sign:\n");
	for (unsigned i = 1; i <= kd.n_crossings; i ++)
	  {
	    printf ("  % 2d: ", i);
	    display (crossing_over_sign(i));
	  }
#endif
	
	cube<R> c (kd);
#if 0
	for (unsigned i = 0; i < c.n_resolutions; i ++)
	  {
	    smallbitset state (kd.n_crossings, i);
	    smoothing s (kd, state);
	    s.show_self (kd, state);
	    newline ();
	  }
#endif
  
	mod_map<R> untwisted_d = c.compute_d (1, 0, 0, 0, 0);
	assert (untwisted_d.compose (untwisted_d) == 0);
	
	mod_map<R> d = untwisted_d;
	for (unsigned x = 1; x <= kd.n_crossings; x ++)
	  {
	    unsigned p1 = kd.crossings[x][1],
	      p2 = kd.crossings[x][2];
	    assert (kd.is_over_ept (p2));
	    
	    unsigned r1 = u.find (kd.ept_edge (p1)),
	      r2 = u.find (kd.ept_edge (p2));
	    
	    unsigned c1 = root_comp(r1),
	      c2 = root_comp(r2);
	    
	    if (c1 != c2)
	      {
		R s = crossing_over_sign(x);
		
		R w_under = comp_weight[c1];
		R w_over = comp_weight[c2];
		
		d = d + c.compute_dinv (x)*(s*(w_over - w_under))
		  ;
	      }
	  }
	
	// display ("d:\n", d);
	
	// mod_map<Z> h = d.compose (d);
	// display ("h:\n", h);
	
	assert (d.compose (d) == 0);
	
	ptr<const module<R> > Ek = c.khC;
	mod_map<R> dk = d;
	
	unsigned kinf;
	for (int dq = 0;; dq -= 2)
	  {
	    chain_complex_simplifier<R> s (Ek, dk, dq);
	    Ek = s.new_C;
	    dk = s.new_d;
           
	    printf ("|E%d| = %d\n", ((- dq) / 2) + 1, Ek->dim ());
	    if (dk == 0)
	      {
		kinf = ((- dq) / 2);
		break;
	      }
	  }
	
	unsigned total_lk = kd.total_linking_number ();
	
	unsigned trivial_bound = total_lk == 0 ? 2 : total_lk;
	
	printf ("kinf = %d, trivial bound = %d (total_lk = %d)\n",
		kinf, trivial_bound, total_lk);
	
	if (trivial_bound < kinf)
         printf (" > BETTER\n");
      }
}

template<class R> sseq
compute_forgetfulss (knot_diagram &kd,
		     basedvector<R, 1> comp_weight)
{
  unsigned n = kd.num_components ();
  
  unionfind<1> u (kd.num_edges ());
  for (unsigned i = 1; i <= kd.n_crossings; i ++)
    {
      u.join (kd.ept_edge (kd.crossings[i][1]),
	      kd.ept_edge (kd.crossings[i][3]));
      u.join (kd.ept_edge (kd.crossings[i][2]),
	      kd.ept_edge (kd.crossings[i][4]));
    }
	
  map<unsigned, unsigned> root_comp;
  unsigned t = 0;
  for (unsigned i = 1; i <= kd.num_edges (); i ++)
    {
      if (u.find (i) == i)
	{
	  ++ t;
	  root_comp.push (i, t);
	}
    }
  assert (t == n);
  
  assert (comp_weight.size () == n);
  
  map<unsigned, R> crossing_over_sign;
	
  // crossings
  set<unsigned> pending;
  set<unsigned> finished;
	
  crossing_over_sign.push (1, R (1));
  pending.push (1);
  
  while (pending.card () > 0)
    {
      unsigned x = pending.pop ();
      finished.push (x);
      
      R s = crossing_over_sign(x);
      
      for (unsigned j = 1; j <= 4; j ++)
	{
	  unsigned p = kd.crossings[x][j];
	  R t = kd.is_over_ept (p) ? s : -s;  // sign of (x, p)
	  
	  unsigned q = kd.edge_other_ept (p);
	  unsigned x2 = kd.ept_crossing[q];
	  
	  R u = kd.is_over_ept (q) ? -t : t;
	  
	  if (crossing_over_sign % x2)
	    assert (crossing_over_sign(x2) == u);
	  else
	    crossing_over_sign.push (x2, u);
	  
	  if (! (finished % x2))
	    pending += x2;
	}
    }
  assert (finished.card () == kd.n_crossings);
  
  cube<R> c (kd);
#if 0
  for (unsigned i = 0; i < c.n_resolutions; i ++)
    {
      smallbitset state (kd.n_crossings, i);
      smoothing s (kd, state);
      s.show_self (kd, state);
      newline ();
    }
#endif
  
  mod_map<R> untwisted_d = c.compute_d (1, 0, 0, 0, 0);
  assert (untwisted_d.compose (untwisted_d) == 0);
	
  mod_map<R> d = untwisted_d;
  for (unsigned x = 1; x <= kd.n_crossings; x ++)
    {
      unsigned p1 = kd.crossings[x][1],
	p2 = kd.crossings[x][2];
      assert (kd.is_over_ept (p2));
	    
      unsigned r1 = u.find (kd.ept_edge (p1)),
	r2 = u.find (kd.ept_edge (p2));
	    
      unsigned c1 = root_comp(r1),
	c2 = root_comp(r2);
	    
      if (c1 != c2)
	{
	  R s = crossing_over_sign(x);
		
	  R w_under = comp_weight[c1];
	  R w_over = comp_weight[c2];
		
	  d = d + c.compute_dinv (x)*(s*(w_over - w_under))
	    ;
	}
    }
  
  assert (d.compose (d) == 0);
  
  ptr<const module<R> > C = c.khC;
  // d
  
  int minh = 1000,
    maxh = -1000,
    minq = 1000,
    maxq = -1000;
  
  for (unsigned i = 1; i <= C->dim (); i ++)
    {
      grading hq = C->generator_grading (i);
      if (hq.h < minh)
	minh = hq.h;
      if (hq.h > maxh)
	maxh = hq.h;
      if (hq.q < minq)
	minq = hq.q;
      if (hq.q > maxq)
	maxq = hq.q;
    }
  
  sseq_bounds bounds (minh, maxh, minq, maxq);
  
  basedvector<sseq_page, 1> pages;
  
  for (unsigned dq = 0;;)
    {
      chain_complex_simplifier<R> s (C, d, dq);
      C = s.new_C;
      d = s.new_d;
      
      dq -= 2;
      
      sseq_page pg (bounds);
      for (unsigned i = 1; i <= C->dim (); i ++)
	{
	  grading hq = C->generator_grading (i);
	  pg.rank[hq.h - bounds.minh][hq.q - bounds.minq] ++;
	}
      pages.append (pg);
      
#if 0
      mod_map<R> dk = d.graded_piece (grading (dq + 1, dq));
      dk.check_grading (grading (dq + 1, dq));
      
      ptr<const free_submodule<R> > dk_im = dk.image ();
      for (unsigned i = 1; i <= dk_im->dim (); i ++)
	{
	  grading hq = dk_im->generator_grading (i);
	  pg.im_rank[hq.h - bounds.minh][hq.q - bounds.minq] ++;
	}
#endif
      
#if 0
      printf ("E_%d: ", (-dq) / 2);
      display (C->free_poincare_polynomial ());
#endif
      
      if (d == 0)
	break;
    }
  
  return sseq (bounds, pages);
}

void
compute_lee_bound ()
{
  typedef Z2 R;
  
#if 0
  for (unsigned i = 2; i <= 16; i ++)
    for (unsigned j = i; j <= 16; j ++)
      {
	knot_diagram kd (torus_knot (i, j));
	assert (kd.n_crossings == (i - 1) * j
		&& kd.n_crossings <= i * (j - 1));
	if (kd.n_crossings > 13)
	  continue;
      }
#endif
#if 1
  for (unsigned i = 1; i <= 10; i ++)
    for (unsigned j = 1; j <= rolfsen_crossing_knots (i); j ++)
      {
	knot_diagram kd (rolfsen_knot (i, j));
#endif
	
	show (kd); newline ();
	
	cube<R> c (kd);
#if 0
	for (unsigned i = 0; i < c.n_resolutions; i ++)
	  {
	    smallbitset state (kd.n_crossings, i);
	    smoothing s (kd, state);
	    s.show_self (kd, state);
	    newline ();
	  }
#endif
	
	mod_map<R> d = c.compute_d (1, 0, 0, 0, 0);
	assert (d.compose (d) == 0);
	
	for (unsigned x = 1; x <= kd.n_crossings; x ++)
	  d = d + c.H_i (x);
	
	assert (d.compose (d) == 0);
	
	ptr<const module<R> > Ek = c.khC;
	mod_map<R> dk = d;
	
	unsigned kinf;
	for (int dq = 0;; dq += 2)
	  {
	    chain_complex_simplifier<R> s (Ek, dk, dq);
	    Ek = s.new_C;
	    dk = s.new_d;
	    
	    printf ("|E%d| = %d\n", (dq / 2) + 1, Ek->dim ());
	    if (dk == 0)
	      {
		kinf = (dq / 2);
		break;
	      }
	  }
	
	printf ("kinf = %d\n", kinf);
      }
}

void
test_crossing_ss ()
{
  typedef multivariate_laurentpoly<Z2> R;
  
  for (unsigned i = 1; i <= 14; i ++)
    for (unsigned j = 1; j <= htw_knots (i); j ++)
      {
	knot_diagram kd (htw_knot (i, j));
	show (kd); newline ();
	
	cube<R> c (kd);
#if 0
	for (unsigned i = 0; i < c.n_resolutions; i ++)
	  {
	    smallbitset state (kd.n_crossings, i);
	    smoothing s (kd, state);
	    s.show_self (kd, state);
	    newline ();
	  }
#endif
	
	mod_map<R> untwisted_d = c.compute_d (1, 0, 0, 0, 0);
	assert (untwisted_d.compose (untwisted_d) == 0);
	
	mod_map<R> d = untwisted_d;
	for (unsigned x = 1; x <= kd.n_crossings; x ++)
	  {
	    d = d + c.compute_dinv (x)*(R (1, VARIABLE, x));
	  }
	
	display ("d:\n", d);
	
	mod_map<R> h = d.compose (d);
	display ("h:\n", h);
      }
}

/* Kh homotopy type:
   - check for CP^2,
   - mutation invariance
   - K-theory */

map<grading, basedvector<int, 1> >
compute_st (mod_map<Z2> sq1,
	    mod_map<Z2> sq2)
{
  ptr<const module<Z2> > H = sq1.domain ();
  
  map<grading, basedvector<int, 1> > st;
  
  bool first = 1;
  set<grading> gs = H->gradings ();
  for (set_const_iter<grading> gg = gs; gg; gg ++)
    {
      grading hq = gg.val (),
	h1q (hq.h + 1, hq.q),
	h2q (hq.h + 2, hq.q);
      
      ptr<const free_submodule<Z2> > H_hq = H->graded_piece (hq),
	H_h1q = H->graded_piece (h1q),
	H_h2q = H->graded_piece (h2q);
      
      mod_map<Z2> S = sq2.restrict (H_hq, H_h2q),
	A = sq1.restrict (H_hq, H_h1q),
	B = sq1.restrict (H_h1q, H_h2q);
      
      ptr<const free_submodule<Z2> > S_im = S.image (),
	A_ker = A.kernel (),
	B_im = B.image ();
      ptr<const free_submodule<Z2> > inter = S_im->intersection (B_im);
      
      mod_map<Z2> S_res = S.restrict_from (A_ker);
      ptr<const free_submodule<Z2> > S_res_im = S_res.image ();
      
      ptr<const free_submodule<Z2> > res_inter = S_res_im->intersection (B_im);
      
      int r1 = S_im->dim ();
      int r2 = S_res_im->dim ();
      int r3 = inter->dim ();
      int r4 = res_inter->dim ();
      
      int s1 = r2 - r4,
	s2 = r1 - r2 - r3 + r4,
	s3 = r4,
	s4 = r3 - r4;
      
      if (!s1 && !s2 && !s3 && !s4)
	continue;
      
      basedvector<int, 1> s (4);
      s[1] = s1;
      s[2] = s2;
      s[3] = s3;
      s[4] = s4;
      
      st.push (hq, s);
    }
  
  return st;
}

void
display_st (map<grading, basedvector<int, 1> > st)
{
  bool first = 1;
  for (map_const_iter<grading, basedvector<int, 1> > i = st; i; i ++)
    {
      grading hq = i.key ();
      int s1 = i.val ()[1],
	s2 = i.val ()[2],
	s3 = i.val ()[3],
	s4 = i.val ()[4];
      if (s1 == 0 && s2 == 0 && s3 == 0 && s4 == 0)
	continue;
      if (first)
	first = 0;
      else
	printf (",");
      printf ("$(%d, %d) \\mapsto (%d, %d, %d, %d)$", hq.h, hq.q, s1, s2, s3, s4);
    }
  newline ();
}

void
test_kh_htt_stronger ()
{
  map<pair<multivariate_laurentpoly<Z>,
	   multivariate_laurentpoly<Z> >,
      pair<knot_desc,
	   map<grading, basedvector<int, 1> > > > P_sq1_knot_st;
  
  const char *knot_sq_files[] = {
    "knot_sq/rolfsen_sq.dat.gz",
    "knot_sq/Ksmall_sq.dat.gz",
    "knot_sq/K11_sq.dat.gz",
    "knot_sq/K12_sq.dat.gz",
    "knot_sq/K13_sq.dat.gz",
    "knot_sq/K14_sq.dat.gz",
    "knot_sq/Lsmall_sq.dat.gz",
    "knot_sq/L11_sq.dat.gz",
    "knot_sq/L12_sq.dat.gz",
    "knot_sq/L13_sq.dat.gz",
    "knot_sq/L14_sq.dat.gz",
    0,
  };
  
  for (unsigned i = 0; knot_sq_files[i]; i ++)
    {
      gzfile_reader r (knot_sq_files[i]);
      
      printf ("loading %s...\n", knot_sq_files[i]);
      
      unsigned n = r.read_unsigned ();
      for (unsigned i = 1; i <= n; i ++)
	{
	  knot_desc desc (r);
	  pair<mod_map<Z2>, mod_map<Z2> > p1 (r);
	  
	  if (desc.t == knot_desc::MT
	      || desc.t == knot_desc::MT_ALT
	      || desc.t == knot_desc::MT_NONALT)
	    {
	      knot_diagram kd = desc.diagram ();
	      if (kd.num_components () == 1)
		continue;
	    }
	  
	  mod_map<Z2> sq1 = p1.first,
	    sq2 = p1.second;
	  ptr<const module<Z2> > H = sq1.domain ();
	  multivariate_laurentpoly<Z> P = H->free_poincare_polynomial ();
	  
	  unsigned hwidth = homological_width (H);
	  
	  map<grading, basedvector<int, 1> > st
	    = compute_st (sq1, sq2);
	  
	  ptr<const free_submodule<Z2> > sq1_im = sq1.image ();
	  multivariate_laurentpoly<Z> sq1_im_P = sq1_im->free_poincare_polynomial ();
	  
	  pair<pair<knot_desc,
		    map<grading, basedvector<int, 1> > > &,
	       bool> p2 = P_sq1_knot_st.find (pair<multivariate_laurentpoly<Z>,
					           multivariate_laurentpoly<Z> > (P, sq1_im_P));
	  if (p2.second)
	    {
	      if (p2.first.second != st)
		{
		  printf ("DIFFER:\n");
		  printf ("hwidth = %d\n", hwidth);

		  show (p2.first.first);
		  display_st (p2.first.second);
		  
		  show (desc);
		  display_st (st);
		  
		  printf ("Kh[");
		  planar_diagram (p2.first.first.diagram ()).show_knottheory ();
		  printf (", Modulus -> Null][q,t] === Kh[");
		  planar_diagram (desc.diagram ()).show_knottheory ();
		  printf (", Modulus -> Null][q,t]\n");
		}
	    }
	  else
	    {
	      p2.first.first = desc;
	      p2.first.second = st;
	    }
	}
    }
}

void
find_width4_in_h0 ()
{
  const char *knot_sq_files[] = {
    "knot_sq/rolfsen_sq.dat.gz",
    "knot_sq/Ksmall_sq.dat.gz",
    "knot_sq/K11_sq.dat.gz",
    "knot_sq/K12_sq.dat.gz",
    "knot_sq/K13_sq.dat.gz",
    "knot_sq/K14_sq.dat.gz",
    "knot_sq/K15_sq_part.dat.gz",
    0,
  };
  
  for (unsigned i = 0; knot_sq_files[i]; i ++)
    {
      gzfile_reader r (knot_sq_files[i]);
      
      printf ("loading %s...\n", knot_sq_files[i]);
      fflush (stdout);
      
      unsigned n = r.read_unsigned ();
      for (unsigned i = 1; i <= n; i ++)
	{
	  knot_desc desc (r);
	  // show (desc); newline ();
	  
	  pair<mod_map<Z2>, mod_map<Z2> > p (r);
	  
	  mod_map<Z2> sq1 = p.first,
	    sq2 = p.second;
	  ptr<const module<Z2> > H = sq1.domain ();
	  
	  int minq = 1000,
	    maxq = -1000;
	  
	  for (unsigned j = 1; j <= H->dim (); j ++)
	    {
	      grading hq = H->generator_grading (j);
	      if (hq.h == 0)
		{
		  if (hq.q < minq)
		    minq = hq.q;
		  if (hq.q > maxq)
		    maxq = hq.q;
		}
	    }
	  
	  unsigned h0_width = 0;
	  if (minq <= maxq)
	    h0_width = (maxq - minq) / 2 + 1;
	  
	  // printf ("h0_width = %d\n", h0_width);
	  
	  if (h0_width >= 4)
	    {
	      show (desc); newline ();
	      // printf (" > WIDTH 4!!\n");
	    }
	}
    }
}

void
test_knot_sq ()
{
  const char *knot_sq_files[] = {
    "knot_sq/rolfsen_sq.dat.gz",
    "knot_sq/Ksmall_sq.dat.gz",
    "knot_sq/K11_sq.dat.gz",
    "knot_sq/K12_sq.dat.gz",
    "knot_sq/K13_sq.dat.gz",
    "knot_sq/K14_sq.dat.gz",
    "knot_sq/K15_sq_part.dat.gz",
    "knot_sq/Lsmall_sq.dat.gz",
    "knot_sq/L11_sq.dat.gz",
    "knot_sq/L12_sq.dat.gz",
    "knot_sq/L13_sq.dat.gz",
    "knot_sq/L14_sq.dat.gz",
    0,
  };
  
  hashset<knot_desc> mutants;
  for (unsigned i = 11; i <= 15; i ++)
    {
      basedvector<basedvector<unsigned, 1>, 1> groups
	= mutant_knot_groups (i);
      for (unsigned j = 1; j <= groups.size (); j ++)
	{
	  for (unsigned k = 1; k <= groups[j].size (); k ++)
	    mutants += knot_desc (knot_desc::HTW, i, groups[j][k]);
	}
    }
  printf ("|mutants| = %d\n", mutants.card ());
  fflush (stdout);
  
  basedvector<unsigned, 1> width_hist (7);
  for (unsigned i = 1; i <= width_hist.size (); i ++)
    width_hist[i] = 0;
  
  hashmap<knot_desc,
	  pair<mod_map<Z2>, mod_map<Z2> > > mutant_knot_sq,
    mutant_mknot_sq;

  {
    gzfile_reader r ("knot_sq/mut_mknot.dat.gz");
    mutant_mknot_sq = hashmap<knot_desc,
			      pair<mod_map<Z2>, mod_map<Z2> > > (r);
    
    for (hashmap<knot_desc,
	   pair<mod_map<Z2>, mod_map<Z2> > >::const_iter i = mutant_mknot_sq; i; i ++)
      {
	knot_desc desc = i.key ();
	
	// careful: duplicate code!
	pair<mod_map<Z2>, mod_map<Z2> > p = i.val ();
	
	mod_map<Z2> sq1 = p.first,
	  sq2 = p.second;
	ptr<const module<Z2> > H = sq1.domain ();
	
	unsigned w = homological_width (H);
	assert (w >= 1 && w <= 7);
	width_hist[w] ++;
	
	assert (sq1.compose (sq1) == 0);
	assert (sq2.compose (sq2) == sq1.compose (sq2).compose (sq1));
	
	map<grading, basedvector<int, 1> > st
	  = compute_st (sq1, sq2);
	for (map_const_iter<grading, basedvector<int, 1> > i = st; i; i ++)
	  {
	    if (i.val ()[1] != 0)
	      {
		show (desc); newline ();
		printf (" > has CP^2\n");
		fflush (stdout);
	      }
	  }
      }
  }
  
  for (unsigned i = 0; knot_sq_files[i]; i ++)
    {
      gzfile_reader r (knot_sq_files[i]);
      
      printf ("loading %s...\n", knot_sq_files[i]);
      fflush (stdout);
      
      unsigned n = r.read_unsigned ();
      for (unsigned i = 1; i <= n; i ++)
	{
	  knot_desc desc (r);
	  
	  pair<mod_map<Z2>, mod_map<Z2> > p (r);
	  if (mutants % desc)
	    mutant_knot_sq.push (desc, p);
	  
	  mod_map<Z2> sq1 = p.first,
	    sq2 = p.second;
	  ptr<const module<Z2> > H = sq1.domain ();
	  
	  unsigned w = homological_width (H);
	  assert (w >= 1 && w <= 7);
	  width_hist[w] ++;
	  
	  assert (sq1.compose (sq1) == 0);
	  assert (sq2.compose (sq2) == sq1.compose (sq2).compose (sq1));
	  
	  map<grading, basedvector<int, 1> > st
	    = compute_st (sq1, sq2);
	  for (map_const_iter<grading, basedvector<int, 1> > i = st; i; i ++)
	    {
	      if (i.val ()[1] != 0)
		{
		  show (desc); newline ();
		  printf (" > has CP^2\n");
		  fflush (stdout);
		}
	    }
	}
    }
  
  printf ("|mutant_knot_sq| = %d\n", mutant_knot_sq.card ());
  printf ("width_hist:\n");
  for (unsigned i = 1; i <= width_hist.size (); i ++)
    {
      printf (" % 2d: %d\n", i, width_hist[i]);
    }
  fflush (stdout);
  
  unsigned missing = 0,
    compared = 0;
  for (unsigned i = 11; i <= 15; i ++)
    {
      basedvector<basedvector<unsigned, 1>, 1> groups
	= mutant_knot_groups (i);
      
      for (unsigned j = 1; j <= groups.size (); j ++)
	{
	  bool first = 1;
	  
	  knot_desc desc_1;
	  ptr<const module<Z2> > H_1;
	  map<grading, basedvector<int, 1> > st_1;
	  
	  for (unsigned k = 1; k <= groups[j].size (); k ++)
	    {
	      knot_desc desc (knot_desc::HTW, i, groups[j][k]);
	      
	      if (mutant_knot_sq % desc)
		{
		  pair<mod_map<Z2>, mod_map<Z2> > p = mutant_knot_sq(desc);
		  
		  mod_map<Z2> sq1 = p.first,
		    sq2 = p.second;
		  ptr<const module<Z2> > H = sq1.domain ();
		  
		  map<grading, basedvector<int, 1> > st = compute_st (sq1, sq2);
		  
		  if (first)
		    {
		      first = 0;
		      desc_1 = desc;
		      H_1 = H;
		      st_1 = st;
		    }
		  else
		    {
		      if (H_1->free_poincare_polynomial ()
			  == H->free_poincare_polynomial ())
			{
			  if (st_1 != st)
			    {
			      printf ("> DIFFER\n");
			      display ("  ", desc_1);
			      display ("  ", desc);
			      fflush (stdout);
			    }
			  compared ++;
			}
		      else if (mutant_mknot_sq % desc)
			{
			  pair<mod_map<Z2>, mod_map<Z2> > mp = mutant_mknot_sq(desc);
			  
			  mod_map<Z2> msq1 = mp.first,
			    msq2 = mp.second;
			  ptr<const module<Z2> > mH = msq1.domain ();
			  
			  assert (H_1->free_poincare_polynomial ()
				  == mH->free_poincare_polynomial ());
			  
			  map<grading, basedvector<int, 1> > mst
			    = compute_st (msq1, msq2);
			  
			  if (st_1 != mst)
			    {
			      printf ("> DIFFER\n");
			      display ("  ", desc_1);
			      display ("  ", desc);
			      fflush (stdout);
			    }
			  compared ++;
			}
		      else
			missing ++;
		    }
		}
	      else
		missing ++;
	    }
	}
    }
  printf ("missing = %d\n", missing);
  printf ("compared = %d\n", compared);
  fflush (stdout);
}

bool
file_exists (const char *file)
{
  struct stat stat_buf;
  if (stat (file, &stat_buf) != 0)
    {
      if (errno == ENOENT)
	return 0;
      
      stderror ("stat: %s", file);
      exit (EXIT_FAILURE);
    }
  
  return 1;
}

#if 0
bool
file_exists (const char *buf)
{
  struct stat stat_buf;
  if (stat (buf, &stat_buf) != 0)
    {
      if (errno == ENOENT)
	return 0;
      
      stderror ("stat: %s", buf);
      exit (EXIT_FAILURE);
    }
  
  return 1;
}

void
convert_mut15 ()
{
  hashmap<knot_desc,
	  pair<mod_map<Z2>, mod_map<Z2> > > knot15_sq_part;
  
  basedvector<basedvector<unsigned, 1>, 1> groups = mutant_knot_groups (15);
  for (unsigned i = 1; i <= groups.size (); i ++)
    {
      for (unsigned j = 1; j <= groups[i].size (); j ++)
	{
	  knot_desc desc (knot_desc::HTW, i, groups[i][j]);
	  
	  char buf[1000];
	  sprintf (buf, "/u/cseed/mut15.bak/K15_%d.dat.gz", groups[i][j]);
	  
	  if (file_exists (buf))
	    {
	      gzfile_reader r (buf);
	      
	      mod_map<Z2> sq1 (r);
	      mod_map<Z2> sq2 (r);
	      knot15_sq_part.push (desc,
				   pair<mod_map<Z2>, mod_map<Z2> > (sq1, sq2));
	    }
	}
    }
  
  printf ("|knot15_sq_part| = %d\n", knot15_sq_part.card ());
  
  gzfile_writer w ("/u/cseed/src/knotkit/knot_sq/K15_sq_part.dat.gz");
  write (w, knot15_sq_part);
}
#endif

void
compute_mutant_mirrors ()
{
  const char *knot_sq_files[] = {
    // "knot_sq/K11_sq.dat.gz",
    // "knot_sq/K12_sq.dat.gz",
    // "knot_sq/K13_sq.dat.gz",
    "knot_sq/K14_sq.dat.gz",
    0,
  };
  unsigned minn = 14,
    maxn = 14;
  
  hashset<knot_desc> mutants;
  for (unsigned i = minn; i <= maxn; i ++)
    {
      basedvector<basedvector<unsigned, 1>, 1> groups
	= mutant_knot_groups (i);
      for (unsigned j = 1; j <= groups.size (); j ++)
	{
	  for (unsigned k = 1; k <= groups[j].size (); k ++)
	    mutants += knot_desc (knot_desc::HTW, i, groups[j][k]);
	}
    }
  printf ("|mutants| = %d\n", mutants.card ());
  fflush (stdout);
  
  hashmap<knot_desc,
	  pair<mod_map<Z2>, mod_map<Z2> > > mutant_knot_sq;
  
  for (unsigned i = 0; knot_sq_files[i]; i ++)
    {
      gzfile_reader r (knot_sq_files[i]);
      
      printf ("loading %s...\n", knot_sq_files[i]);
      fflush (stdout);
      
      unsigned n = r.read_unsigned ();
      for (unsigned i = 1; i <= n; i ++)
	{
	  knot_desc k (r);
	  
	  pair<mod_map<Z2>, mod_map<Z2> > p (r);
	  if (mutants % k)
	    mutant_knot_sq.push (k, p);
	}
    }
  
  printf ("|mutant_knot_sq| = %d\n", mutant_knot_sq.card ());
  fflush (stdout);
  
  map<knot_desc,
      pair<mod_map<Z2>, mod_map<Z2> > > mknot_kh_sq;
  for (unsigned i = minn; i <= maxn; i ++)
    {
      basedvector<basedvector<unsigned, 1>, 1> groups
	= mutant_knot_groups (i);
      for (unsigned j = 1; j <= groups.size (); j ++)
	{
	  unsigned n = groups[j].size ();
	  
	  basedvector<ptr<const module<Z2> >, 1> group_H (n);
	  for (unsigned k = 1; k <= n; k ++)
	    {
	      knot_desc desc (knot_desc::HTW, i, groups[j][k]);
	      pair<mod_map<Z2>, mod_map<Z2> > p = mutant_knot_sq(desc);
	      group_H[k] = p.first.domain ();
	    }
	  
	  for (unsigned k = 2; k <= n; k ++)
	    {
	      if (group_H[1]->free_poincare_polynomial ()
		  != group_H[k]->free_poincare_polynomial ())
		{
		  knot_desc desc (knot_desc::HTW, i, groups[j][k]);
		  show (desc); newline ();
		  fflush (stdout);
		  
		  char buf[1000];
		  assert (desc.t == knot_desc::HTW);
		  sprintf (buf, "mknot/K%d_%d.dat.gz", desc.i, desc.j);
		  
		  if (!file_exists (buf))
		    {
		      knot_diagram kd (MIRROR, desc.diagram ());
		  
		      cube<Z2> c (kd);
		      mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
		  
		      chain_complex_simplifier<Z2> s (c.khC, d, 1);
		      assert (s.new_d == 0);
		  
		      steenrod_square sq (c, d, s);
		      mod_map<Z2> sq1 = sq.sq1 ();
		      mod_map<Z2> sq2 = sq.sq2 ();
		  
		      ptr<const module<Z2> > H = sq1.domain ();
		  
		      assert (group_H[1]->free_poincare_polynomial ()
			      == H->free_poincare_polynomial ());
		  
		      gzfile_writer w (buf);
		      write (w, desc);
		      write (w, sq1);
		      write (w, sq2);
		    }
		}
	    }
	}
    }
}

void
convert_mknots ()
{
  hashmap<knot_desc,
	  pair<mod_map<Z2>, mod_map<Z2> > > mutant_mknot_sq;
  
  for (unsigned i = 11; i <= 15; i ++)
    {
      basedvector<basedvector<unsigned, 1>, 1> groups
	= mutant_knot_groups (i);
      for (unsigned j = 1; j <= groups.size (); j ++)
	{
	  unsigned n = groups[j].size ();
	  for (unsigned k = 1; k <= n; k ++)
	    {
	      knot_desc desc (knot_desc::HTW, i, groups[j][k]);
	      
	      char buf[1000];
	      assert (desc.t == knot_desc::HTW);
	      sprintf (buf, "mknot/K%d_%d.dat.gz", desc.i, desc.j);
	      
	      struct stat stat_buf;
	      if (stat (buf, &stat_buf) != 0)
		{
		  if (errno != ENOENT)
		    {
		      stderror ("stat: %s", buf);
		      exit (EXIT_FAILURE);
		    }		      
		}
	      else
		{
		  printf ("loading %s...\n", buf);
		  
		  gzfile_reader r (buf);
		  
		  knot_desc desc_2 (r);
		  assert (desc_2 == desc);
		  
		  mod_map<Z2> sq1 (r);
		  mod_map<Z2> sq2 (r);

		  mutant_mknot_sq.push (desc,
					pair<mod_map<Z2>, mod_map<Z2> > (sq1, sq2));
		}
	    }
	}
    }
  
  gzfile_writer w ("knot_sq/mut_mknot.dat.gz");
  write (w, mutant_mknot_sq);
}

void
convert_15 ()
{
  hashmap<knot_desc,
    pair<mod_map<Z2>, mod_map<Z2> > > knot15_sq;
  
  basedvector<basedvector<unsigned, 1>, 1> groups
    = mutant_knot_groups (15);
  for (unsigned i = 1; i <= groups.size (); i ++)
    {
      for (unsigned j = 1; j <= groups[i].size (); j ++)
	{
	  unsigned k = groups[i][j];
	  
	  char buf[1000];
	  sprintf (buf, "/u/cseed/mut15.bak/K15_%d.dat.gz", k);
	  
	  if (file_exists (buf))
	    {
	      printf ("loading %s...\n", buf);
	      fflush (stdout);
	      
	      gzfile_reader r (buf);
	      mod_map<Z2> sq1 (r);
	      mod_map<Z2> sq2 (r);
	      
	      knot_desc desc (knot_desc::HTW, 15, k);
	      knot15_sq.push (desc,
			      pair<mod_map<Z2>, mod_map<Z2> > (sq1, sq2));
	    }
	}
    }
  
  printf ("|knot15_sq| = %d\n", knot15_sq.card ());
  
  gzfile_writer w ("/u/cseed/src/knotkit/K15_sq_part.dat.gz");
  write (w, knot15_sq);
}

void
compare_gss_splitting ()
{
  typedef Z2 R;
  
#if 1
  for (unsigned i = 1; i <= 14; i ++)
    for (unsigned j = 1; j <= mt_links (i, 1); j ++)
      {
	knot_diagram kd (mt_link (i, 1, j));
#endif
	
	unsigned n = kd.num_components ();
	if (n < 2)
	  continue;
	
	show (kd); newline ();
	
	unionfind<1> u (kd.num_edges ());
	for (unsigned i = 1; i <= kd.n_crossings; i ++)
	  {
	    u.join (kd.ept_edge (kd.crossings[i][1]),
		    kd.ept_edge (kd.crossings[i][3]));
	    u.join (kd.ept_edge (kd.crossings[i][2]),
		    kd.ept_edge (kd.crossings[i][4]));
	  }
	
	map<unsigned, unsigned> root_comp;
	unsigned t = 0;
	for (unsigned i = 1; i <= kd.num_edges (); i ++)
	  {
	    if (u.find (i) == i)
	      {
		++ t;
		root_comp.push (i, t);
	      }
	  }
	assert (t == n);
	
	cube<R> c (kd);
#if 1
	for (unsigned i = 0; i < c.n_resolutions; i ++)
	  {
	    smallbitset state (kd.n_crossings, i);
	    smoothing s (kd, state);
	    s.show_self (kd, state);
	    newline ();
	  }
#endif

	printf ("inter-component x:\n");
	mod_map<R> ds (c.khC);
	for (unsigned x = 1; x <= kd.n_crossings; x ++)
	  {
	    unsigned p1 = kd.crossings[x][1],
	      p2 = kd.crossings[x][2];
	    unsigned r1 = u.find (kd.ept_edge (p1)),
	      r2 = u.find (kd.ept_edge (p2));
	    unsigned c1 = root_comp(r1),
	      c2 = root_comp(r2);

	    if (c1 != c2)
	      {
		printf ("  %d\n", x);
		
		ds = ds + c.compute_dinv (x);
	      }
	  }
	
	mod_map<R> d1 = c.compute_d (1, 0, 0, 0, 0);
	assert (d1.compose (ds) == ds.compose (d1));

	mod_map<R> d2 = c.compute_d (2, 0, 0, 0, 0);
	mod_map<R> h2 = d2.compose (ds) + ds.compose (d2);
	if (h2 != 0)
	  display ("h2:\n", h2);
	
	mod_map<R> d3 = c.compute_d (3, 0, 0, 0, 0);
	mod_map<R> h3 = d3.compose (ds) + ds.compose (d3);
	if (h3 != 0)
	  display ("h3:\n", h3);
	
	mod_map<R> d = c.compute_d (0, 0, 0, 0, 0);
	mod_map<R> h = d.compose (ds) + ds.compose (d);
	if (h != 0)
	  display ("h:\n", h);
      }
}

template<class R> unsigned
splitting_bound (knot_diagram &kd,
		 basedvector<R, 1> comp_weight)
{
  sseq ss = compute_forgetfulss<R> (kd, comp_weight);
  
  assert (ss.pages.size () >= 1);
  return ss.pages.size () - 1;
}

basedvector<basedvector<unsigned, 1>, 1>
permutations (basedvector<unsigned, 1> v)
{
  unsigned n = v.size ();
  basedvector<basedvector<unsigned, 1>, 1> ps;
  
  if (n == 1)
    {
      ps.append (v);
      return ps;
    }
  
  for (unsigned i = 1; i <= n; i ++)
    {
      unsigned x = v[i];
      
      basedvector<unsigned, 1> v2 (n - 1);
      for (unsigned j = 1; j < i; j ++)
	v2[j] = v[j];
      for (unsigned j = i + 1; j <= n; j ++)
	v2[j - 1] = v[j];
      
      basedvector<basedvector<unsigned, 1>, 1> ps2 = permutations (v2);
      for (unsigned j = 1; j <= ps2.size (); j ++)
	{
	  basedvector<unsigned, 1> p2 = ps2[j];
	  assert (p2.size () == n - 1);
	  
	  basedvector<unsigned, 1> v3 (n);
	  v3[1] = x;
	  for (unsigned k = 1; k <= n - 1; k ++)
	    v3[k + 1] = p2[k];
	  ps.append (v3);
	}
    }
  
  return ps;
}

basedvector<basedvector<unsigned, 1>, 1>
permutations (unsigned n)
{
  basedvector<unsigned, 1> v (n);
  for (unsigned i = 1; i <= n; i ++)
    v[i] = i;
  return permutations (v);
}

void
compute_splitting_bounds ()
{
  typedef fraction_field<polynomial<Z2> > Z2x;
  
  for (unsigned i = 1; i <= 12; i ++)
    for (unsigned j = 1; j <= mt_links (i); j ++)
      {
	knot_diagram kd (mt_link (i, j));
	unsigned m = kd.num_components ();
	if (m == 1)
	  continue;
	
	show (kd); newline ();
	printf ("  m = %d\n", m);
	
	unionfind<1> u (kd.num_edges ());
	
	for (unsigned i = 1; i <= kd.n_crossings; i ++)
	  {
	    u.join (kd.ept_edge (kd.crossings[i][1]),
		    kd.ept_edge (kd.crossings[i][3]));
	    u.join (kd.ept_edge (kd.crossings[i][2]),
		    kd.ept_edge (kd.crossings[i][4]));
	  }
	assert (u.num_sets () == m);
	
	map<unsigned, unsigned> root_comp;
	unsigned t = 0;
	for (unsigned i = 1; i <= kd.num_edges (); i ++)
	  {
	    if (u.find (i) == i)
	      {
		++ t;
		root_comp.push (i, t);
	      }
	  }
	assert (t == m);
	
	basedvector<Q, 1> comp_weightQ (m);
	for (unsigned i = 1; i <= m; i ++)
	  comp_weightQ[i] = Q (i);
	unsigned bQ = splitting_bound<Q> (kd, comp_weightQ);
	
	basedvector<Z2x, 1> comp_weightZ2x (m);
	for (unsigned i = 1; i <= m; i ++)
	  comp_weightZ2x[i] = Z2x (polynomial<Z2> (Z2 (1), i));
	unsigned bZ2x = splitting_bound<Z2x> (kd, comp_weightZ2x);
	
	// lower bound
	unsigned b = std::max (bQ, bZ2x);
	
	printf ("  bQ = %d\n", bQ);
	printf ("  bZ2x = %d\n", bZ2x);
	printf ("  b = %d\n", b);
	
	unsigned total_lk = kd.total_linking_number ();
	unsigned b_lk_weak = total_lk == 0 ? 2 : total_lk;
	
	printf ("  b_lk_weak = %d\n", b_lk_weak);
	
	basedvector<basedvector<unsigned, 1>, 1> ps = permutations (m);
#if 0
	printf ("ps, |ps| = %d, m = %d:\n", ps.size (), m);
	for (unsigned i = 1; i <= ps.size (); i ++)
	  {
	    basedvector<unsigned, 1> p = ps[i];
	    assert (p.size () == m);
	    
	    printf (" % 3d: ", i);
	    for (unsigned j = 1; j <= m; j ++)
	      printf (" %d", p[j]);
	    newline ();
	  }
#endif
	
	unsigned r = kd.n_crossings;
	for (unsigned i = 1; i <= ps.size (); i ++)
	  {
	    basedvector<unsigned, 1> p = ps[i];
	    
	    unsigned ri = 0;
	    for (unsigned j = 1; j <= kd.n_crossings; j ++)
	      {
		unsigned upper_e = kd.ept_edge (kd.crossings[j][2]),
		  lower_e = kd.ept_edge (kd.crossings[j][1]);
		
		unsigned upper_c = root_comp(u.find (upper_e)),
		  lower_c = root_comp(u.find (lower_e));
		
		if (upper_c != lower_c
		    && p[upper_c] < p[lower_c])
		  ri ++;
	      }
	    
	    if (ri < r)
	      r = ri;
	  }
	printf ("  r = %d\n", r);
	
	assert (b <= r);
	
	// non-trivial link, sp at least 1.
	assert (b_lk_weak >= 1);
	if (b <= 1
	    && b_lk_weak == 1)
	  continue;
	
	if (b > b_lk_weak)
	  {
	    if (b == r)
	      printf ("  > sp = %d (b)\n", b);
	    else
	      printf ("  > %d <= sp <= %d (b)\n", b, r);
	  }
	else if (b == b_lk_weak)
	  {
	    if (b == r)
	      printf ("  > sp = %d (b + b_lk_weak)\n", b);
	    else
	      printf ("  > %d <= sp <= %d (b + b_lk_weak)\n", b, r);
	    
	  }
	else if (b_lk_weak == r)
	  {
	    assert (b < b_lk_weak);
	    
	    printf ("  > sp = %d (b_lk_weak)\n", b_lk_weak);
	  }
      }
}


void
compute_forgetful_tables ()
{
  // typedef fraction_field<multivariate_polynomial<Z2, 10> > R;
  typedef Z2 R;
  
#if 0
  for (unsigned i = 1; i <= 10; i ++)
    for (unsigned j = 1; j <= mt_links (i); j ++)
      {
	knot_diagram kd (mt_link (i, j));
#endif
      {
	// knot_diagram kd (mt_link (12, 0, 2087));
	// knot_diagram kd (mt_link (12, 0, 1705));
	// knot_diagram kd (mt_link (14, 0, 66759));
	// knot_diagram kd (mt_link (14, 0, 65798));
	knot_diagram kd (mt_link (13, 0, 8862));
	
	abort ();
	// sseq ss = compute_forgetfulss<R> (kd);
	sseq ss;
	
#if 0
	if (ss.pages.size () < 2)
	  continue;
#endif
	
	for (unsigned k = 1; k <= ss.pages.size (); k ++)
	  {
	    if (k == 1)
	      printf ("%s &", kd.name.c_str ());
	    else
	      printf (" &");
	    
	    printf (" $E_%d$ & %d & $", k, ss.pages[k].total_rank ());
	    
	    bool first = 1;
	    
	    const sseq_bounds &b = ss.bounds;
	    
	    for (int i = b.minh; i <= b.maxh; i ++)
	      for (int j = b.minq; j <= b.maxq; j ++)
		{
		  unsigned r = ss.pages[k].rank[i - b.minh][j - b.minq];
		  
		  if (r > 0)
		    {
		      
		      if (first)
			first = 0;
		      else
			printf (" + ");
		
		      if (i == 0
			  && j == 0)
			printf ("%d", r);
		      else
			{
			  if (r != 1)
			    printf ("%d", r);
		    
			  if (i != 0)
			    printf ("t^{%d}", i);
			  if (j != 0)
			    printf ("q^{%d}", j);
			}
		    }
		}
	    printf ("$ \\\\\n");
	  }
      }
}

int
main ()
{
  compute_splitting_bounds ();
  return 0;
  
  compute_forgetful_tables ();
  return 0;
  
#if 0
  {
    knot_diagram kd (mt_link (5, 1, 3));
    show (kd); newline ();
    
    sseq ss = compute_forgetfulss<Q> (kd);
    ss.texshow (stdout, "L5a3");
  }
  return 0;
#endif
  
  compute_lee_bound ();
  return 0;
  
  compare_gss_splitting ();
  
  {
    knot_diagram kd (rolfsen_knot (3, 1));
    cube<Q> c (kd);
    mod_map<Q> d = c.compute_d (1, 0, 0, 0, 0);
    chain_complex_simplifier<Q> s (c.khC, d, 0);
    assert (s.new_d == 0);
    display ("s.new_C:\n", *s.new_C);
  }
  return 0;
  
  test_forgetful_signs ();
  
  {
    knot_diagram kd (torus_knot (2, 2));
    printf ("n+ = %d, n- = %d\n", kd.nplus, kd.nminus);
    cube<Z2> c (kd);
    sseq ss = compute_szabo_sseq (c);
    ss.texshow (stdout, "Hopf");
  }
  {
    knot_diagram kd (MIRROR, knot_diagram (torus_knot (2, 2)));
    printf ("n+ = %d, n- = %d\n", kd.nplus, kd.nminus);
    cube<Z2> c (kd);
    sseq ss = compute_szabo_sseq (c);
    ss.texshow (stdout, "Hopf");
  }
  
  return 0;
  
  compute_forgetful_torsion ();
  return 0;
  
  test_forgetful_signs ();
  test_crossing_ss ();
  
  find_width4_in_h0 ();
  // convert_15 ();
  // test_knot_sq ();
  return 0;
  
  // convert_mut15 ();
  
  compute_mutant_mirrors ();
  
  test_kh_htt_stronger ();
  
  compute_mutant_mirrors ();
  
  test_knot_sq ();
  
  convert_mknots ();
  
  test_forgetful_ss ();
  compute_twistedU ();
  
#if 1
  knot_desc desc;
  desc.t = knot_desc::HTW;
  desc.i = 15;
  desc.j = 15020;
  
  char buf[1000];
  sprintf (buf, "/scratch/network/cseed/incoming/K%d_%d.dat.gz",
	   desc.i, desc.j);
  
  desc = knot_desc (knot_desc::ROLFSEN, 3, 1);

  knot_diagram kd = desc.diagram ();

  cube<Z2> c (kd);
  mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
  
  chain_complex_simplifier<Z2> s (c.khC, d, 1);
  assert (s.new_d == 0);
  
  steenrod_square sq (c, d, s);
  mod_map<Z2> sq1 = sq.sq1 ();
  mod_map<Z2> sq2 = sq.sq2 ();
  
  assert (sq1.compose (sq1) == 0);
  assert (sq2.compose (sq2) + sq1.compose (sq2).compose (sq1) == 0);

#if 1
  {
    file_writer w (buf);
    write (w, sq1);
    write (w, sq2);
  }
#endif
#endif
  return 0;
  
  // convert_knot_sq ();
  test_knot_sq ();
  
  // test_sage_show ();
  dump_sage ();
  
#if 0
  knot_diagram kd (rolfsen_knot (8, 19));
  show (kd); newline ();
  
  cube<Z2> c (kd);
  mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
  
  chain_complex_simplifier<Z2> s (c.khC, d, 1);
  
  steenrod_square sq (c, d, s);
  mod_map<Z2> sq1 = sq.sq1 ();
  mod_map<Z2> sq2 = sq.sq2 ();
  
  assert (sq1.compose (sq1) == 0);
  assert (sq2.compose (sq2) + sq1.compose (sq2).compose (sq1) == 0);
  
  display ("sq1:\n", sq1);
  display ("sq2:\n", sq2);
  
  {
    file_writer w ("sqtest.dat");
    write (w, sq1);
    write (w, sq2);
  }
  
  {
    file_reader r ("sqtest.dat");
    mod_map<Z2> sq1p (r);
    mod_map<Z2> sq2p (r);
    display ("sq1p:\n", sq1p);
    display ("sq2p:\n", sq2p);
  }
  
#endif

#if 0
  knot_diagram kd (mt_link (10, 0, 9));
  cube<Z2> c (kd);
#endif
  
#if 0
  for (unsigned i = 1; i <= 10; i ++)
    for (unsigned j = 1; j <= mt_links (i, 0); j ++)
      {
	knot_diagram kd (mt_link (i, 0, j));
	kd.marked_edge = 1;
	
	cube<Z2> c (kd, 1);
	sseq ss = compute_szabo_sseq (c);
	ss.texshow (stdout, kd.name);
      }
#endif
  
#if 0
#if 1
  for (unsigned i = 10; i <= 10; i ++)
    for (unsigned j = 124; j <= rolfsen_crossing_knots (i); j ++)
      {
	knot_diagram kd (rolfsen_knot (i, j));
#endif
#if 0
  for (unsigned i = 1; i <= 10; i ++)
    for (unsigned j = 1; j <= mt_links (i, 0); j ++)
      {
#endif
#if 0
  for (unsigned i = 11; i <= 11; i ++)
    for (unsigned j = 1; j <= htw_knots (i, 0); j ++)
      {
#endif
	// knot_diagram kd (htw_knot (i, 0, j));
	// knot_diagram kd (mt_link (i, 0, j));
	kd.marked_edge = 1;
	
	show (kd); newline ();
  
	cube<Z2> c (kd, 1);
	
#if 0
	mod_map<Z2> d1 = c.compute_d (1, 0, 0, 0, 0);
  
	chain_complex_simplifier<Z2> s (c.khC, d1, 1);
	assert (s.new_d == 0);
#endif
  
	sseq ss = compute_szabo_sseq (c);
  
	multivariate_laurentpoly<Z> Phat = 
	  ss.pages[ss.pages.size ()].delta_poincare_polynomial (ss.bounds);
	
	typedef spanning_tree_complex<Z2>::R R;
	
	spanning_tree_complex<Z2> spanc (kd);
	mod_map<R> d2 = spanc.twisted_d2 ();
	mod_map<R> d2U = spanc.twisted_d2Un (1);
	
	chain_complex_simplifier<R> s2 (spanc.C, d2, 2);
	assert (s2.new_d == 0);
	
	mod_map<R> H_d2U = s2.pi.compose (d2U).compose (s2.iota);
	assert (H_d2U.compose (H_d2U) == 0);
	
	ptr<const module<R> > ker = H_d2U.kernel ();
	ptr<const module<R> > quot = s2.new_C->quotient (H_d2U.image ());
        
        multivariate_laurentpoly<Z> Pminus1
	    = ker->free_delta_poincare_polynomial (),
	  PminusU = quot->free_delta_poincare_polynomial ();
	
	if (PminusU != Pminus1)
	  {
	    display ("  HFhat: ", Phat);
	    // display ("  HF-: ", Pminus);
	    display ("  HF- (1): ", Pminus1);
	    display ("  HF- (U): ", PminusU);
	  }
      }
#endif
  
#if 0
  hwidth_knots = basedvector<unsigned, 1> (10);
  for (unsigned i = 1; i <= hwidth_knots.size (); i ++)
    hwidth_knots[i] = 0;
  
  map<knot_desc,
      pair<mod_map<Z2>, mod_map<Z2> > > knot_kh_sq;
  
  for (unsigned i = 14; i >= 1; i --)
    {
#if 0
      if (i <= 10)
	{
	  for (unsigned j = 1; j <= rolfsen_crossing_knots (i); j += block_size)
	    {
	      load (knot_kh_sq, knot_desc (knot_desc::ROLFSEN, i, j));
	    }
	}
#endif
      
      for (unsigned j = 1; j <= htw_knots (i); j += block_size)
	{
	  load (knot_kh_sq, knot_desc (knot_desc::HTW, i, j));
	}

      if (i <= 13)
	{
	  for (unsigned j = 1; j <= mt_links (i); j += block_size)
	    {
	      load (knot_kh_sq, knot_desc (knot_desc::MT, i, j));
	    }
	}
      
      if (i == 14)
	{
	  for (unsigned j = 1; j <= mt_links (14, 0); j += block_size)
	    {
	      load (knot_kh_sq, knot_desc (knot_desc::MT, 14, mt_links (14, 1) + j));
	    }
	}
    }
  
  unsigned total_knots = 0;
  printf ("hwidth_knots:\n");
  for (unsigned i = 1; i <= hwidth_knots.size (); i ++)
    {
      printf ("  % 2d: %d\n", i, hwidth_knots[i]);
      total_knots += hwidth_knots[i];
    }
  printf ("total_knots = %d\n", total_knots);
  
  printf ("|knot_kh_sq| = %d\n", knot_kh_sq.card ());
  
#if 0
  map<pair<multivariate_laurentpoly<Z>,
	   map<grading, unsigned> >,
      pair<knot_desc,
	   map<grading, basedvector<int, 1> > > > P_sq1_knot_st;
  
  set<multivariate_laurentpoly<Z> > Ps;
  basedvector<unsigned, 1> collisons (10);
  for (unsigned i = 1; i <= 10; i ++)
    collisons[i] = 0;
  
  for (map<knot_desc,
	   pair<mod_map<Z2>, mod_map<Z2> > >::const_iter i = knot_kh_sq; i; i ++)
    {
      show_st (knot_kh_sq, i.key ());
      
      mod_map<Z2> sq1 = i.val ().first;
      mod_map<Z2> sq2 = i.val ().second;
      
#if 0
      display ("sq1:\n", sq1);
      display ("sq2:\n", sq2);
#endif
      
      printf ("%s  ", i.key ().name ().c_str ());
      
      assert (sq1.compose (sq1) == 0);
      assert (sq2.compose (sq2) + sq1.compose (sq2).compose (sq1) == 0);
      
      ptr<const module<Z2> > H = sq1.domain ();
      unsigned hwidth = homological_width (H);
      
      map<grading, basedvector<int, 1> > st;
      map<grading, unsigned> sq1_ranks;
      
      bool first = 1;
      
      set<grading> gs = H->gradings ();
      
      for (set_const_iter<grading> gg = gs; gg; gg ++)
	{
	  grading hq = gg.val (),
	    h1q (hq.h + 1, hq.q),
	    h2q (hq.h + 2, hq.q),
	    h3q (hq.h + 3, hq.q);
	  
	  // printf ("(%d, %d):\n", hq.h, hq.q);
	  
	  ptr<const free_submodule<Z2> > H_hq = H->graded_piece (hq),
	    H_h1q = H->graded_piece (h1q),
	    H_h2q = H->graded_piece (h2q),
	    H_h3q = H->graded_piece (h3q);
	  
	  mod_map<Z2> S = sq2.restrict (H_hq, H_h2q),
	    T = sq2.restrict (H_h1q, H_h3q),
	    A = sq1.restrict (H_hq, H_h1q),
	    B = sq1.restrict (H_h1q, H_h2q),
	    C = sq1.restrict (H_h2q, H_h3q);
	  
	  ptr<const free_submodule<Z2> > Sker = S.kernel (),
	    Sim = S.image (),
	    Tker = T.kernel (),
	    Tim = T.image (),
	    Aker = A.kernel (),
	    Aim = A.image (),
	    Bker = B.kernel (),
	    Bim = B.image (),
	    Cker = C.kernel (),
	    Cim = C.image ();
	  
	  sq1_ranks.push (hq, Aim->dim ());
	  
	  mod_map<Z2> ArSker = A.restrict_from (Sker);
	  mod_map<Z2> SrAker = S.restrict_from (Aker);
	  
	  mod_map<Z2> TrAim = T.restrict_from (Aim);
	  mod_map<Z2> TrBker = T.restrict_from (Bker);
	  mod_map<Z2> BrTker = B.restrict_from (Tker);
	  
	  mod_map<Z2> CrSim = C.restrict_from (Sim);
	  
	  ptr<const free_submodule<Z2> > ArSker_im = ArSker.image ();
	  ptr<const free_submodule<Z2> > SrAker_im = SrAker.image ();
	  
	  ptr<const free_submodule<Z2> > TrAim_im = TrAim.image ();
	  ptr<const free_submodule<Z2> > TrBker_im = TrBker.image ();
	  ptr<const free_submodule<Z2> > BrTker_im = BrTker.image ();
	  
	  ptr<const free_submodule<Z2> > CrSim_im = CrSim.image ();
	  
	  mod_map<Z2> CrSrAker_im = C.restrict_from (SrAker_im);
	  mod_map<Z2> TrArSker_im = T.restrict_from (ArSker_im);
	  
	  ptr<const free_submodule<Z2> > CrSrAker_im_im = CrSrAker_im.image ();
	  ptr<const free_submodule<Z2> > TrArSker_im_im = TrArSker_im.image ();
	  
	  ptr<const free_submodule<Z2> > Aker_cap_Sker = Aker->intersection (Sker);
	  
	  ptr<const free_submodule<Z2> > Aim_cap_Tker = Aim->intersection (Tker);
	  ptr<const free_submodule<Z2> > Bker_cap_Tker = Bker->intersection (Tker);
	  ptr<const free_submodule<Z2> > ArSker_im_cap_Tker = ArSker_im->intersection (Tker);
	  
	  ptr<const free_submodule<Z2> > Sim_cap_Bim = Sim->intersection (Bim);
	  ptr<const free_submodule<Z2> > Sim_cap_BrTker_im = Sim->intersection (BrTker_im);
	  ptr<const free_submodule<Z2> > Sim_cap_Cker = Sim->intersection (Cker);
	  ptr<const free_submodule<Z2> > SrAker_im_cap_Bim = SrAker_im->intersection (Bim);
	  ptr<const free_submodule<Z2> > SrAker_im_cap_BrTker_im = SrAker_im->intersection (BrTker_im);
	  ptr<const free_submodule<Z2> > SrAker_im_cap_Cker = SrAker_im->intersection (Cker);
	  
	  ptr<const free_submodule<Z2> > Tim_cap_Cim = Tim->intersection (Cim);
	  ptr<const free_submodule<Z2> > TrAim_im_cap_Cim = TrAim_im->intersection (Cim);
	  ptr<const free_submodule<Z2> > TrBker_im_cap_Cim = TrBker_im->intersection (Cim);
	  ptr<const free_submodule<Z2> > TrArSker_im_im_cap_Cim = TrBker_im->intersection (Cim);
	  
	  ptr<const free_submodule<Z2> > Tim_cap_CrSim_im = Tim->intersection (CrSim_im);
	  ptr<const free_submodule<Z2> > TrAim_im_cap_CrSim_im = TrAim_im->intersection (CrSim_im);
	  ptr<const free_submodule<Z2> > TrBker_im_cap_CrSim_im = TrBker_im->intersection (CrSim_im);
	  ptr<const free_submodule<Z2> > TrArSker_im_im_cap_CrSim_im = TrBker_im->intersection (CrSim_im);

	  ptr<const free_submodule<Z2> > Tim_cap_CrSrAker_im_im = Tim->intersection (CrSrAker_im_im);
	  ptr<const free_submodule<Z2> > TrAim_im_cap_CrSrAker_im_im = TrAim_im->intersection (CrSrAker_im_im);
	  ptr<const free_submodule<Z2> > TrBker_im_cap_CrSrAker_im_im = TrBker_im->intersection (CrSrAker_im_im);
	  ptr<const free_submodule<Z2> > TrArSker_im_im_cap_CrSrAker_im_im = TrBker_im->intersection (CrSrAker_im_im);
	  
	  basedvector<int, 1> v;
	  
	  v.append (Sker->dim ());
	  v.append (Sim->dim ());
	  v.append (Tker->dim ());
	  v.append (Tim->dim ());
	  
	  v.append (Aker->dim ());
	  v.append (Aim->dim ());
	  v.append (Bker->dim ());
	  v.append (Bim->dim ());
	  v.append (Cker->dim ());
	  v.append (Cim->dim ());
	  
	  v.append (ArSker_im->dim ());
	  v.append (SrAker_im->dim ());
	  
	  v.append (TrAim_im->dim ());
	  v.append (TrBker_im->dim ());
	  v.append (BrTker_im->dim ());
	  
	  v.append (CrSim_im->dim ());
	  
	  v.append (CrSrAker_im_im->dim ());
	  v.append (TrArSker_im_im->dim ());
	  
	  v.append (Aker_cap_Sker->dim ());
	  
	  v.append (Aim_cap_Tker->dim ());
	  v.append (Bker_cap_Tker->dim ());
	  v.append (ArSker_im_cap_Tker->dim ());
	  
	  v.append (Sim_cap_Bim->dim ());
	  v.append (Sim_cap_BrTker_im->dim ());
	  v.append (Sim_cap_Cker->dim ());
	  v.append (SrAker_im_cap_Bim->dim ());
	  v.append (SrAker_im_cap_BrTker_im->dim ());
	  v.append (SrAker_im_cap_Cker->dim ());
	  
	  v.append (Tim_cap_Cim->dim ());
	  v.append (TrAim_im_cap_Cim->dim ());
	  v.append (TrBker_im_cap_Cim->dim ());
	  v.append (TrArSker_im_im_cap_Cim->dim ());
	  
	  v.append (Tim_cap_CrSim_im->dim ());
	  v.append (TrAim_im_cap_CrSim_im->dim ());
	  v.append (TrBker_im_cap_CrSim_im->dim ());
	  v.append (TrArSker_im_im_cap_CrSim_im->dim ());

	  v.append (Tim_cap_CrSim_im->dim ());
	  v.append (TrAim_im_cap_CrSim_im->dim ());
	  v.append (TrBker_im_cap_CrSim_im->dim ());
	  v.append (TrArSker_im_im_cap_CrSim_im->dim ());
	  
	  st.push (hq, v);
	}
      newline ();
      
      multivariate_laurentpoly<Z> P = H->free_poincare_polynomial ();
      pair<pair<knot_desc,
		map<grading, basedvector<int, 1> > > &,
	   bool> p = P_sq1_knot_st.find (pair<multivariate_laurentpoly<Z>,
					      map<grading, unsigned> > (P, sq1_ranks));
      if (p.second)
	{
	  collisons[hwidth] ++;
	  Ps += P;
	  
	  if (p.first.second != st)
	    {
	      printf ("DIFFER:\n");
	      printf ("hwidth = %d\n", hwidth);
	      
	      show_st (knot_kh_sq, p.first.first);
	      show_st (knot_kh_sq, i.key ());

	      printf ("Kh[");
	      planar_diagram (p.first.first.diagram ()).show_knottheory ();
	      printf (", Modulus -> Null][q,t] === Kh[");
	      planar_diagram (i.key ().diagram ()).show_knottheory ();
	      printf (", Modulus -> Null][q,t]\n");
	      
#if 0
	      printf ("%s:\n", 
		      p.first.first.name ().c_str ());
	      for (map<grading, basedvector<int, 1> >::const_iter j = p.first.second; j; j ++)
		{
		  printf ("  (%d, %d) -> [",
			  j.key ().h, j.key ().q);
		  for (unsigned k = 1; k <= j.val ().size (); k ++)
		    {
		      if (k > 1)
			printf (",");
		      printf ("%d", j.val ()[k]);
		    }
		  newline ();
		}
	      printf ("%s:\n",
		      i.key ().name ().c_str ());
	      for (map<grading, basedvector<int, 1> >::const_iter j = st; j; j ++)
		{
		  printf ("  (%d, %d) -> [",
			  j.key ().h, j.key ().q);
		  for (unsigned k = 1; k <= j.val ().size (); k ++)
		    {
		      if (k > 1)
			printf (",");
		      printf ("%d", j.val ()[k]);
		    }
		  newline ();
		}
#endif
	    }
	}
      else
	{
	  p.first.first = i.key ();
	  p.first.second = st;
	}
    }
  
  printf ("groups = %d\n", Ps.card ());
  printf ("collisons:\n");
  for (unsigned i = 1; i <= 10; i ++)
    printf ("  % 2d: %d\n", i, collisons[i]);
  
#endif
#endif

#if 0
#if 0
  reader r ("sqtest.dat");
  
  knot_desc kd (knot_desc::ROLFSEN, 8, 19);
  mod_map<Z2> sq1 (r);
  mod_map<Z2> sq2 (r);
  
  ptr<const module<Z2> > H = sq1.domain ();
  
  display ("sq1:\n", sq1);
  display ("sq2:\n", sq2);
#endif
  
  for (unsigned i = 1; i <= 10; i ++)
    for (unsigned j = 1; j <= rolfsen_crossing_knots (i);  j ++)
      {
#if 0
	// (Knot Atlas) L10n18 
	int xings[10][4] = {
	  { 6, 1, 7, 2 },
	  { 18, 7, 19, 8 },
	  { 4,19,1,20 },
	  { 5,12,6,13 },
	  { 8,4,9,3 },
	  { 13,17,14,16 },
	  { 9,15,10,14 },
	  { 15,11,16,10 },
	  { 11,20,12,5 },
	  { 2,18,3,17 },
	};
	knot_diagram kd (planar_diagram ("L10n18", 10, xings));
#endif
	
	// (Knot Atlas) L10n102
	int xings[10][4] = {
	  { 6,1,7,2 }, { 10,3,11,4 }, { 14,7,15,8 }, { 8,13,5,14 }, { 11,18,12,19 }, { 15,20,16,17 }, { 19,16,20,9 }, { 17,12,18,13 }, { 2,5,3,6 }, { 4,9,1,10 },
	};
	knot_diagram kd (planar_diagram ("L10n102", 10, xings));
	
	// knot_diagram kd (rolfsen_knot (i, j));
	// knot_diagram kd (mt_link (10, 0, 18));
	
	// show (kd); newline ();
	printf ("%s  ", kd.name.c_str ());
	
	cube<Z2> c (kd);
	mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
	
	chain_complex_simplifier<Z2> s (c.khC, d, 1);
	
	steenrod_square sq (c, d, s);

	mod_map<Z2> sq1 = sq.sq1 ();
	mod_map<Z2> sq2 = sq.sq2 ();
	
	assert (sq1.compose (sq1) == 0);
	assert (sq2.compose (sq2) + sq1.compose (sq2).compose (sq1) == 0);
	
	ptr<const module<Z2> > H = sq1.domain ();
	
	bool first = 1;
  
	// ???
	set<grading> gs = H->gradings ();
	for (set_const_iter<grading> i = gs; i; i ++)
	  {
	    grading hq = i.val (),
	      h1q (hq.h + 1, hq.q),
	      h2q (hq.h + 2, hq.q);
      
	    // printf ("(%d, %d):\n", hq.h, hq.q);
      
	    ptr<const free_submodule<Z2> > H_hq = H->graded_piece (hq),
	      H_h1q = H->graded_piece (h1q),
	      H_h2q = H->graded_piece (h2q);
      
	    mod_map<Z2> whole = sq2.restrict (H_hq, H_h2q),
	      tail = sq1.restrict (H_hq, H_h1q),
	      head = sq1.restrict (H_h1q, H_h2q);
      
	    ptr<const free_submodule<Z2> > whole_im = whole.image (),
	      tail_ker = tail.kernel (),
	      head_im = head.image ();
	    ptr<const free_submodule<Z2> > inter = whole_im->intersection (head_im);
      
	    mod_map<Z2> whole_res = whole.restrict_from (tail_ker);
	    ptr<const free_submodule<Z2> > whole_res_im = whole_res.image ();
      
	    ptr<const free_submodule<Z2> > res_inter = whole_res_im->intersection (head_im);
      
	    int r1 = whole_im->dim ();
	    int r2 = whole_res_im->dim ();
	    int r3 = inter->dim ();
	    int r4 = res_inter->dim ();
      
	    if (r1 == 0
		&& r2 == 0
		&& r3 == 0
		&& r4 == 0)
	      continue;
      
	    // printf ("  r = (%d, %d, %d, %d)\n", r1, r2, r3, r4);
      
	    int s1 = r2 - r4,
	      s2 = r1 - r2 - r3 + r4,
	      s3 = r4,
	      s4 = r3 - r4;

	    if (first)
	      {
#if 0
		show (kd);
		printf (":  ");
#endif
		first = 0;
	      }
	    else
	      printf (", ");
	    printf ("(%d, %d) -> (%d, %d, %d, %d)",
		    hq.h, hq.q,
		    s1, s2, s3, s4);
	  }
	// if (!first)
	  newline ();
      }
#endif

#if 0
  // knot_diagram kd (htw_knot (12, 0, 48));
  knot_diagram kd (htw_knot (10, 1, 23));
  show (kd); newline ();
#endif
  
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
#endif
  
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
  
#if 0
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
