
template<class R> class mod_map;
template<class R> class mod_span;
template<class R> class free_submodule;
template<class R> class quotient_module;

/* `module' is a bigraded module over a ring R. */
template<class R>
class module : public refcounted
{
  typedef typename R::linear_combination linear_combination;
  typedef typename R::linear_combination_const_iter linear_combination_const_iter;
  
  typedef mod_map<R> Rmod_map;
  typedef mod_span<R> Rmod_span;
  
 public:
  module () { }
  module (const module &); // doesn't exist
  virtual ~module () { }
  
  module &operator = (const module &); // doesn't exist
  
 public:
  // the number of generators; n
  virtual unsigned dim () const = 0;
  
  // r; 1 <= r <= n
  virtual unsigned free_rank () const = 0;
  
  virtual grading generator_grading (unsigned i) const = 0;
  virtual void show_generator (unsigned i) const = 0;
  
  // r < i <= n
  virtual R generator_ann (unsigned i) const = 0;
  
  bool is_zero (R c, unsigned i) const
  {
    if (i <= free_rank ())
      return c == 0;
    else
      return generator_ann (i) | c;
  }
  
  R annihilator (R c, unsigned i) const
  {
    R iann = generator_ann (i);
    
    return iann.div (iann.gcd (c));
  }
  
  bool isomorphic (ptr<const module<R> > m) const;
  
  ptr<const quotient_module<R> > quotient (const Rmod_span &span) const;
  ptr<const quotient_module<R> > quotient (ptr<const free_submodule<R> > m) const;
  
  ptr<const free_submodule<R> > submodule (const Rmod_span &span) const;
  
  multivariate_laurentpoly<Z> free_poincare_polynomial () const;
  multivariate_laurentpoly<Z> free_delta_poincare_polynomial () const;
  
  void show_self () const;
  void display_self () const;
};

template<class R, class G>
class base_module : public module<R>
{
 private:
  G g;
  
 public:
  base_module (); // doesn't exist
  base_module (const G &g_) : g(g_) { }
  ~base_module () { }
  
  base_module &operator = (const base_module &); // doesn't exist
  
  unsigned dim () const { return g.dim (); }
  unsigned free_rank () const { return g.free_rank (); }
  grading generator_grading (unsigned i) const { return g.generator_grading (i); }
  void show_generator (unsigned i) const { g.show_generator (i); }
  R generator_ann (unsigned i) const { return g.generator_ann (i); }
};

template<class R>
class explicit_module : public module<R>
{
  unsigned r;
  basedvector<R, 1> ann;
  basedvector<grading, 1> hq;
  
 public:
  explicit_module (); // doesn't exist
  explicit_module (unsigned r_,
		   basedvector<R, 1> ann_,
		   basedvector<grading, 1> hq_)
    : r(r_), ann(ann_), hq(hq_)
  { }
  
  explicit explicit_module (unsigned r_, basedvector<grading, 1> hq_) : r(r_), hq(hq_) { }
  ~explicit_module () { }
  
  explicit_module &operator = (const explicit_module &); // doesn't exist
  
  unsigned dim () const { return r + ann.size (); }
  unsigned free_rank () const { return r; }
  grading generator_grading (unsigned i) const { return hq[i]; }
  void show_generator (unsigned i) const { printf ("%d", i); }
  R generator_ann (unsigned i) const { return ann[i - r]; }
};

template<class R>
class free_submodule : public module<R>
{
  friend class module<R>;
  
  typedef typename R::linear_combination linear_combination;
  typedef typename R::linear_combination_const_iter linear_combination_const_iter;
  
  ptr<const module<R> > parent;
  basedvector<linear_combination, 1> gens;
  basedvector<unsigned, 1> pivots;
  
 public:
  free_submodule (ptr<const module<R> > parent_,
		  basedvector<linear_combination, 1> gens_,
		  basedvector<unsigned, 1> pivots_)
    : parent(parent_),
      gens(gens_),
      pivots(pivots_)
  { }
  ~free_submodule () { }
  
  free_submodule &operator = (const free_submodule &); // doesn't exist
  
  ptr<const module<R> > parent_module () const { return parent; }
  
  unsigned dim () const { return gens.size (); }
  unsigned free_rank () const { return gens.size (); }
  grading generator_grading (unsigned i) const { return gens[i].hq (); }
  void show_generator (unsigned i) const { show (gens[i]); }
  R generator_ann (unsigned i) const { abort (); }
  
