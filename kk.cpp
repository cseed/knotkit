
#include <knotkit.h>

const char *program_name;

void
usage ()
{
  printf ("usage: %s <invariant> [options...] <link>\n", program_name);
  printf ("  compute <invariant> for knot or link <link>\n");
  printf ("<invariant> can be one of:\n");
  printf ("  kh: Khovanov homology\n");
  printf ("  gss: Szabo's geometric spectral sequence\n");
  printf ("  ls: Batson-Seed link splitting spectral sequence\n");
  printf ("    component weights are 0, 1, ..., m\n");
  printf ("  sq2: Lipshitz-Sarkar Steenrod square on Z/2 Kh\n");
  printf ("    output suitable for Sage\n");
  printf ("  leess: spectral sequence coming from Bar-Natan analogue of Lee's\n");
  printf ("    deformation of Khovanov's complex (whew!)\n");
  printf ("  s: Rasmussen's s-invariant coming from lee\n");
  printf ("options:\n");
  printf ("  -r         : compute reduced theory\n");
  printf ("  -h         : print this message\n");
  printf ("  -o <file>  : write output to <file>\n");
  printf ("                (stdout is the default)\n");
  printf ("  -f <field> : ground field (if applicable)\n");
  printf ("                (Z2 is the default)\n");
  printf ("  -v         : verbose: report progress as the computation proceeds\n");
  printf ("<field> can be one of:\n");
  printf ("  Z2, Z3, Q\n");
  printf ("<link> can be one of:\n");
  printf ("  - the unknot, e.g. U or unknot\n");
  printf ("  - a torus knot, e.g. T(2,3)\n");
  printf ("  - a Rolfsen table knot, e.g. 10_124\n");
  printf ("  - a Hoste-Thistlethwaite-Weeks knot, e.g. 11a12 or 12n214\n");
  printf ("  - a Morwen Thistlethwaite link, e.g. L9a21 or L14n7631\n");
  printf ("  - a planar diagram, e.g.\n");
  printf ("      PD[X[1, 4, 2, 5], X[3, 6, 4, 1], X[5, 2, 6, 3]] or\n");
  printf ("      PD[[1, 4, 2, 5], [3, 6, 4, 1], [5, 2, 6, 3]]\n");
  printf ("  - a Dowker-Thistlethwaite code, e.g.\n");
  printf ("      DTCode[6,8,2,4],\n");
  printf ("      DT[dadbcda] or\n");
  printf ("      DT[{6, -8}, {-10, 12, -14, 2, -4}]\n");
  printf ("  - a braid, e.g. BR[2, {-1, -1, -1}]\n");
}

FILE *outfp = stdout;

void tex_header ()
{
  fprintf (outfp, "\\documentclass{article}\n\
\\usepackage{amsmath, tikz, hyperref}\n\
\\DeclareMathOperator{\\rank}{rank}\n\
\\setlength{\\parindent}{0pt}\n\
\n\
\\begin{document}\n\
\\pagestyle{empty}\n\
\\sloppy\n");
}

void tex_footer ()
{
  fprintf (outfp, "\\end{document}\n");
}

const char *knot = 0;
const char *invariant = 0;
const char *field = "Z2";

knot_diagram kd;

bool reduced = 0;

class hg_grading_mapper
{
  unsigned m;
  
public:
  hg_grading_mapper (unsigned m_) : m(m_) { }
  
  grading operator () (grading hq) const
  {
    int t = hq.q - (int)m;
    if (reduced)
      t --;
    
    assert (is_even (t));
    return grading (hq.h, t / 2);
  }
  
  grading map_delta (grading d_hq) const
  {
    assert (is_even (d_hq.q));
    return grading (d_hq.h, d_hq.q / 2);
  }
  
  void x_label (FILE *fp, int h) const
  {
    fprintf (fp, "%d", h);
  }
  void y_label (FILE *fp, int g) const
  {
    unsigned q = 2*g + (int)m;
    if (reduced)
      q ++;
    
    fprintf (fp, "%d", q);
  }
};

template<class R> mod_map<R>
compute_link_splitting_d (knot_diagram &kd,
			  cube<R> &c,
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
		
	  d = d + c.compute_dinv (x)*(s*(w_over - w_under));
	}
    }
  
  assert (d.compose (d) == 0);
  return d;
}

void
compute_gss ()
{
  cube<Z2> c (kd, reduced);
  ptr<const module<Z2> > C = c.khC;
  mod_map<Z2> d = c.compute_d (0, 0, 0, 0, 0);
  
  unsigned m = kd.num_components ();
  hg_grading_mapper mapper (m);
  
  sseq_bounds b (C, mapper);
  basedvector<sseq_page, 1> pages;
  
  unsigned k = 1;
  for (;;)
    {
      chain_complex_simplifier<Z2> s (C, d,
				      maybe<int> (k), maybe<int> (2*k - 2));
      C = s.new_C;
      d = s.new_d;
      k ++;
      
      grading dk_gr (k, 2*k - 2);
      pages.append (sseq_page (b, k, dk_gr, d.graded_piece (dk_gr), mapper));
      if (d == 0)
	break;
    }
  
  sseq ss (b, pages);
  
  tex_header ();
  fprintf (outfp, "$E_k = %s^{Sz}_k(\\verb~%s~; \\verb~%s~)$:\\\\\n",
	       (reduced
		? "\\widetilde{E}"
		: "E"),
	   knot, field);
  ss.texshow (outfp, mapper);
  tex_footer ();
}

