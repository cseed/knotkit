
#include <knotkit.h>

sseq_page::sseq_page (const sseq_bounds &b)
  : rank(b.width ()),
    im_rank(b.width ())
{
  for (unsigned i = 0; i < b.width (); i ++)
    {
      vector<unsigned> r (b.height ()),
	im_r (b.height ());
      for (unsigned j = 0; j < b.height (); j ++)
	r[j] = im_r[j] = 0;
      
      rank[i] = r;
      im_rank[i] = im_r;
    }
}

void
sseq_page::addeq (const sseq_bounds &b, const sseq_bounds &b2, const sseq_page &pg2)
{
  for (unsigned i = 0; i < b2.width (); i ++)
    {
      for (unsigned j = 0; j < b2.height (); j ++)
	{
	  rank[i + b2.minh - b.minh][j + b2.minq - b.minq]
	    += pg2.rank[i][j];
	  im_rank[i + b2.minh - b.minh][j + b2.minq - b.minq]
	    += pg2.im_rank[i][j];
	}
    }
}

void
sseq_page::otimes_addeq (const sseq_bounds &b,
			 const sseq_bounds &b1, const sseq_page &pg1,
			 const sseq_bounds &b2, const sseq_page &pg2)
{
  for (int i1 = b1.minh; i1 <= b1.maxh; i1 ++)
    for (int j1 = b1.minq; j1 <= b1.maxq; j1 ++)
      {
	for (int i2 = b2.minh; i2 <= b2.maxh; i2 ++)
	  for (int j2 = b2.minq; j2 <= b2.maxq; j2 ++)
	    {
	      unsigned n1 = pg1.rank[i1 - b1.minh][j1 - b1.minq],
		n2 = pg2.rank[i2 - b2.minh][j2 - b2.minq];
	      unsigned k1 = pg1.im_rank[i1 - b1.minh][j1 - b1.minq],
		k2 = pg2.im_rank[i2 - b2.minh][j2 - b2.minq];
	      
	      rank[i1 + i2 - b.minh][j1 + j2 + 1 - b.minq]
		+= n1 * n2;
	      im_rank[i1 + i2 - b.minh][j1 + j2 + 1 - b.minq]
		+= n1*k2 + n2*k1 - k1*k2;
	    }
      }
}

unsigned
sseq_page::total_rank () const
{
  unsigned r = 0;
  for (unsigned i = 0; i < rank.size (); i ++)
    {
      for (unsigned j = 0; j < rank[i].size (); j ++)
	r += rank[i][j];
    }
  return r;
}

unsigned
sseq_page::homological_width (const sseq_bounds &b) const
{
  int mind = 0, maxd = 0;
  bool first = 1;
  
  for (int i = b.minh; i <= b.maxh; i ++)
    for (int j = b.minq; j <= b.maxq; j ++)
      {
	if (rank[i - b.minh][j - b.minq] > 0)
	  {
	    int d = j - 2*i;
	    if (first)
	      {
		first = 0;
		mind = maxd = d;
	      }
	    else
	      {
		if (d < mind)
		  mind = d;
		if (d > maxd)
		  maxd = d;
	      }
	  }
      }
  if (first)
    return 0;
  else
    {
      assert (maxd >= mind);
      return maxd - mind + 1;
    }
}

multivariate_laurentpoly<Z>
sseq_page::poincare_polynomial (const sseq_bounds &b) const
{
  multivariate_laurentpoly<Z> r;
  for (int i = b.minh; i <= b.maxh; i ++)
    {
      for (int j = b.minq; j <= b.maxq; j ++)
	{
	  unsigned c = rank[i - b.minh][j - b.minq];
	  if (c)
	    {
	      multivariate_laurent_monomial m;
	      m.push_exponent (1, i);
	      m.push_exponent (2, j);
	      r.muladdeq (c, m);
	    }
	}
    }
  return r;
}

multivariate_laurentpoly<Z>
sseq_page::delta_poincare_polynomial (const sseq_bounds &b) const
{
  multivariate_laurentpoly<Z> r;
  for (int i = b.minh; i <= b.maxh; i ++)
    {
      for (int j = b.minq; j <= b.maxq; j ++)
	{
	  unsigned c = rank[i - b.minh][j - b.minq];
	  if (c)
	    r.muladdeq (c, multivariate_laurent_monomial (VARIABLE, 1, j - 2*i));
	}
    }
  return r;
}