  linear_combination inject_generator (unsigned i) const { return gens[i]; }
  linear_combination inject (linear_combination v) const;
  
  mod_map<R> injection_map () const;
  
  linear_combination restrict (linear_combination v0) const;
  ptr<const free_submodule<R> > restrict_submodule (ptr<const free_submodule<R> > m) const;
};

template<class R> 
class quotient_module : public module<R>
{
  friend class module<R>;
  
  typedef typename R::linear_combination linear_combination;
  typedef typename R::linear_combination_const_iter linear_combination_const_iter;
  
  ptr<const module<R> > parent;
  
  // note: these get filled in by module::quotient
  
  basedvector<R, 1> ann;
  
  // parent lc representing i
  basedvector<linear_combination, 1> rep;
  
  // map from parent generator to lc in quotient
  basedvector<map<unsigned, R>, 1> pi;
  
 public:
  quotient_module (const quotient_module &); // doesn't exist
  quotient_module (ptr<const module<R> > parent_)
    : parent(parent_)
  { }
  ~quotient_module () { }
  
  quotient_module &operator = (const quotient_module &); // doesn't exsit
  
  ptr<const module<R> > parent_module () const { return parent; }
  
  unsigned dim () const { return rep.size (); }
  unsigned free_rank () const { return rep.size () - ann.size (); }
  grading generator_grading (unsigned i) const { return rep[i].hq (); }
  void show_generator (unsigned i) const
  {
    show (rep[i]);
    printf ("/~");
  }
  
  R generator_ann (unsigned i) const
  {
    unsigned r = free_rank ();
    assert (i > r);
    return ann[i - r];
  }
  
  linear_combination project_generator (unsigned i) const
  {
    assert (i >= 1 && i <= parent->dim ());
    linear_combination r (this);
    for (typename map<unsigned, R>::const_iter j = pi[i]; j; j ++)
      r.muladd (j.val (), j.key ());
    return r;
  }
  
  linear_combination project (linear_combination v) const
  {
    assert (v.m == parent);
    linear_combination r (this);
    for (linear_combination_const_iter i = v; i; i ++)
      r.muladd (i.val (), project_generator (i.key ()));
    return r;
  }
  
  linear_combination generator_rep (unsigned i) const { return rep[i]; }
};

template<class R>
class mod_map
{
  typedef const module<R> Rmod;
  typedef typename R::linear_combination linear_combination;
  // typedef typename R::linear_combination_iter linear_combination_iter;
  typedef typename R::linear_combination_const_iter linear_combination_const_iter;
  
  ptr<const Rmod> from,
    to;
  basedvector<linear_combination, 1> columns;
  
 public:
  mod_map () { }
  mod_map (ptr<const Rmod> fromto);
  mod_map (ptr<const Rmod> fromto, int i);
  mod_map (ptr<const Rmod> from_, ptr<const Rmod> to_);
  mod_map (ptr<const Rmod> from_,
	   ptr<const Rmod> to_,
	   basedvector<linear_combination, 1> columns_)
    : from(from_), to(to_), columns(columns_)
  { }

  mod_map (copy, const mod_map &m)
    : from(m.from), to(m.to), columns(COPY, m.columns)
  { }
  
  mod_map (copy2, const mod_map &m)
    : from(m.from), to(m.to), columns(COPY2, m.columns)
  { }
  
  bool operator == (const mod_map &m2) const
  {
    assert (from == m2.from);
    assert (to == m2.to);
    return columns == m2.columns;
  }
  
  bool operator == (int x) const
  {
    R c (x);
    assert (c == 0);
    
    for (unsigned i = 1; i <= from->dim (); i ++)
      {
	if (columns[i] != 0)
	  return 0;
      }
    return 1;
  }
  
  bool operator != (int x) const { return !operator == (x); }
  
  mod_map &operator = (const mod_map &m)
  {
    from = m.from;
    to = m.to;
    columns = m.columns;
    return *this;
  }
  
  linear_combination &operator [] (unsigned i) { return columns[i]; }
  const linear_combination &operator [] (unsigned i) const { return columns[i]; }
  
  linear_combination map (const linear_combination &lc) const;
  mod_map compose (const mod_map &m2) const;
  
  mod_map &operator *= (R c);
  mod_map &operator += (const mod_map &m2);
  
  mod_map operator + (const mod_map &m2) const;
  
  bool homogeneous () const;
  void check_grading (grading delta) const;
  
