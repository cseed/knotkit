
class sseq_bounds
{
 public:
  int minh, maxh,
    minq, maxq;
  
 public:
  sseq_bounds () { }
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
  vector<vector<unsigned> > rank,
    im_rank;
  
 public:
  sseq_page () { }
  sseq_page (const sseq_bounds &b);
  sseq_page (const sseq_page &pg)
    : rank(pg.rank), im_rank(pg.im_rank)
  { }
  ~sseq_page () { }
  
  bool operator == (const sseq_page &pg) const { return rank == pg.rank && im_rank == pg.im_rank; }
  bool operator != (const sseq_page &pg) const { return !operator == (pg); }
  
  bool equal_as_spaces (const sseq_page &pg) const { return rank == pg.rank; }
  
  unsigned total_rank () const;
  multivariate_laurentpoly<Z> poincare_polynomial (const sseq_bounds &b) const;
  multivariate_laurentpoly<Z> delta_poincare_polynomial (const sseq_bounds &b) const;
  unsigned homological_width (const sseq_bounds &b) const;
  
  void addeq (const sseq_bounds &b, const sseq_bounds &b2, const sseq_page &pg2);
  void otimes_addeq (const sseq_bounds &b,
		     const sseq_bounds &b1, const sseq_page &pg1,
		     const sseq_bounds &b2, const sseq_page &pg2);
  
  void texshow (FILE *fp, const sseq_bounds &b, unsigned dh, bool last);
};

class sseq
{
 public:
  sseq_bounds bounds;
  basedvector<sseq_page, 1> pages;
  
 public:
  sseq () { }
  sseq (const sseq_bounds &b, const basedvector<sseq_page, 1> &pages_)
    : bounds(b), pages(pages_)
  { }
  sseq (const sseq_bounds &b)
    : bounds(b)
  { }
  sseq (const sseq &ss) : bounds(ss.bounds), pages(ss.pages) { }
  ~sseq () { }
  
  sseq &operator = (const sseq &ss) { bounds = ss.bounds; pages = ss.pages; return *this; }
  
  sseq operator + (const sseq &ss2) const;  // direct sum
  sseq otimes (const sseq &ss2) const; // tensor product
  sseq shift (int dh, int dq) const;
  
  unsigned e2_rank () const { return pages[1].total_rank (); }
  unsigned einf_rank () const { return pages[pages.size ()].total_rank (); }
  unsigned homological_width () const { return pages[1].homological_width (bounds); }
  
  bool operator == (const sseq &ss) const { return bounds == ss.bounds && pages == ss.pages; }
  bool operator != (const sseq &ss) const { return !operator == (ss); }

  
  bool equal_as_spaces (const sseq &ss) const;
  
  void texshow (FILE *fp, std::string name);
};

template<class R> class simplified_complex_generators
{
  unsigned new_n;
  ptr<const module<R> > C;
  basedvector<unsigned, 1> new_C_to_C_generator;
  
public:
  simplified_complex_generators (const simplified_complex_generators &g)
    : new_n(g.new_n),
      C(g.C),
      new_C_to_C_generator(g.new_C_to_C_generator)
  { }
  simplified_complex_generators (unsigned new_n_,
				 ptr<const module<R> > C_,
				 basedvector<unsigned, 1> new_C_to_C_generator_)
    : new_n(new_n_),
      C(C_),
      new_C_to_C_generator(new_C_to_C_generator_)
  { }
  ~simplified_complex_generators () { }
  
  simplified_complex_generators &operator = (const simplified_complex_generators &) = delete;
  
  unsigned dim () const { return new_n; }
  unsigned free_rank () const { return new_n; }
  grading generator_grading (unsigned i) const { return C->generator_grading (new_C_to_C_generator[i]); }
  void show_generator (unsigned i) const { C->show_generator (new_C_to_C_generator[i]); }
  R generator_ann (unsigned i) const { abort (); }
};

template<class R> 
class chain_complex_simplifier
{
 public:
  typedef typename R::linear_combination linear_combination;
  typedef typename R::linear_combination_const_iter linear_combination_const_iter;
  
 public:
  ptr<const module<R> > C;
  unsigned n; // |C|
  const mod_map<R> &d;
  
  ptr<const module<R> > new_C;
  mod_map<R> new_d;
  
  // chain homotopy equivalences
  // pi : C -> new_C, iota : new_C -> C
  mod_map<R> pi, iota;
  
 private:
  basedvector<linear_combination, 1> new_d0;
  basedvector<set<unsigned>, 1> preim;
  
  bool build_pi_iota;
  mod_map<R> pi0, iota0;
  set<unsigned> canceled;
  
  void cancel (unsigned i, R b, unsigned j);
  
 public:
  chain_complex_simplifier (ptr<const module<R> > C_,
			    const mod_map<R> &d_,
			    int dh,
			    bool build_pi_iota_ = 0);
  
};