void
sseq_page::texshow (FILE *fp, const sseq_bounds &b, unsigned dh, bool last)
{
  fprintf (fp, "\
\\begin{tikzpicture}[scale=.66]\n\
  \\draw[->] (0,0) -- (%d.5,0) node[right] {$t$};\n\
  \\draw[->] (0,0) -- (0,%d.5) node[above] {$q$};\n\
  \\draw[step=1] (0,0) grid (%d,%d);\n",
	   b.width (), (b.height () + 1) / 2, b.width (), (b.height () + 1) / 2);

  fprintf (fp, "  \\draw (%.3f,-0.8) node[below] {$",
	   ((double)b.width () + 0.5) / 2);
  if (last)
    fprintf (fp, "E_\\infty = ");
  fprintf (fp, "E_{%d}$};\n",
	   dh);
  
  /* label axes */
  for (int x = b.minh; x <= b.maxh; x ++)
    {
      fprintf (fp, "\
  \\draw (%d.5,-.2) node[below] {$%d$};\n",
	       x - b.minh, x);
    }
  for (int y = b.minq; y <= b.maxq; y += 2)
    {
      fprintf (fp, "\
  \\draw (-.2,%d.5) node[left] {$%d$};\n",
	       (y - b.minq) / 2, y);
    }
  
  for (unsigned i = 0; i < b.width (); i ++)
    for (unsigned j = 0; j < b.height (); j ++)
      {
	int r = rank[i][j];
	if (r == 1)
	  {
	    assert (is_even (j));
	    fprintf (fp, "\
  \\fill (%d.5, %d.5) circle (.15);\n",
		     i, j / 2);
	  }
	else if (r > 1)
	  {
	    assert (is_even (j));
	    fprintf (fp, "\
  \\draw (%d.5, %d.5) node {$%d$};\n",
		    i, j / 2, r);
	  }
      }
  
  for (unsigned i = 0; i < b.width (); i ++)
    for (unsigned j = 0; j < b.height (); j ++)
      {
	unsigned r = im_rank[i][j];
	if (r == 0)
	  continue;
	
	unsigned dx = dh,
	  dy = dh - 1;  // in "boxes"
	assert (dx >= 2); // eliminated d_1
	
	double h = sqrt ((double)(dx * dx + dy * dy));
	double xadj = (double)dx / h * 0.3;
	double yadj = (double)dy / h * 0.3;
	
	assert (is_even (j));
	fprintf (fp, "  \\draw[->] (%.3f, %.3f) -- ",
		(double)i + 0.5 + xadj, (double)(j / 2) + 0.5 + yadj);
	if (r > 1)
	  fprintf (fp, "node[color=red!75!black] {$%d$} ", r);
	fprintf (fp, "(%.3f, %.3f);\n",
		(double)(i + dx) + 0.5 - xadj, (double)(j / 2 + dy) + 0.5 - yadj);
      }
  
  fprintf (fp, "\
\\end{tikzpicture}\n");
  
  fflush (fp);
}

sseq
sseq::operator + (const sseq &ss2) const
{
  sseq_bounds b (std::min (bounds.minh, ss2.bounds.minh),
		 std::max (bounds.maxh, ss2.bounds.maxh),
		 std::min (bounds.minq, ss2.bounds.minq),
		 std::max (bounds.maxq, ss2.bounds.maxq));
  sseq r (b);
  
  unsigned n = std::max (pages.size (),
			 ss2.pages.size ());
  for (unsigned i = 1; i <= n; i ++)
    {
      sseq_page pg (b);
      if (i <= pages.size ())
	pg.addeq (b, bounds, pages[i]);
      if (i <= ss2.pages.size ())
	pg.addeq (b, ss2.bounds, ss2.pages[i]);
      r.pages.append (pg);
    }

  return r;
}

sseq
sseq::otimes (const sseq &ss2) const
{
  sseq_bounds b (bounds.minh + ss2.bounds.minh,
		 bounds.maxh + ss2.bounds.maxh,
		 bounds.minq + ss2.bounds.minq + 1,
		 bounds.maxq + ss2.bounds.maxq + 1);
  sseq r (b);
  
  unsigned n1 = pages.size (),
    n2 = ss2.pages.size ();
  
  unsigned n = std::max (n1, n2);
  for (unsigned i = 1; i <= n; i ++)
    {
      sseq_page pg (b);
      pg.otimes_addeq (b,
		       bounds, pages[std::min (i, n1)],
		       ss2.bounds, ss2.pages[std::min (i, n2)]);
      r.pages.append (pg);
    }

  return r;
}

sseq
sseq::shift (int dh, int dq) const
{
  return sseq (sseq_bounds (bounds.minh + dh,
			    bounds.maxh + dh,
			    bounds.minq + dq,
			    bounds.maxq + dq),
	       pages);
}

bool
sseq::equal_as_spaces (const sseq &ss) const
{
  if (bounds != ss.bounds
      || pages.size () != ss.pages.size ())
    return 0;
  
  for (unsigned i = 1; i <= pages.size (); i ++)
    {
      if (!pages[i].equal_as_spaces (ss.pages[i]))
	return 0;
    }
  
  return 1;
}