  // inj : ker -> from
  ptr<const free_submodule<R> > kernel () const;
  
  // inj : im -> to
  ptr<const free_submodule<R> > image () const;
  
  ptr<const quotient_module<R> > cokernel () const;
  
  ptr<const quotient_module<R> > homology () const;
  
  mod_map restrict_from (ptr<const free_submodule<R> > new_from) const;
  mod_map restrict_to (ptr<const free_submodule<R> > new_to) const;
  mod_map restrict (ptr<const free_submodule<R> > new_from,
		       ptr<const free_submodule<R> > new_to) const;
  mod_map restrict (ptr<const free_submodule<R> > new_fromto) const
  {
    return restrict (new_fromto, new_fromto);
  }
  
  mod_map induced_map_to (ptr<const quotient_module<R> > new_to);
  mod_map induced_map (ptr<const quotient_module<R> > new_fromto);
  
  void show_self () const;
  void display_self () const;
};

template<class R>
class mod_span
{
  typedef typename R::linear_combination linear_combination;
  typedef typename R::linear_combination_const_iter linear_combination_const_iter;
  
 public:
  basedvector<linear_combination, 1> gens;
  basedvector<unsigned, 1> pivots;
  
 public:
  mod_span (ptr<const module<R> > mod, basedvector<linear_combination, 1> xs);
  ~mod_span () { }
};

template<class R>
class quotient_helper
{
 public:
  typedef typename R::linear_combination linear_combination;
  typedef typename R::linear_combination_const_iter linear_combination_const_iter;
  
 public:
  ptr<const module<R> > mod;
  
  // rows of the presentation matrix
  basedvector<linear_combination, 1> rows;
  
  basedvector<linear_combination, 1> generators;
  basedvector<linear_combination, 1> generators_inv;
  
  bool improve_pivot_row (unsigned i, unsigned j, unsigned i2);
  bool improve_pivot_column (unsigned i, unsigned j, unsigned j2);
  void improve_pivot (unsigned i, unsigned j);
  
 public:
  quotient_helper (ptr<const module<R> > mod_, basedvector<linear_combination, 1> rows);
  
  void normalize ();
};

template<class R> bool
module<R>::isomorphic (ptr<const module<R> > m) const
{
  if (dim () != m->dim ()
      || free_rank () != m->free_rank ())
    return 0;
  
  set<grading> gradings;
  for (unsigned i = 1; i <= dim (); i ++)
    gradings += generator_grading (i);
  
  for (set_const_iter<grading> i = gradings; i; i ++)
    {
      grading hq = i.val ();
      
      unsigned hq_r = 0,
	m_hq_r = 0;
      basedvector<R, 1> hq_ann, m_hq_ann;
      for (unsigned i = 1; i <= dim (); i ++)
	{
	  if (generator_grading (i) == hq)
	    {
	      if (i <= free_rank ())
		hq_r ++;
	      else
		{
		  hq_ann.append (generator_ann (i));
		}
	    }
	  if (m->generator_grading (i) == hq)
	    {
	      if (i <= free_rank ())
		m_hq_r ++;
	      else
		{
		  m_hq_ann.append (m->generator_ann (i));
		}
	    }
	}
      
      if (hq_r != m_hq_r)
	return 0;
      
      if (hq_ann.size () != m_hq_ann.size ())
	return 0;

      for (unsigned i = 1; i <= hq_ann.size (); i ++)
	{
	  R a = hq_ann[i],
	    m_a = m_hq_ann[i];
	  if (! (a | m_a && m_a | a))
	    return 0;
	}
    }
  
  return 1;
}

template<class R>
quotient_helper<R>::quotient_helper (ptr<const module<R> > mod_,
				     basedvector<linear_combination, 1> rows_)
  : mod(mod_),
    rows(rows_),
    generators(mod->dim ()),
    generators_inv(mod->dim ())
{
  assert (mod->dim () == mod->free_rank ());
  
  for (unsigned i = 1; i <= mod->dim (); i ++)
    {
      linear_combination v (mod);
      v.muladd (1, i);
      generators[i] = v;
      
      linear_combination vinv (mod);
      vinv.muladd (1, i);
      generators_inv[i] = vinv;
    }
}