template<class R> void
chain_complex_simplifier<R>::cancel (unsigned i, R b, unsigned j)
{
  assert (i != j);
  assert (b.is_unit ());
  
  R binv = b.recip ();
  
  canceled.push (i);
  canceled.push (j);
  
  new_d0[i].yank (j);
  preim[j].yank (i);
  
  for (linear_combination_const_iter k = new_d0[i]; k; k ++)
    preim[k.key ()].yank (i);
  for (set_const_iter<unsigned> k = preim[i]; k; k ++)
    new_d0[k.val ()].yank (i);
  for (linear_combination_const_iter k = new_d0[j]; k; k ++)
    preim[k.key ()].yank (j);

  if (build_pi_iota)
    {
      mod_map<R> local_pi0 (C, 1),
	local_iota0 (C, 1);
      
      local_pi0[i].clear ();
      local_pi0[j].clear ();
      
      local_iota0[i].clear ();
      local_iota0[j].clear ();
      
      for (linear_combination_const_iter ll = new_d0[i]; ll; ll ++)
	{
	  R c = ll.val ();
	  local_pi0[j].mulsub (binv * c, ll.key ());
	}
      
      for (set_const_iter<unsigned> kk = preim[j]; kk; kk ++)
	{
	  unsigned k = kk.val ();
	  R a = new_d0[k](j);
	  assert (a != 0);
	  
	  local_iota0[k].mulsub (a * binv, i);
	}
      
      pi0 = local_pi0.compose (pi0);
      iota0 = iota0.compose (local_iota0);
    }
  
  for (set_const_iter<unsigned> kk = preim[j]; kk; kk ++)
    {
      unsigned k = kk.val ();
      R a = new_d0[k](j);
      assert (a != 0);
      
      R abinv = a * binv;
      
      for (linear_combination_const_iter ll = new_d0[i]; ll; ll ++)
	{
	  unsigned ell = ll.key ();
	  R c = ll.val ();
	  
	  assert (! (canceled % k));
	  assert (! (canceled % ell));
	  assert (k != i);
	  assert (k != j);
	  assert (ell != i);
	  assert (ell != j);
	  assert (ell != k);
	  
	  new_d0[k].mulsub (abinv * c, ell);
	  if (new_d0[k] % ell)
	    preim[ell] += k;
	  else
	    preim[ell] -= k;
	}
    }
  
  for (set_const_iter<unsigned> k = preim[j]; k; k ++)
    new_d0[k.val ()].yank (j);
  
  new_d0[i].clear ();
  preim[i].clear ();
  new_d0[j].clear ();
  preim[j].clear ();
  
#if 0  // expensive
  if (build_pi_iota)
    {
      assert (pi0.compose (d) == new_d0.compose (pi0));
      assert (iota0.compose (new_d0) == d.compose (iota0));
    }
#endif
}

template<class R>
chain_complex_simplifier<R>::chain_complex_simplifier (ptr<const module<R> > C_,
						       const mod_map<R> &d_,
						       int dh,
						       bool build_pi_iota_)
  : C(C_), n(C_->dim ()), d(d_),
    new_d0(COPY2, d_.explicit_columns ()),
    preim(C_->dim ()),
    build_pi_iota(build_pi_iota_)
{
  for (unsigned i = 1; i <= n; i ++)
    {
      for (linear_combination_const_iter j = d[i]; j; j ++)
	preim[j.key ()].push (i);
    }
  
  if (build_pi_iota)
    {
      pi0 = mod_map<R> (C, 1);
      iota0 = mod_map<R> (C, 1);
      
#if 0 // expensive
      assert (pi0.compose (d) == new_d0.compose (pi0));
      assert (iota0.compose (new_d0) == d.compose (iota0));
#endif
    }
  
 Lretry:
  for (unsigned i = n; i >= 1; i --)
    {
      if (canceled % i)
	continue;
      
      grading igr = C->generator_grading (i);
      for (linear_combination_const_iter j = new_d0[i]; j; j ++)
	{
	  grading jgr = C->generator_grading (j.key ());
	  assert (jgr.h >= igr.h);
	  if (j.val ().is_unit ()
	      && jgr.h - igr.h == dh)
	    {
	      cancel (i, j.val (), j.key ());
	      goto Lretry;
	    }
	}
    }
  
  unsigned new_n = n - canceled.card ();
  basedvector<unsigned, 1> new_C_to_C_generator (new_n),
    C_to_new_C_generator (n);
  for (unsigned i = 1, j = 1; i <= n; i ++)
    {
      if (canceled % i)
	{
	  C_to_new_C_generator[i] = 0;
	  continue;
	}
      
      C_to_new_C_generator[i] = j;
      new_C_to_C_generator[j] = i;
      j ++;
    }
  
  new_C = (new base_module<R, simplified_complex_generators<R> >
	   (simplified_complex_generators<R> (new_n, C, new_C_to_C_generator)));
  new_d = mod_map<R> (new_C);
  
  for (unsigned i = 1; i <= new_n; i ++)
    {
      unsigned i0 = new_C_to_C_generator[i];
	  
      for (linear_combination_const_iter j0 = new_d0[i0]; j0; j0 ++)
	{
	  unsigned j = C_to_new_C_generator[j0.key ()];
	  assert (j != 0);
	  
	  new_d[i].muladd (j0.val (), j);
	}
    }
  
  if (build_pi_iota)
    {
      pi = mod_map<R> (C, new_C);
      iota = mod_map<R> (new_C, C);
      
      for (unsigned i0 = 1; i0 <= n; i0 ++)
	{
	  for (linear_combination_const_iter j0 = pi0[i0]; j0; j0 ++)
	    {
	      unsigned j = C_to_new_C_generator[j0.key ()];
	      pi[i0].muladd (j0.val (), j);
	    }
	}
      
      for (unsigned i = 1; i <= new_n; i ++)
	{
	  unsigned i0 = new_C_to_C_generator[i];
	  iota[i] = iota0[i0];
	}
      
#if 0  // expensive
      assert (pi.compose (d) == new_d.compose (pi));
      assert (d.compose (iota) == iota.compose (new_d));
#endif
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
