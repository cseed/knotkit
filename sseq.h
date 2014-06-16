
class sseq_bounds
{
 public:
  int minh, maxh,
    minq, maxq;
  
 public:
  sseq_bounds () { }
  
  template<class R, class M> sseq_bounds (ptr<const module<R> > C, M m)
  {
    grading hq1 = m(C->generator_grading (1));
    minh = maxh = hq1.h;
    minq = maxq = hq1.q;
    
    for (unsigned i = 2; i <= C->dim (); i ++)
      {
	grading hq = m(C->generator_grading (i));
	if (hq.h < minh)
	  minh = hq.h;
	if (hq.h > maxh)
	  maxh = hq.h;
	if (hq.q < minq)
	  minq = hq.q;
	if (hq.q > maxq)
	  maxq = hq.q;
      }
  }
  
  sseq_bounds (int minh_, int maxh_, int minq_, int maxq_)
    : minh(minh_), maxh(maxh_), minq(minq_), maxq(maxq_)
  { }
  sseq_bounds (const sseq_bounds &b)
    : minh(b.minh), maxh(b.maxh), minq(b.minq), maxq(b.maxq)
  { }
  ~sseq_bounds ()  { }
  
  sseq_bounds &operator = (const sseq_bounds &b)
  {
    minh = b.minh;
    maxh = b.maxh;
    minq = b.minq;
    maxq = b.maxq;
    return *this;
  }
  
  unsigned width () const { return maxh - minh + 1; }
  unsigned height () const { return maxq - minq + 1; }
  
  bool operator == (const sseq_bounds &b) const
  {
    return minh == b.minh
    && maxh == b.maxh
    && minq == b.minq
    && maxq == b.maxq;
  }
  bool operator != (const sseq_bounds &b) const { return !operator == (b); }
};

class sseq_page
{
 public:
  /* bounds come from sseq */
  unsigned k;
  grading dk_gr;
  vector<vector<unsigned> > rank,
    im_rank;
  
 public:
  sseq_page () { }
  sseq_page (const sseq_bounds &b);
  template<class R, class M> sseq_page (const sseq_bounds &b,
					unsigned k_,
					grading dk_gr_,
					mod_map<R> d,
					M m);
  sseq_page (const sseq_page &pg)
    : k(pg.k), dk_gr(pg.dk_gr), rank(pg.rank), im_rank(pg.im_rank)
  { }
  ~sseq_page () { }
  
  bool equal_as_spaces (const sseq_page &pg) const { return rank == pg.rank; }

  bool operator == (const sseq_page &pg) const
  {
    return k == pg.k
    && dk_gr == pg.dk_gr
    && rank == pg.rank
    && im_rank == pg.im_rank;
  }

  bool operator != (const sseq_page &pg) const { return !operator == (pg); }
  
  unsigned total_rank () const;
  multivariate_laurentpoly<Z> poincare_polynomial (const sseq_bounds &b) const;
  multivariate_laurentpoly<Z> delta_poincare_polynomial (const sseq_bounds &b) const;
  unsigned homological_width (const sseq_bounds &b) const;
  
  void addeq (const sseq_bounds &b, const sseq_bounds &b2, const sseq_page &pg2);
  void otimes_addeq (const sseq_bounds &b,
		     const sseq_bounds &b1, const sseq_page &pg1,
		     const sseq_bounds &b2, const sseq_page &pg2);
  
  template<class M> void texshow (FILE *fp, const sseq_bounds &b, const char *label, M m);
};

class sseq
{
 public:
  sseq_bounds bounds;
  basedvector<sseq_page, 1> pages;
  
 public:
  sseq () { }
  sseq (const sseq_bounds &b,
	const basedvector<sseq_page, 1> &pages_)
    : bounds(b),
      pages(pages_)
  { }
  sseq (const sseq_bounds &b)
    : bounds(b)
  { }
  sseq (const sseq &ss) : bounds(ss.bounds), pages(ss.pages) { }
  ~sseq () { }
  
  sseq &operator = (const sseq &ss)
  {
    bounds = ss.bounds;
    pages = ss.pages;
    return *this;
  }
  
  sseq operator + (const sseq &ss2) const;  // direct sum
  sseq otimes (const sseq &ss2) const; // tensor product
  sseq shift (int dh, int dq) const;
  
  unsigned e2_rank () const { return pages[1].total_rank (); }
  unsigned einf_rank () const { return pages[pages.size ()].total_rank (); }
  unsigned homological_width () const { return pages[1].homological_width (bounds); }
  
  bool operator == (const sseq &ss) const { return bounds == ss.bounds && pages == ss.pages; }
  bool operator != (const sseq &ss) const { return !operator == (ss); }
  
  template<class M> void texshow (FILE *fp, M m);
};