template<class R> bool
quotient_helper<R>::improve_pivot_row (unsigned i, unsigned j, unsigned i2)
{
  assert (i != i2);
  
  const linear_combination &r = rows[i],
    &r2 = rows[i2];
  R rc = r(j),
    r2c = r2(j);
  
  if (r2c == 0)
    return 0;
  
#if 0
  if (rc | r2c)
    {
      R q = r2c.div (rc);
      rows[i2].mulsub (q, r);
      
      assert (rows[i2](j) == 0);
      
      return 0;
    }
#endif
  
  triple<R, R, R> t = rc.extended_gcd (r2c);
  assert (t.first == rc*t.second + t.third*r2c);
  
  rows[i] = r*t.second + r2*t.third;
  rows[i2] = (rc.div (t.first))*r2 - (r2c.div (t.first))*r;
  
  assert ((rc | r2c) == rc.divides (t.first));
  assert (!rc.divides (t.first) || rows[i2](j) == 0);
  
  return !rc.divides (t.first);
}

template<class R> bool
quotient_helper<R>::improve_pivot_column (unsigned i, unsigned j, unsigned j2)
{
  assert (j != j2);
  
#if 0
  basedvector<linear_combination, 1> orig_row_image (rows.size ());
  for (unsigned k = 1; k <= rows.size (); k ++)
    {
      linear_combination r (mod);
      for (linear_combination_const_iter l = rows[k]; l; l ++)
	r.muladd (l.val (), generators[l.key ()]);
      orig_row_image[k] = r;
    }
#endif
  
  const linear_combination &r = rows[i];
  R rc = r(j),
    rc2 = r(j2);
  
  assert (rc != 0);
  if (rc2 == 0)
    return 0;
  
  assert (generators[j].hq () == generators[j2].hq ());
  
#if 0
  if (rc | rc2)
    {
      R q = rc2.div (rc);
      for (unsigned k = 1; k <= rows.size (); k ++)
	{
	  linear_combination &rk = rows[k];
	  rk.mulsub (rk(j) * q, j2);
	}
      
      assert (r(j2) == 0);
      
      return 0;
    }
#endif
  
  triple<R, R, R> t = rc.extended_gcd (rc2);
  assert (t.first == rc*t.second + t.third*rc2);
  
  for (unsigned k = 1; k <= rows.size (); k ++)
    {
      linear_combination &rk = rows[k];
      R rkc = rk(j),
	rkc2 = rk(j2);
      
      rk.set_coeff (rkc*t.second + rkc2*t.third,
		    j);
      rk.set_coeff (rkc2*(rc.div (t.first)) - rkc*(rc2.div (t.first)),
		    j2);
    }
  
  linear_combination g = generators[j],
    g2 = generators[j2];
  
  assert (g.hq () == g2.hq ());
  
  generators[j] = (rc.div (t.first)) * g + (rc2.div (t.first)) * g2;
  generators[j2] = t.second * g2 - t.third * g;
  
 #if 0
  for (unsigned k = 1; k <= rows.size (); k ++)
    {
      linear_combination r2 (mod);
      for (linear_combination_const_iter l = rows[k]; l; l ++)
	r2.muladd (l.val (), generators[l.key ()]);
      assert (r2 == orig_row_image[k]);
    }
#endif
  
  for (unsigned k = 1; k <= mod->dim (); k ++)
    {
      linear_combination &ginv = generators_inv[k];
      
      R d = ginv(j),
	d2 = ginv(j2);
      
      ginv.set_coeff (t.second*d + t.third*d2, j);
      ginv.set_coeff (rc.div (t.first) * d2 - rc2.div (t.first) * d, j2);
    }
  
#if 0
  for (unsigned k = 1; k <= mod->dim (); k ++)
    {
      linear_combination r (mod);
      r.muladd (1, k);
      
      linear_combination r2 (mod);
      for (linear_combination_const_iter l = generators_inv[k]; l; l ++)
	r2.muladd (l.val (), generators[l.key ()]);
      
      assert (r == r2);
    }
#endif
  
  assert ((rc | rc2) == rc.divides (t.first));
  assert (!rc.divides (t.first) || r(j2) == 0);
  
  return !rc.divides (t.first);
}

template<class R> void
quotient_helper<R>::improve_pivot (unsigned i, unsigned j)
{
  for (;;)
    {
      bool changed = 0;
      for (unsigned k = 1; k <= rows.size (); k ++)
	{
	  if (k == i)
	    continue;
	  if (improve_pivot_row (i, j, k))
	    changed = 1;
	}
      
      for (unsigned k = 1; k <= mod->dim (); k ++)
	{
	  if (k == j)
	    continue;
	  if (improve_pivot_column (i, j, k))
	    changed = 1;
	}
      
#if 0
    L:
      for (linear_combination_const_iter k = rows[i]; k; k ++)
	{
	  if (k.key () != j)
	    {
	      if (improve_pivot_column (i, j, k.key ()))
		changed = 1;
	      goto L;
	    }
	}
#endif
      
      if (!changed)
	return;
    }
}