void
sseq::texshow (FILE *fp, std::string name)
{
  fprintf (fp, "\\section{Knot $%s$}\n", name.c_str ());
  
  unsigned n = pages.size ();
  for (unsigned i = 1; i <= n; i ++)
    fprintf (fp, "$\\rank E_%d = %d$ \\\\\n",
	    i + 1, pages[i].total_rank ());
  
  for (unsigned i = 1; i <= n; i ++)
    pages[i].texshow (fp, bounds, i + 1, i == n);
}

void
sseq_builder::cancel (unsigned d)
{
  unsigned n = C->dim ();

  unsigned step = n / 8;
  if (verbose
      && d == 1
      && step)
    {
      fprintf (stderr, "cancelling generators...\n");
      fprintf (stderr, "%d generators.\n", n);
    }
  
  for (unsigned i = n; i >= 1; i --)
    {
      if (! (present % i))
	continue;
      
      if (verbose
	  && d == 1
	  && step
	  && ((n - i) % step) == 0)
	{
	  fprintf (stderr, "%d / %d generators cancelled.\n", (n - i), n);
	}
      
      grading igr = C->generator_grading (i);
      for (set_const_iter<unsigned> j = im[i]; j; j ++)
	{
	  grading jgr = C->generator_grading (j.val ());
	  assert (jgr.h > igr.h);
	  if ((unsigned)(jgr.h - igr.h) == d)
	    {
	      cancel (i, j.val ());
	      break;
	    }
	}
    }
  
  if (verbose
      && d == 1
      && step
      && (n % step) != 0)
    {
      fprintf (stderr, "%d / %d generators cancelled.\n", n, n);
    }
  if (verbose
      && d == 1
      && step)
    {
      fprintf (stderr, "cancelling generators done.\n");
    }
  
#if 0
  unsigned m = present.card ();
  
  basedvector<unsigned, 1> pri (m),
    nth (m);
  for (ullmanset<1>::const_iter ii = present; ii; ii ++)
    {
      unsigned i = ii.val ();
      grading igr = C->generator_grading (i);
      
      unsigned max_pri = 0;
      for (set_const_iter<unsigned> j = im[i]; j; j ++)
	{
	  grading jgr = C->generator_grading (j.val ());
	  assert (jgr.h > igr.h);
	  if ((unsigned)(jgr.h - igr.h) == d)
	    {
	      unsigned p = 100000000 - (im[i].card () - 1) * (preim[j.val ()].card () - 1);
	      if (p > max_pri)
		max_pri = p;
	    }
	}
      
      unsigned p = ii.pos () + 1;
      pri[p] = max_pri;
      nth[p] = i;
    }
  priority_queue q (m, pri);
  for (unsigned i = 1; i <= m; i ++)
    q.push (i);
  while (!q.is_empty ())
    {
      unsigned p = q.pop ();
      unsigned i = nth[p];
      
      grading igr = C->generator_grading (i);
      
      unsigned max_pri = 0,
	maxj = 0;
      for (set_const_iter<unsigned> j = im[i]; j; j ++)
	{
	  grading jgr = C->generator_grading (j.val ());
	  assert (jgr.h > igr.h);
	  if ((unsigned)(jgr.h - igr.h) == d)
	    {
	      unsigned p = 100000000 - (im[i].card () - 1) * (preim[j.val ()].card () - 1);
	      if (p > max_pri)
		{
		  max_pri = p;
		  maxj = j.val ();
		}
	    }
	}
      if (maxj)
	cancel (i, maxj);
    }
#endif
  
#ifndef NDEBUG
  for (ullmanset<1>::const_iter ii = present; ii; ii ++)
    {
      unsigned i = ii.val ();
      grading igr = C->generator_grading (i);
      for (set_const_iter<unsigned> j = im[i]; j; j ++)
	{
	  grading jgr = C->generator_grading (j.val ());
	  assert (jgr.h > igr.h);
	  assert ((unsigned)(jgr.h - igr.h) > d);
	}
    }
#endif
}