template<class R, class M> 
sseq_page::sseq_page (const sseq_bounds &b,
		      unsigned k_,
		      grading dk_gr_,
		      mod_map<R> dk,
		      M m)
  : k(k_),
    dk_gr(dk_gr_),
    rank(b.width ()),
    im_rank(b.width ())
{
  dk.check_grading (dk_gr);
  
  grading m_dk_gr = m.map_delta(dk_gr);
  
  for (unsigned i = 0; i < b.width (); i ++)
    {
      vector<unsigned> r (b.height ()),
	im_r (b.height ());
      for (unsigned j = 0; j < b.height (); j ++)
	r[j] = im_r[j] = 0;
      
      rank[i] = r;
      im_rank[i] = im_r;
    }
  
  ptr<const module<R> > C = dk.domain ();
  for (unsigned i = 1; i <= C->dim (); i ++)
    {
      grading gr = m(C->generator_grading (i));
      rank[(unsigned)(gr.h - b.minh)][(unsigned)(gr.q - b.minq)] ++;
    }
  
  ptr<const free_submodule<R> > im = dk.image ();
  for (unsigned i = 1; i <= im->dim (); i ++)
    {
      grading gr = m(im->generator_grading (i));
      im_rank[(unsigned)(gr.h - m_dk_gr.h - b.minh)][(unsigned)(gr.q - m_dk_gr.q - b.minq)] ++;
    }
}


template<class M> void
sseq_page::texshow (FILE *fp, const sseq_bounds &b, const char *label, M m)
{
  grading m_dk_gr = m.map_delta(dk_gr);
  
  unsigned dh = m_dk_gr.h,
    dq = m_dk_gr.q;
  
  fprintf (fp, "\
\\begin{tikzpicture}[scale=.66]\n\
  \\draw[->] (0,0) -- (%d.5,0) node[right] {$t$};\n\
  \\draw[->] (0,0) -- (0,%d.5) node[above] {$q$};\n\
  \\draw[step=1] (0,0) grid (%d,%d);\n",
	   b.width (), b.height (), b.width (), b.height ());
  
  fprintf (fp, "  \\draw (%.3f,-0.8) node[below] {%s};\n",
	   ((double)b.width () + 0.5) / 2, label);
  
  /* label axes */
  for (int x = b.minh; x <= b.maxh; x ++)
    {
      fprintf (fp, "\
  \\draw (%d.5,-.2) node[below] {$",
	       x - b.minh);
      m.x_label (fp, x);
      fprintf (fp, "$};\n");
    }
  for (int y = b.minq; y <= b.maxq; y ++)
    {
      fprintf (fp, "\
  \\draw (-.2,%d.5) node[left] {$",
	       y - b.minq);
      m.y_label (fp, y);
      fprintf (fp, "$};\n");
    }
  
  for (unsigned i = 0; i < b.width (); i ++)
    for (unsigned j = 0; j < b.height (); j ++)
      {
	int r = rank[i][j];
	if (r == 1)
	  {
	    fprintf (fp, "\
  \\fill (%d.5, %d.5) circle (.15);\n",
		     i, j);
	  }
	else if (r > 1)
	  {
	    fprintf (fp, "\
  \\draw (%d.5, %d.5) node {$%d$};\n",
		    i, j, r);
	  }
      }
  
  for (unsigned i = 0; i < b.width (); i ++)
    for (unsigned j = 0; j < b.height (); j ++)
      {
	unsigned r = im_rank[i][j];
	if (r == 0)
	  continue;
	
	int dx = dh,
	  dy = dq;  // in "boxes"
	
	double h = sqrt ((double)(dx * dx + dy * dy));
	double xadj = (double)dx / h * 0.3;
	double yadj = (double)dy / h * 0.3;
	
	fprintf (fp, "  \\draw[->] (%.3f, %.3f) -- ",
		(double)i + 0.5 + xadj, (double)j + 0.5 + yadj);
	if (r > 1)
	  fprintf (fp, "node[color=red!75!black] {$%d$} ", r);
	fprintf (fp, "(%.3f, %.3f);\n",
		 (double)((int)i + dx) + 0.5 - xadj, (double)((int)j + dy) + 0.5 - yadj);
      }
  
  fprintf (fp, "\
\\end{tikzpicture}\n");
  
  fflush (fp);
}

template<class M> void
sseq::texshow (FILE *fp, M m)
{
  unsigned n = pages.size ();
  for (unsigned i = 1; i <= n; i ++)
    {
      if (i == n)
	fprintf (fp, "$\\rank E_\\infty = \\rank E_%d = %d$ \\\\\n",
		 pages[i].k, pages[i].total_rank ());
      else
	fprintf (fp, "$\\rank E_%d = %d$ \\\\\n",
		 pages[i].k, pages[i].total_rank ());
    }
  
  for (unsigned i = 1; i <= n; i ++)
    {
      char buf[1000];
      if (i == n)
	sprintf (buf, "$E_\\infty = E_{%d}$", pages[i].k);
      else
	sprintf (buf, "$E_{%d}$", pages[i].k);
      pages[i].texshow (fp, bounds, buf, m);
    }
}

class sseq_builder
{
 private:
  typedef Z2::linear_combination linear_combination;
  // typedef Z2::linear_combination_iter linear_combination_iter;
  typedef Z2::linear_combination_const_iter linear_combination_const_iter;
  
 public:
  ptr<const module<Z2> > C;
  
  ullmanset<1> present;
  basedvector<set<unsigned>, 1> im, preim;
  
  void cancel (unsigned i, unsigned j);
  void cancel (unsigned d);
  linear_combination lc (const set<unsigned> &s);
  bool im_zero ();
  
public:
  sseq_builder (const ptr<const module<Z2> > &C_, const mod_map<Z2> &d);
  
  sseq build_sseq ();
};