template<class R> void
quotient_helper<R>::normalize ()
{
  for (unsigned i = 1; i <= rows.size (); i ++)
    {
      if (rows[i] == 0)
	continue;
      
      pair<unsigned, R> p = rows[i].head ();
      improve_pivot (i, p.first);
    }
  
  for (unsigned i = 1; i <= rows.size (); i ++)
    {
      if (rows[i] == 0)
	continue;
      
      assert (rows[i].card () == 1);
      pair<unsigned, R> p = rows[i].head ();
      grading ihq = generators[p.first].hq ();
      
      for (unsigned j = i; j <= rows.size (); j ++)
	{
	  if (rows[j] == 0)
	    continue;
	  
	  assert (rows[j].card () == 1);
	  pair<unsigned, R> q = rows[j].head ();
	  grading jhq = generators[q.first].hq ();
	  
	  if (ihq != jhq)
	    continue;
	  
	  if (p.second | q.second)
	    continue;
	  
	  rows[i] += rows[j];
	  improve_pivot (i, p.first);
	  
#ifndef NDEBUG
	  assert (rows[i].card () == 1);
	  pair<unsigned, R> p2 = rows[i].head ();
	  assert (p2.first == p.first);
	  
	  assert (rows[j].card () == 1);
	  pair<unsigned, R> q2 = rows[j].head ();
	  assert (q2.first == q.first);
	  
	  assert (p2.second | q2.second);
#endif
	}
    }
}

template<class R> ptr<const quotient_module<R> > 
module<R>::quotient (const Rmod_span &span) const
{
  unsigned n = dim ();
  
  quotient_helper<R> h (this, span.gens);
  h.normalize ();

  basedvector<R, 1> ann (n);
  for (unsigned i = 1; i <= h.rows.size (); i ++)
    {
      if (h.rows[i] == 0)
	continue;
      
      assert (h.rows[i].card () == 1);
      
      pair<unsigned, R> p = h.rows[i].head ();
      ann[p.first] = p.second;
    }
  
  ptr<quotient_module<R> > Q = new quotient_module<R> (this);
  
  unsigned quot_r = 0;
  basedvector<linear_combination, 1> quot_rep;
  
  basedvector<unsigned, 1> generator_quot_gen (n);
  for (unsigned i = 1; i <= n; i ++)
    generator_quot_gen[i] = 0;
  
  for (unsigned i = 1; i <= n; i ++)
    {
      if (ann[i] == 0)
	{
	  quot_r ++;
	  quot_rep.append (h.generators[i]);
	  generator_quot_gen[i] = quot_r;
	}
    }
  
  unsigned quot_n = quot_r;
  basedvector<R, 1> quot_ann;
  for (unsigned i = 1; i <= n; i ++)
    {
      R a = ann[i];
      if (a != 0 && !a.is_unit ())
	{
	  quot_n ++;
	  quot_rep.append (h.generators[i]);
	  
	  assert (generator_quot_gen[i] == 0);
	  generator_quot_gen[i] = quot_n;
	  quot_ann.append (a);
	}
    }
  
  basedvector<map<unsigned, R>, 1> quot_pi (n);
  for (unsigned i = 1; i <= n; i ++)
    {
      map<unsigned, R> v;
      for (linear_combination_const_iter j = h.generators_inv[i]; j; j ++)
	{
	  unsigned qg = generator_quot_gen[j.key ()];
	  if (qg)
	    v.push (qg, j.val ());
	}
      quot_pi[i] = v;
    }
  
  assert (quot_n >= quot_r);
  assert (quot_rep.size () == quot_n);
  assert (quot_ann.size () == quot_n - quot_r);
  
  Q->rep = quot_rep;
  Q->pi = quot_pi;
  Q->ann = quot_ann;
  
  assert (Q->dim () == quot_n);
  assert (Q->free_rank () == quot_r);
  
  return Q;
}

template<class R> ptr<const quotient_module<R> > 
module<R>::quotient (ptr<const free_submodule<R> > m) const
{
  assert (m->parent_module () == this);
  
  // ??? duplicate
  mod_span<R> span (this, m->gens);
  return quotient (span);
}