template<class R> void
compute_invariant ()
{
  if (!strcmp (invariant, "kh"))
    {
      cube<R> c (kd, reduced);
      ptr<const module<R> > C = c.khC;
      mod_map<R> d = c.compute_d (1, 0, 0, 0, 0);
      
      unsigned m = kd.num_components ();
      hg_grading_mapper mapper (m);
      
      chain_complex_simplifier<R> s (C, d,
				     maybe<int> (1), maybe<int> (0));
      C = s.new_C;
      d = s.new_d;
      
      sseq_bounds b (C, mapper);
      sseq_page pg (b, 2, grading (0, 0), mod_map<R> (C), mapper);
      
      tex_header ();
      
      fprintf (outfp, "Kh = $%s(\\verb~%s~; \\verb~%s~)$:\\\\\n",
	       (reduced
		? "\\widetilde{Kh}"
		: "Kh"),
	       knot,
	       field);
      fprintf (outfp, "$\\rank Kh = %d$\\\\\n", C->dim ());
      
      char buf[1000];
      sprintf (buf, "$Kh$");
      pg.texshow (outfp, b, buf, mapper);
      
      tex_footer ();
    }
  else if (!strcmp (invariant, "lsss"))
    {
      cube<R> c (kd, reduced);
      ptr<const module<R> > C = c.khC;
      
      unsigned m = kd.num_components ();
      basedvector<R, 1> comp_weight (m);
      for (unsigned i = 1; i <= m; i ++)
	comp_weight[i] = R ((int)(i - 1));
      
      mod_map<R> d = compute_link_splitting_d (kd, c, comp_weight);
      
      hg_grading_mapper mapper (m);
      
      sseq_bounds b (C, mapper);
      basedvector<sseq_page, 1> pages;
      
      int k = 0;
      for (;;)
	{
	  chain_complex_simplifier<R> s (C, d,
					 maybe<int> (1 - 2*k), maybe<int> (-2*k));
	  C = s.new_C;
	  d = s.new_d;
	  k ++;
	  
	  grading dk_gr (1 - 2*k, -2*k);
	  pages.append (sseq_page (b, k, dk_gr, d.graded_piece (dk_gr), mapper));
	  if (d == 0)
	    break;
	}
      
      sseq ss (b, pages);
      
      tex_header ();
      fprintf (outfp, "$E_k = %s^{BS}_k(\\verb~%s~; \\verb~%s~)$:\\\\\n",
	       (reduced
		? "\\widetilde{E}"
		: "E"),
	       knot,
	       field);
      ss.texshow (outfp, mapper);
      tex_footer ();
    }
  else if (!strcmp (invariant, "leess"))
    {
      cube<R> c (kd, reduced);
      ptr<const module<R> > C = c.khC;
      
      mod_map<R> d = c.compute_d (1, 0, 0, 0, 0);
      for (unsigned i = 1; i <= kd.n_crossings; i ++)
	d = d + c.H_i (i);
      assert (d.compose (d) == 0);
      
      unsigned m = kd.num_components ();
      hg_grading_mapper mapper (m);
      
      sseq_bounds b (C, mapper);
      basedvector<sseq_page, 1> pages;
      
      int k = 0;
      for (;;)
	{
	  chain_complex_simplifier<R> s (C, d,
					 maybe<int> (1), maybe<int> (2*k));
	  C = s.new_C;
	  d = s.new_d;
	  k ++;
	  
	  grading dk_gr (1, 2*k);
	  pages.append (sseq_page (b, k, dk_gr, d.graded_piece (dk_gr), mapper));
	  if (d == 0)
	    break;
	}
      
      sseq ss (b, pages);
      
      tex_header ();
      fprintf (outfp, "$E_k = %s^{BN}_k(\\verb~%s~; \\verb~%s~)$:\\\\\n",
	       (reduced
		? "\\widetilde{E}"
		: "E"),
	       knot, field);
      ss.texshow (outfp, mapper);
      tex_footer ();
    }
  else if (!strcmp (invariant, "s"))
    {
      unsigned m = kd.num_components ();
      if (m != 1)
	{
	  fprintf (stderr, "error: s-invariant only defined for knots\n");
	  exit (EXIT_FAILURE);
	}
      
      cube<R> c (kd, 0);
      ptr<const module<R> > C = c.khC;
      
      mod_map<R> d = c.compute_d (1, 0, 0, 0, 0);
      for (unsigned i = 1; i <= kd.n_crossings; i ++)
	d = d + c.H_i (i);
      assert (d.compose (d) == 0);
      
      int k = 0;
      for (;;)
	{
	  chain_complex_simplifier<R> s (C, d,
					 maybe<int> (1), maybe<int> (2*k));
	  C = s.new_C;
	  d = s.new_d;
	  k ++;
	  
	  if (d == 0)
	    break;
	}
      
      assert (C->dim () == 2);
      grading gr1 = C->generator_grading (1),
	gr2 = C->generator_grading (2);
      
      int qmin = gr1.q,
	qmax = gr2.q;
      if (qmax < qmin)
	std::swap (qmin, qmax);
      
      assert (qmax == qmin + 2);
      
      fprintf (outfp, "s(%s; %s) = %d\n", knot, field, qmin + 1);
    }
  else 
    {
      fprintf (stderr, "error: unknown invariant %s\n", invariant);
      exit (EXIT_FAILURE);
    }
}