void 
sseq_builder::cancel (unsigned i, unsigned j)
{
  assert (i != j);
  
  present.yank (i);
  present.yank (j);
  
  im[i].yank (j);
  preim[j].yank (i);
  
  for (set_const_iter<unsigned> k = im[i]; k; k ++)
    preim[k.val ()].yank (i);
  for (set_const_iter<unsigned> k = preim[i]; k; k ++)
    im[k.val ()].yank (i);
  for (set_const_iter<unsigned> k = im[j]; k; k ++)
    preim[k.val ()].yank (j);
  for (set_const_iter<unsigned> k = preim[j]; k; k ++)
    im[k.val ()].yank (j);
  
  for (set_const_iter<unsigned> kk = preim[j]; kk; kk ++)
    {
      for (set_const_iter<unsigned> ll = im[i]; ll; ll ++)
	{
	  unsigned k = kk.val (),
	    ell = ll.val ();
	  
	  assert (present % k);
	  assert (present % ell);
	  assert (k != i);
	  assert (k != j);
	  assert (ell != i);
	  assert (ell != j);
	  assert (ell != k);
	  
	  if (im[k].toggle (ell))
	    preim[ell].yank (k);
	  else
	    preim[ell].push (k);
	}
    }
  
  im[i].clear ();
  preim[i].clear ();
  im[j].clear ();
  preim[j].clear ();
}

sseq_builder::linear_combination
sseq_builder::lc (const set<unsigned> &s)
{
  linear_combination r;
  for (set<unsigned>::const_iter i = s; i; i ++)
    r.muladd (1, i.val ());
  return r;
}

bool
sseq_builder::im_zero ()
{
  for (ullmanset<1>::const_iter i = present; i; i ++)
    {
      if (!im[i.val ()].is_empty ())
	return 0;
    }
  return 1;
}

sseq_builder::sseq_builder (const ptr<const module<Z2> > &C_,
			    const mod_map<Z2> &d)
  : C(C_), present(C->dim ()), im(C->dim ()), preim(C->dim ())
{
  unsigned n = C->dim ();
  
  for (unsigned i = 1; i <= n; i ++)
    present.push (i);
  
  for (unsigned i = 1; i <= n; i ++)
    {
      for (linear_combination_const_iter j = d[i]; j; j ++)
	{
	  im[i].push (j.key ());
	  preim[j.key ()].push (i);
	}
    }
}

sseq
sseq_builder::build_sseq ()
{
  if (verbose)
    {
      fprintf (stderr, "computing spectral sequence...\n");
      fprintf (stderr, "computing E^1...\n");
    }
  
  unsigned dh = 1;
  cancel (dh);
  dh ++;
  
  if (verbose)
    fprintf (stderr, "computing E^1 done.\n");
  
  unsigned i0 = present.head ();
  grading i0gr = C->generator_grading (i0);
  int minh = i0gr.h,
    maxh = i0gr.h,
    minq = i0gr.q,
    maxq = i0gr.q;
  for (ullmanset<1>::const_iter i = present; i; i ++)
    {
      grading igr = C->generator_grading (i.val ());
      if (igr.h < minh)
	minh = igr.h;
      if (igr.h > maxh)
	maxh = igr.h;
      if (igr.q < minq)
	minq = igr.q;
      if (igr.q > maxq)
	maxq = igr.q;
    }
  sseq_bounds b (minh, maxh, minq, maxq);
  sseq r (b);
  
#if 0
  printf ("minh=%d, maxh=%d, minq=%d, maxq=%d\n",
	  minh, maxh, minq, maxq);
#endif
  
  for (;;)
    {
      if (verbose)
	fprintf (stderr, "computing E^%d...\n", dh);
      
      sseq_page pg (b);
      
      basedvector<linear_combination, 1> span;
      for (ullmanset<1>::const_iter i = present; i; i ++)
	{
	  grading igr = C->generator_grading (i.val ());
	  
#if 0
	  printf ("i=%d, igr.h=%d, igr.q=%d\n",
		  i.val (), igr.h, igr.q);
#endif
	  
	  pg.rank[igr.h - minh][igr.q - minq] ++;
	  
	  linear_combination v (C);
	  for (set<unsigned>::const_iter j = im[i.val ()]; j; j ++)
	    {
	      grading jgr = C->generator_grading (j.val ());
	      
	      if (jgr.h - igr.h == (int)dh)
		{
		  assert ((jgr.h - igr.h - 1) * 2 == (jgr.q - igr.q));
		  
		  v.muladd (1, j.val ());
		}
	    }
	  span.append (v);
	}
      
      unsigned dq = (dh - 1) * 2;
      
      mod_span<Z2> span2 (C, span);
      ptr<const module<Z2> > im2 = C->submodule (span2);
      
      for (unsigned i = 1; i <= im2->dim (); i ++)
	{
	  grading igr = im2->generator_grading (i);
	  pg.im_rank[igr.h - dh - minh][igr.q - dq - minq] ++;
	}
      
      r.pages.append (pg);
      
      if (verbose)
	fprintf (stderr, "computing E^%d done.\n", dh);
      
      if (im_zero ())
	break;
      
      cancel (dh);
      dh ++;
    }
  
  if (verbose)
    fprintf (stderr, "computing spectral sequence done.\n");
  
  return r;
}