template<class R> ptr<const free_submodule<R> >
module<R>::submodule (const Rmod_span &span) const
{
  assert (free_rank () == dim ());
  return new free_submodule<R> (this,
				span.gens,
				span.pivots);
}

template<class R> multivariate_laurentpoly<Z>
module<R>::free_poincare_polynomial () const
{
  multivariate_laurentpoly<Z> r;
  for (unsigned i = 1; i <= free_rank (); i ++)
    {
      grading hq = generator_grading (i);
      multivariate_laurent_monomial m;
      m.push_exponent (1, hq.h);
      m.push_exponent (2, hq.q);
      r.muladdeq (1, m);
    }
  return r;
}

template<class R> multivariate_laurentpoly<Z>
module<R>::free_delta_poincare_polynomial () const
{
  multivariate_laurentpoly<Z> r;
  for (unsigned i = 1; i <= free_rank (); i ++)
    {
      grading hq = generator_grading (i);
      multivariate_laurent_monomial m;
      m.push_exponent (1, hq.q - 2*hq.h);
      r.muladdeq (1, m);
    }
  return r;
}

template<class R> void
module<R>::show_self () const
{
  printf ("module/");
  R::show_ring ();
  printf (" %p %d", this, dim ());
}

template<class R> void
module<R>::display_self () const
{
  show_self (); newline ();
  
  printf ("  free_rank %d\n", free_rank ());
  
  for (unsigned i = 1; i <= dim (); i ++)
    {
      printf ("  %d ", i);
      show (generator_grading (i));
      printf (" ");
      show_generator (i);
      
      if (i > free_rank ())
	{
	  printf (": ");
	  R iann = generator_ann (i);
	  if (iann.is_unit ())
	    printf ("0\n");
	  else
	    {
	      R::show_ring ();
	      if (iann != 0)
		{
		  printf ("/");
		  show (iann);
		  R::show_ring ();
		}
	    }
	}
      newline ();
    }
}

template<class R> typename R::linear_combination
free_submodule<R>::inject (linear_combination v) const
{
  linear_combination r (parent);
  for (linear_combination_const_iter i = v; i; i ++)
    r.muladd (i.val (), gens[i.key ()]);
  return r;
}

template<class R> mod_map<R>
free_submodule<R>::injection_map () const
{
  return mod_map<R> (this, parent, gens);
}

template<class R> mod_map<R>
mod_map<R>::induced_map_to (ptr<const quotient_module<R> > new_to)
{
  assert (new_to->parent_module () == to);
  
  mod_map r (from, new_to);
  for (unsigned i = 1; i <= from->dim (); i ++)
    r[i] = new_to->project (columns[i]);
  return r;
}

template<class R> mod_map<R>
mod_map<R>::induced_map (ptr<const quotient_module<R> > new_fromto)
{
  assert (from == new_fromto->parent_module ());
  assert (to == new_fromto->parent_module ());
  
  // ??? doesn't check induced map is well-defined
  
  mod_map r (new_fromto, new_fromto);
  for (unsigned i = 1; i <= new_fromto->dim (); i ++)
    r[i] = new_fromto->project (map (new_fromto->generator_rep (i)));
  return r;
}

template<class R> mod_map<R> 
mod_map<R>::restrict_from (ptr<const free_submodule<R> > new_from) const
{
  assert (new_from->parent_module () == from);
  
  mod_map<R> r (new_from, to);
  for (unsigned i = 1; i <= new_from->dim (); i ++)
    r[i] = map (new_from->inject_generator (i));
  return r;
}

template<class R> mod_map<R> 
mod_map<R>::restrict_to (ptr<const free_submodule<R> > new_to) const
{
  assert (new_to->parent_module () == to);
  
  mod_map<R> r (from, new_to);
  for (unsigned i = 1; i <= from->dim (); i ++)
    r[i] = new_to->restrict (columns[i]);
  return r;
}

template<class R> mod_map<R> 
mod_map<R>::restrict (ptr<const free_submodule<R> > new_from,
		      ptr<const free_submodule<R> > new_to) const
{
  assert (new_from->parent_module () == from);
  assert (new_to->parent_module () == to);
  
  mod_map<R> r (new_from, new_to);
  for (unsigned i = 1; i <= new_from->dim (); i ++)
    r[i] = new_to->restrict (map (new_from->inject_generator (i)));
  return r;
}