void
sage_show (FILE *fp,
	   map<grading, basedvector<unsigned, 1> > hq_gens,
	   ptr<const module<Z2> > H)
{
  fprintf (fp, "hom={");
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
	   map<grading, basedvector<unsigned, 1> > hq_gens,
	   std::string name,
	   grading delta,
	   mod_map<Z2> f)
{
  fprintf (fp, "%s={", name.c_str ());
  
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
  
  sage_show (fp, hq_gens, H);
  sage_show (fp, hq_gens, "sq1", grading (1, 0), sq1);
  sage_show (fp, hq_gens, "sq2", grading (2, 0), sq2);
}

void
compute_sq2 ()
{
  cube<Z2> c (kd);
  mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
  
  chain_complex_simplifier<Z2> s (c.khC, d, maybe<int> (1), maybe<int> (0));
  assert (s.new_d == 0);
  
  steenrod_square sq (c, d, s);
  mod_map<Z2> sq1 = sq.sq1 ();
  mod_map<Z2> sq2 = sq.sq2 ();
  
  ptr<const module<Z2> > H = sq1.domain ();
  
  sage_show_khsq (outfp, H, sq1, sq2);
}

int
main (int argc, char **argv)
{
  program_name = argv[0];
  
  const char *file = 0;
  
  for (int i = 1; i < argc; i ++)
    {
      if (argv[i][0] == '-')
	{
	  if (strcmp (argv[i], "-r") == 0)
	    reduced = 1;
	  else if (strcmp (argv[i], "-h") == 0)
	    {
	      usage ();
	      exit (EXIT_SUCCESS);
	    }
	  else if (!strcmp (argv[i], "-v"))
	    verbose = 1;
	  else if (!strcmp (argv[i], "-f"))
	    {
	      i ++;
	      if (i == argc)
		{
		  fprintf (stderr, "error: missing argument to option `-f'\n");
		  exit (EXIT_FAILURE);
		}
	      field = argv[i];
	    }
	  else if (!strcmp (argv[i], "-o"))
	    {
	      i ++;
	      if (i == argc)
		{
		  fprintf (stderr, "error: missing argument to option `-o'\n");
		  exit (EXIT_FAILURE);
		}
	      file = argv[i];
	    }
	  else
	    {
	      fprintf (stderr, "error: unknown argument `%s'\n", argv[1]);
	      fprintf (stderr, "  use -h for usage\n");
	      exit (EXIT_FAILURE);
	    }
	}
      else
	{
	  if (knot)
	    {
	      fprintf (stderr, "error: too many arguments\n");
	      fprintf (stderr, "  use -h for usage\n");
	      exit (EXIT_FAILURE);
	    }
	  else if (invariant)
	    knot = argv[i];
	  else
	    {
	      assert (invariant == 0);
	      invariant = argv[i];
	    }
	}
    }
  
  if (!knot)
    {
      fprintf (stderr, "error: too few arguments, <invariant> or <knot> missing\n");
      fprintf (stderr, "  use -h for usage\n");
      exit (EXIT_FAILURE);
    }
  
  if (file)
    {
      outfp = fopen (file, "w");
      if (!outfp)
	{
	  stderror ("fopen: %s", file);
	  exit (EXIT_FAILURE);
	}
    }
  else
    outfp = stdout;
  
  kd = parse_knot (knot);
  kd.marked_edge = 1;
  
  if (!strcmp (invariant, "sq2"))
    {
      if (strcmp (field, "Z2"))
	{
	  fprintf (stderr, "warning: sq2 only defined over Z2, ignoring -f %s\n", field);
	  field = "Z2";
	}
      
      compute_sq2 ();
    }
  else if (!strcmp (invariant, "gss"))
    {
      if (strcmp (field, "Z2"))
	{
	  fprintf (stderr, "warning: gss only defined over Z2, ignoring -f %s\n", field);
	  field = "Z2";
	}
      
      compute_gss ();
    }
  else
    {
      if (!strcmp (field, "Z2"))
	compute_invariant<Z2> ();
      else if (!strcmp (field, "Z3"))
	compute_invariant<Zp<3> > ();
      else if (!strcmp (field, "Q"))
	compute_invariant<Q> ();
      else
	{
	  fprintf (stderr, "error: unknown field %s\n", field);
	  exit (EXIT_FAILURE);
	}
    }
  
  if (file)
    fclose (outfp);
}