template<class R> typename R::linear_combination
free_submodule<R>::restrict (linear_combination v0) const
{
  assert (v0.m == parent);
  
  linear_combination v (COPY, v0);
  
  linear_combination r (this);
  for (unsigned i = 1; i <= gens.size (); i ++)
    {
      unsigned j = pivots[i];
      R vc = v(j);
      if (vc != 0)
	{
	  const linear_combination &g = gens[i];
	  R gc = g(j);
	  
	  assert (gc | vc);
	  R q = vc.div (gc);
	  
	  v.mulsub (q, g);
	  r.muladd (q, i);
	}
    }
  assert (v == 0);
  assert (inject (r) == v0);
  
  return r;
}

template<class R> ptr<const free_submodule<R> > 
free_submodule<R>::restrict_submodule (ptr<const free_submodule<R> > m) const
{
  assert (m->parent == parent);
  
  basedvector<linear_combination, 1> span (m->dim ());
  for (unsigned i = 1; i <= m->dim (); i ++)
    span[i] = restrict (m->inject_generator (i));
  
  mod_span<R> span2 (this, span);
  return submodule (span2);
}

template<class R>
mod_map<R>::mod_map (ptr<const Rmod> fromto)
  : from(fromto),
    to(fromto),
    columns(fromto->dim ())
{
  for (unsigned i = 1; i <= from->dim (); i ++)
    columns[i] = linear_combination (to);
}

template<class R>
mod_map<R>::mod_map (ptr<const Rmod> fromto, int i)
  : from(fromto),
    to(fromto),
    columns(fromto->dim ())
{
  assert (i == 0 || i == 1);
  
  for (unsigned j = 1; j <= from->dim (); j ++)
    columns[j] = linear_combination (to);
  if (i == 1)
    {
      for (unsigned i = 1; i <= fromto->dim (); i ++)
	columns[i] += i;
    }
}

template<class R>
mod_map<R>::mod_map (ptr<const Rmod> from_, ptr<const Rmod> to_)
  : from(from_),
    to(to_),
    columns(from_->dim ())
{
  for (unsigned i = 1; i <= from->dim (); i ++)
    columns[i] = linear_combination (to);
}

template<class R> typename R::linear_combination
mod_map<R>::map (const linear_combination &lc) const
{
  assert (lc.m == from);
  
  linear_combination r (to);
  for (linear_combination_const_iter i = lc; i; i ++)
    r.muladd (i.val (), columns[i.key ()]);
  return r;
}

template<class R> bool
mod_map<R>::homogeneous () const
{
  for (unsigned i = 1; i <= from->dim (); i ++)
    {
      if (columns[i] != 0)
	{
	  grading dhq = columns[i].hq () - from->generator_grading (i);
	  for (unsigned j = i + 1; j <= from->dim (); j ++)
	    {
	      if (columns[j] != 0
		  && dhq != columns[j].hq () - from->generator_grading (j))
		return 0;
	    }
	  return 1;
	}
    }
  return 1;
}

template<class R> void
mod_map<R>::check_grading (grading delta) const
{
  for (unsigned i = 1; i <= from->dim (); i ++)
    {
      if (columns[i] != 0)
	assert (columns[i].hq () - from->generator_grading (i) == delta);
    }
}

template<class R> mod_map<R> 
mod_map<R>::compose (const mod_map<R> &m2) const
{
  assert (m2.to == from);
  
  basedvector<linear_combination, 1> r (m2.from->dim ());
  for (unsigned i = 1; i <= m2.from->dim (); i ++)
    r[i] = map (m2[i]);
  return mod_map (m2.from, to, r);
}

template<class R> mod_map<R> &
mod_map<R>::operator += (const mod_map &m2)
{
  assert (from == m2.from && to == m2.to);
  
  for (unsigned i = 1; i <= from->dim (); i ++)
    columns[i] += m2.columns[i];
  return *this;
}

template<class R> mod_map<R>
mod_map<R>::operator + (const mod_map &m2) const
{
  assert (from == m2.from && to == m2.to);
  
  basedvector<linear_combination, 1> r (from->dim ());
  for (unsigned i = 1; i <= m2.from->dim (); i ++)
    r[i] = columns[i] + m2.columns[i];
  return mod_map (from, to, r);
}

template<class R> mod_map<R> &
mod_map<R>::operator *= (R c)
{
  for (unsigned i = 1; i <= from->dim (); i ++)
    columns[i] *= c;
  return *this;
}

template<class R> ptr<const free_submodule<R> > 
mod_map<R>::kernel () const
{
  basedvector<linear_combination, 1> from_xs (from->dim ());
  for (unsigned i = 1; i <= to->dim (); i ++)
    {
      linear_combination x (from);
      x.muladd (1, i);
      from_xs[i] = x;
    }
  
  basedvector<linear_combination, 1> to_xs (COPY2, columns);
  
  for (unsigned i = 1; i <= to->dim (); i ++)
    {
      linear_combination from_v (from),
	to_v (to);
      
      for (unsigned j = 1; j <= to_xs.size (); j ++)
	{
	  R to_vc = to_v(i);
	  if (to_vc.is_unit ())
	    break;
	  
	  linear_combination &to_x = to_xs[j],
	    &from_x = from_xs[j];
	  R to_xc = to_x(i);
	  if (! (to_vc | to_xc))
	    {
	      triple<R, R, R> t = to_vc.extended_gcd (to_xc);
	      assert (t.first == to_vc*t.second + t.third*to_xc);
	      
	      to_v = t.second*to_v + t.third*to_x;
	      from_v = t.second*from_v + t.third*from_x;
	      
	      assert (to_v(i) != 0);
	    }
	}
      
      R to_vc = to_v(i);
      if (to_vc != 0)
	{
	  for (unsigned j = 1; j <= to_xs.size (); j ++)
	    {
	      linear_combination &to_x = to_xs[j],
		&from_x = from_xs[j];
	      R to_xc = to_x(i);
	      if (to_xc != 0)
		{
		  assert (to_vc | to_xc);
		  
		  R q = to_xc.div (to_vc);
		  to_x.mulsub (q, to_v);
		  from_x.mulsub (q, from_v);
		}
	      
	      assert (to_x(i) == 0);
	    }
	}
    }
  
  mod_span<R> span (from, from_xs);
  return from->submodule (span);
}

template<class R> ptr<const free_submodule<R> >
mod_map<R>::image () const
{
  mod_span<R> span (to, columns);
  return to->submodule (span);
}

template<class R> ptr<const quotient_module<R> > 
mod_map<R>::cokernel () const
{
  mod_span<R> span (to, columns);
  return to->quotient (span);
}

template<class R> ptr<const quotient_module<R> >
mod_map<R>::homology () const
{
  ptr<const free_submodule<R> > ker = kernel (),
    im = image ();
  
  // display ("ker:", *ker);
  // display ("im:", *im);
  
  ptr<const free_submodule<R> > im2 = ker->restrict_submodule (im);
  
  // display ("im2:", *im2);
  
  return ker->quotient (im2);
}

template<class R>
mod_span<R>::mod_span (ptr<const module<R> > mod,
		       basedvector<linear_combination, 1> xs0)
{
  assert (mod->free_rank () == mod->dim ());
  
  basedvector<linear_combination, 1> xs (COPY2, xs0);
  
  for (unsigned i = 1; i <= mod->dim (); i ++)
    {
      linear_combination v (mod);
      
      for (unsigned j = 1; j <= xs.size (); j ++)
	{
	  R vc = v(i);
	  if (vc.is_unit ())
	    break;
	  
	  linear_combination &x = xs[j];
	  R xc = x(i);
	  
	  if (! (vc | xc))
	    {
	      triple<R, R, R> t = vc.extended_gcd (xc);
	      assert (t.first == vc*t.second + t.third*xc);
	      
	      v = t.second*v + t.third*x;
	      
	      assert (v(i) != 0);
	    }
	}
      
      R vc = v(i);
      if (vc != 0)
	{
	  for (unsigned j = 1; j <= xs.size (); j ++)
	    {
	      linear_combination &x = xs[j];
	      R xc = x(i);
	      if (xc != 0)
		{
		  assert (vc | xc);
		  
		  R q = xc.div (vc);
		  x.mulsub (q, v);
		}
	      
	      assert (x(i) == 0);
	    }
	  
	  pivots.append (i);
	  gens.append (v);
	}
    }
}

template<class R> void
mod_map<R>::show_self () const
{
  printf ("mod_map ");
  show (*from);
  printf (" -> ");
  show (*to);
}

template<class R> void
mod_map<R>::display_self () const
{
  show_self (); newline ();
  for (unsigned i = 1; i <= from->dim (); i ++)
    {
      printf ("  %d: ", i);
      show (columns[i]);
      newline ();
    }
}
