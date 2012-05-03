
template<class R> class mod_map;

template<class R> class mod_span;

template<class R> class free_submodule;
template<class R> class quotient_module;
template<class R> class direct_sum;
template<class R> class tensor_product;
template<class R> class hom_module;

/* `module' is a bigraded module over a ring R. */
template<class R>
class module : public refcounted
{
 private:
  friend class reader;
  friend class writer;
  
  unsigned id;
  
  static unsigned id_counter;
  
  static basedvector<ptr<const module<R> >, 1> id_module;
  
  static map<basedvector<unsigned, 1>,
    ptr<const direct_sum<R> > > direct_sum_idx;
  static map<basedvector<unsigned, 1>,
    ptr<const tensor_product<R> > > tensor_product_idx;
  static map<pair<unsigned, unsigned>,
    ptr<const hom_module<R> > > hom_module_idx;
  
 public:
  module ()
  {
    id_counter ++;
    id = id_counter;
    
    id_module.append (this);
    assert (id_module.size () == id_counter);
  }
  module (const module &) = delete;
  virtual ~module () { }
  
  module &operator = (const module &) = delete;
  
 public:
  // the number of generators; n
  virtual unsigned dim () const = 0;
  
  // r; 1 <= r <= n
  virtual unsigned free_rank () const = 0;
  
  virtual grading generator_grading (unsigned i) const = 0;
  virtual void show_generator (unsigned i) const = 0;
  
  // r < i <= n
  virtual R generator_ann (unsigned i) const = 0;
  
  set<grading> gradings () const;
  
  bool is_free () const { return dim () == free_rank (); }
  
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
  
  ptr<const quotient_module<R> > quotient (const mod_span<R> &span) const;
  ptr<const quotient_module<R> > quotient (ptr<const free_submodule<R> > m) const;
  
  ptr<const free_submodule<R> > submodule (const mod_span<R> &span) const;
  
  multivariate_laurentpoly<Z> free_poincare_polynomial () const;
  multivariate_laurentpoly<Z> free_delta_poincare_polynomial () const;
  
  ptr<const direct_sum<R> >
    add (basedvector<ptr<const module<R> >, 1> compound_summands) const;
  
  ptr<const direct_sum<R> > add (ptr<const module<R> > m) const
  {
    basedvector<ptr<const module<R> >, 1> summands (2);
    summands[1] = this;
    summands[2] = m;
    return add (summands);
  }
  
  // g -> (g, 0)
  unsigned inject_1 (unsigned g, ptr<const module<R> > m) const
  {
    return g;
  }
  
  // g -> (0, g)
  unsigned inject_2 (ptr<const module<R> > m, unsigned g) const
  {
    return dim () + g;
  }
  
  pair<unsigned, unsigned> project (ptr<const module<R> > m, unsigned g) const
  {
    if (g <= dim ())
      return pair<unsigned, unsigned> (1, g);
    else
      return pair<unsigned, unsigned> (2, g - dim ());
  }
  
  ptr<const tensor_product<R> > tensor (ptr<const module<R> > m) const
  {
    basedvector<ptr<const module<R> >, 1> factors (2);
    factors[1] = this;
    factors[2] = m;
    return tensor (factors);
  }
  
  ptr<const hom_module<R> > hom (ptr<const module<R> > to) const;
  
  pair<unsigned, unsigned>
  generator_factors (ptr<const module<R> > m, unsigned g) const
  {
    pair<unsigned, unsigned> p ((g - 1) % dim () + 1,
				(g - 1) / dim () + 1);
    assert (g == tensor_generators (p.first, m, p.second));
    return p;
  }
  
  static ptr<const tensor_product<R> > tensor (basedvector<ptr<const module<R> >, 1> compound_factors);
  
  unsigned tensor_generators (unsigned i, ptr<const module<R> > m, unsigned j) const
  {
    return (i - 1) + (j - 1) * dim () + 1;
  }
  
  virtual void append_direct_summands (basedvector<ptr<const module<R> >, 1> &summands) const
  {
    summands.append (this);
  }
  
  virtual void append_tensor_factors (basedvector<ptr<const module<R> >, 1> &factors) const
  {
    factors.append (this);
  }
  
  ptr<const free_submodule<R> > graded_piece (grading hq) const;
  
  void write_self (writer &w) const { w.write_mod<R> (this); }
  void show_self () const;
  void display_self () const;
};

template<class R> unsigned module<R>::id_counter = 0;

template<class R> basedvector<ptr<const module<R> >, 1> module<R>::id_module;

template<class R> map<basedvector<unsigned, 1>,
  ptr<const direct_sum<R> > > module<R>::direct_sum_idx;

template<class R> map<basedvector<unsigned, 1>,
  ptr<const tensor_product<R> > > module<R>::tensor_product_idx;

template<class R> map<pair<unsigned, unsigned>,
  ptr<const hom_module<R> > > module<R>::hom_module_idx;

template<class R>
class direct_sum : public module<R>
{
  unsigned n;
  basedvector<ptr<const module<R> >, 1> summands;
  
 public:
  direct_sum (basedvector<ptr<const module<R> >, 1> summands_)
    : n(0),
      summands(summands_)
  {
#ifndef NDEBUG
    for (unsigned i = 1; i <= summands.size (); i ++)
      assert (summands[i]->is_free ());
#endif
    
    for (unsigned i = 1; i <= summands.size (); i ++)
      n += summands[i]->dim ();
  }
  ~direct_sum () { }
  
  unsigned dim () const { return n; }
  unsigned free_rank () const { return n; }
  
  grading generator_grading (unsigned i) const;
  void show_generator (unsigned i) const;
  R generator_ann (unsigned i) const { return R (0); }
  
  void append_direct_summands (basedvector<ptr<const module<R> >, 1> &psummands) const
  {
    for (unsigned i = 1; i <= summands.size (); i ++)
      psummands.append (summands[i]);
  }
  
  unsigned inject (unsigned i, unsigned g) const;
  pair<unsigned, unsigned> project (unsigned g) const;
};

template<class R> grading
direct_sum<R>::generator_grading (unsigned i) const
{
  pair<unsigned, unsigned> p = project (i);
  return summands[p.first]->generator_grading (p.second);
}

template<class R> void
direct_sum<R>::show_generator (unsigned i) const
{
  pair<unsigned, unsigned> p = project (i);
  printf ("%d:", p.first);
  return summands[p.first]->show_generator (p.second);
}

template<class R> unsigned
direct_sum<R>::inject (unsigned i, unsigned g) const
{
  assert (i >= 1 && i <= summands.size ());
  
  for (unsigned j = 1; j < i; j ++)
    g += summands[j]->dim ();
  return g;
}

template<class R> pair<unsigned, unsigned>
direct_sum<R>::project (unsigned g) const
{
  assert (g <= n);
  
  unsigned g0 = g;
  
  for (unsigned j = 1; j <= summands.size (); j ++)
    {
      if (g <= summands[j]->dim ())
	{
	  assert (inject (j, g) == g0);
	  
	  return pair<unsigned, unsigned> (j, g);
	}
      else
	g -= summands[j]->dim ();
    }
  abort ();  // shouldn't get here
}

template<class R> ptr<const direct_sum<R> >
module<R>::add (basedvector<ptr<const module<R> >, 1> compound_summands) const
{
  basedvector<ptr<const module<R> >, 1> summands;
  for (unsigned i = 1; i <= compound_summands.size (); i ++)
    compound_summands[i]->append_direct_summands (summands);
  
  basedvector<unsigned, 1> summand_ids (summands.size ());
  for (unsigned i = 1; i <= summands.size (); i ++)
    summand_ids[i] = summands[i]->id;
  
  pair<ptr<const direct_sum<R> > &, bool> p = direct_sum_idx.find (summand_ids);
  if (!p.second)
    p.first = new direct_sum<R> (summands);
  return p.first;
}

template<class R>
class tensor_product : public module<R>
{
  unsigned n;
  basedvector<ptr<const module<R> >, 1> factors;
  
  basedvector<unsigned, 1> generator_factors (unsigned g) const;
  
 public:
  tensor_product (basedvector<ptr<const module<R> >, 1> factors_)
    : n(1),
      factors(factors_)
  {
#ifndef NDEBUG
    for (unsigned i = 1; i <= factors.size (); i ++)
      assert (factors[i]->is_free ());
#endif
    
    for (unsigned i = 1; i <= factors.size (); i ++)
      n *= factors[i]->dim ();
  }
  ~tensor_product () { }
  
  unsigned dim () const { return n; }
  unsigned free_rank () const { return n; }
  
  grading generator_grading (unsigned i) const;
  void show_generator (unsigned i) const;
  R generator_ann (unsigned i) const { return R (0); }
  
  unsigned tensor_generators (basedvector<unsigned, 1> gs) const;
  
  void append_tensor_factors (basedvector<ptr<const module<R> >, 1> &pfactors) const
  {
    for (unsigned i = 1; i <= factors.size (); i ++)
      pfactors.append (factors[i]);
  }
};

template<class R> ptr<const tensor_product<R> >
module<R>::tensor (basedvector<ptr<const module<R> >, 1> compound_factors)
{
  basedvector<ptr<const module<R> >, 1> factors;
  for (unsigned i = 1; i <= compound_factors.size (); i ++)
    compound_factors[i]->append_tensor_factors (factors);
  
  basedvector<unsigned, 1> factor_ids (factors.size ());
  for (unsigned i = 1; i <= factors.size (); i ++)
    factor_ids[i] = factors[i]->id;
  
  pair<ptr<const tensor_product<R> > &, bool> p = tensor_product_idx.find (factor_ids);
  if (!p.second)
    p.first = new tensor_product<R> (factors);
  return p.first;
}

template<class R> grading
tensor_product<R>::generator_grading (unsigned i) const
{
  basedvector<unsigned, 1> gs = generator_factors (i);
  assert (gs.size () == factors.size ());
  
  grading gr;
  for (unsigned i = 1; i <= factors.size (); i ++)
    gr += factors[i]->generator_grading (gs[i]);
  
  return gr;
}

template<class R> void
tensor_product<R>::show_generator (unsigned i) const
{
  basedvector<unsigned, 1> gs = generator_factors (i);
  assert (gs.size () == factors.size ());
  
  printf ("o(");
  for (unsigned i = 1; i <= factors.size (); i ++)
    {
      if (i > 1)
	printf (",");
      factors[i]->show_generator (gs[i]);
    }
  printf (")");
}

template<class R> unsigned
tensor_product<R>::tensor_generators (basedvector<unsigned, 1> gs) const
{
  assert (gs.size () == factors.size ());
  
  unsigned r = gs[gs.size ()] - 1;
  for (unsigned i = gs.size () - 1; i >= 1; i --)
    {
      r *= factors[i]->dim ();
      r += gs[i] - 1;
    }
  r ++;
  
  return r;
}

template<class R> basedvector<unsigned, 1>
tensor_product<R>::generator_factors (unsigned g) const
{
  basedvector<unsigned, 1> r (factors.size ());
  
  unsigned g0 = g;
  
  g --;
  for (unsigned i = 1; i <= factors.size (); i ++)
    {
      r[i] = (g % factors[i]->dim ()) + 1;
      g /= factors[i]->dim ();
    }
  assert (g == 0);
  assert (tensor_generators (r) == g0);
  
  return r;
}

template<class R> 
class hom_module : public module<R>
{
 public:
  unsigned n;
  ptr<const module<R> > from;
  ptr<const module<R> > to;
  
 public:
  hom_module (ptr<const module<R> > from_,
	      ptr<const module<R> > to_)
    : from(from_), to(to_)
  {
    assert (from->is_free ()
	    && to->is_free ());
    n = from->dim () * to->dim ();
  }
  ~hom_module () { }
  
  // e_ij -> ij
  pair<unsigned, unsigned> generator_indices (unsigned g) const
  {
    unsigned d = from->dim ();
    unsigned g0 = g;
    
    g --;
    pair<unsigned, unsigned> p ((g % d) + 1,
				(g / d) + 1);
    
    assert (generator (p.first, p.second) == g0);
    
    return p;
  }
  
  // ij -> e_ij 
  unsigned generator (unsigned i, unsigned j) const
  {
    return (i - 1) + (j - 1) * from->dim () + 1;
  }
  
  unsigned dim () const { return n; }
  unsigned free_rank () const { return n; }
  
  grading generator_grading (unsigned i) const;
  void show_generator (unsigned i) const;
  R generator_ann (unsigned i) const { return R (0); }
  
  linear_combination<R> map_as_element (const mod_map<R> &m) const;
};

template<class R> ptr<const hom_module<R> >
module<R>::hom (ptr<const module<R> > to) const
{
  pair<ptr<const hom_module<R> > &, bool> p = hom_module_idx.find (pair<unsigned, unsigned>
								   (id, to->id));
  if (!p.second)
    p.first = new hom_module<R> (this, to);
  return p.first;
}

template<class R> grading
hom_module<R>::generator_grading (unsigned i) const
{
  pair<unsigned, unsigned> p = generator_indices (i);
  return (to->generator_grading (p.second)
	  - from->generator_grading (p.first));
}

template<class R> void
hom_module<R>::show_generator (unsigned i) const
{
  pair<unsigned, unsigned> p = generator_indices (i);
  
  printf ("(");
  from->show_generator (p.first);
  printf (" -> ");
  to->show_generator (p.second);
  printf (")");
}

template<class R, class G>
class base_module : public module<R>
{
 private:
  G g;
  
 public:
  base_module () = delete;
  base_module (const G &g_) : g(g_) { }
  ~base_module () { }
  
  base_module &operator = (const base_module &) = delete;
  
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
  explicit_module () = delete;
  explicit_module (unsigned r_,
		   basedvector<R, 1> ann_,
		   basedvector<grading, 1> hq_)
    : r(r_), ann(ann_), hq(hq_)
  { }
  
  explicit explicit_module (unsigned r_, basedvector<grading, 1> hq_) : r(r_), hq(hq_) { }
  ~explicit_module () { }
  
  explicit_module &operator = (const explicit_module &) = delete;
  
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
  
  ptr<const module<R> > parent;
  basedvector<linear_combination<R>, 1> gens;
  basedvector<unsigned, 1> pivots;
  
 public:
  free_submodule (ptr<const module<R> > parent_,
		  basedvector<linear_combination<R>, 1> gens_,
		  basedvector<unsigned, 1> pivots_)
    : parent(parent_),
      gens(gens_),
      pivots(pivots_)
  { }
  ~free_submodule () { }
  
  free_submodule &operator = (const free_submodule &) = delete;
  
  ptr<const module<R> > parent_module () const { return parent; }
  
  unsigned dim () const { return gens.size (); }
  unsigned free_rank () const { return gens.size (); }
  grading generator_grading (unsigned i) const { return gens[i].hq (); }
  void show_generator (unsigned i) const { show (gens[i]); }
  R generator_ann (unsigned i) const { abort (); }
  
  linear_combination<R> inject_generator (unsigned i) const { return gens[i]; }
  linear_combination<R> inject (linear_combination<R> v) const;
  
  mod_map<R> injection_map () const;
  
  linear_combination<R> restrict (linear_combination<R> v0) const;
  ptr<const free_submodule<R> > restrict_submodule (ptr<const free_submodule<R> > m) const;
  
  ptr<const free_submodule<R> > intersection (ptr<const free_submodule<R> > m) const;
  ptr<const free_submodule<R> > plus (ptr<const free_submodule<R> > m) const;
};

template<class R> 
class quotient_module : public module<R>
{
  friend class module<R>;
  
  ptr<const module<R> > parent;
  
  // note: these get filled in by module::quotient
  
  basedvector<R, 1> ann;
  
  // parent lc representing i
  basedvector<linear_combination<R>, 1> rep;
  
  // map from parent generator to lc in quotient
  basedvector<map<unsigned, R>, 1> pi;
  
 public:
  quotient_module (const quotient_module &) = delete;
  quotient_module (ptr<const module<R> > parent_)
    : parent(parent_)
  { }
  ~quotient_module () { }
  
  quotient_module &operator = (const quotient_module &) = delete;
  
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
  
  linear_combination<R> project_generator (unsigned i) const
  {
    assert (i >= 1 && i <= parent->dim ());
    linear_combination<R> r (this);
    for (typename map<unsigned, R>::const_iter j = pi[i]; j; j ++)
      r.muladd (j.val (), j.key ());
    return r;
  }
  
  linear_combination<R> project (linear_combination<R> v) const
  {
    assert (v.m == parent);
    linear_combination<R> r (this);
    for (linear_combination_const_iter<R> i = v; i; i ++)
      r.muladd (i.val (), project_generator (i.key ()));
    return r;
  }
  
  linear_combination<R> generator_rep (unsigned i) const { return rep[i]; }
};

template<class R>
class map_impl : public refcounted
{
 public:
  ptr<const module<R> > from;
  ptr<const module<R> > to;
  
 public:
  map_impl (const map_impl &) = delete;
  map_impl (ptr<const module<R> > fromto)
    : from(fromto), to(fromto)
  { }
  map_impl (ptr<const module<R> > from_, ptr<const module<R> > to_)
    : from(from_), to(to_)
  { }
  virtual ~map_impl () { }
  
  map_impl &operator = (const map_impl &) = delete;
  
  virtual const linear_combination<R> column (unsigned i) const = 0;
  virtual const linear_combination<R> column_copy (unsigned i) const { return column (i); }
  
  linear_combination<R> map (const linear_combination<R> &lc) const
  {
    linear_combination<R> r (this->to);
    for (linear_combination_const_iter<R> i = lc; i; i ++)
      r.muladd (i.val (), column (i.key ()));
    return r;
  };
};

template<class R>
class explicit_map_impl : public map_impl<R>
{
  basedvector<linear_combination<R>, 1> columns;
  
 public:
  explicit_map_impl (ptr<const module<R> > fromto,
	    basedvector<linear_combination<R>, 1> columns_)
    : map_impl<R>(fromto),
      columns(columns_)
  { }
  explicit_map_impl (ptr<const module<R> > from, ptr<const module<R> > to,
	    basedvector<linear_combination<R>, 1> columns_)
    : map_impl<R>(from, to),
      columns(columns_)
  { }
  ~explicit_map_impl () { }
  
  const linear_combination<R> column (unsigned i) const { return columns[i]; }
  const linear_combination<R> column_copy (unsigned i) const
  {
    return linear_combination<R> (COPY, columns[i]);
  }
};

template<class R>
class zero_map_impl : public map_impl<R>
{
 public:
  zero_map_impl (ptr<const module<R> > fromto) : map_impl<R>(fromto) { }
  zero_map_impl (ptr<const module<R> > from, ptr<const module<R> > to) : map_impl<R>(from, to) { }
  
  const linear_combination<R> column (unsigned i) const { return linear_combination<R> (this->to); }
};

template<class R>
class id_map_impl : public map_impl<R>
{
 public:
  id_map_impl (ptr<const module<R> > fromto) : map_impl<R>(fromto) { }
  id_map_impl (ptr<const module<R> > from, ptr<const module<R> > to) : map_impl<R>(from, to) { }
  
  const linear_combination<R> column (unsigned i) const
  {
    linear_combination<R> r (this->to);
    r.muladd (1, i);
    return r;
  }
};

template<class R>
class composition_impl : public map_impl<R>
{
  // f(g(x))
  ptr<const map_impl<R> > f, g;
  
 public:
  composition_impl (ptr<const map_impl<R> > f_, ptr<const map_impl<R> > g_)
    : map_impl<R>(g_->from, f_->to),
      f(f_),
      g(g_)
  {
    assert (g->to == f->from);
  }
  
  const linear_combination<R> column (unsigned i) const
  {
    return f->map (g->column (i));
  }
};

template<class R>
class direct_sum_impl : public map_impl<R>
{
  // f\oplus g
  ptr<const map_impl<R> > f, g;
  
 public:
  direct_sum_impl (ptr<const map_impl<R> > f_, ptr<const map_impl<R> > g_)
    : map_impl<R>(f_->from->add (g_->from),
		  f_->to->add (g_->to)),
      f(f_),
      g(g_)
  {
  }
  
  const linear_combination<R> column (unsigned i) const
  {
    pair<unsigned, unsigned> p = f->from->project (g->from, i);
    
    linear_combination<R> r (this->to);
    if (p.first == 1)
      {
	for (linear_combination_const_iter<R> j = f->column (p.second); j; j ++)
	  r.muladd (j.val (), f->to->inject_1 (j.key (), g->to));
      }
    else
      {
	assert (p.first == 2);
	for (linear_combination_const_iter<R> j = g->column (p.second); j; j ++)
	  r.muladd (j.val (), f->to->inject_2 (g->to, j.key ()));
      }
    return r;
  }
};

template<class R>
class tensor_impl : public map_impl<R>
{
  // f\otimes g
  ptr<const map_impl<R> > f, g;
  
 public:
  tensor_impl (ptr<const map_impl<R> > f_, ptr<const map_impl<R> > g_)
    : map_impl<R>(f_->from->tensor (g_->from),
		  f_->to->tensor (g_->to)),
      f(f_),
      g(g_)
  {
  }
  
  const linear_combination<R> column (unsigned i) const
  {
    pair<unsigned, unsigned> p = f->from->generator_factors (g->from, i);
    
    // ??
    return f->column (p.first).tensor (g->column (p.second));
  }
};

template<class R>
class map_builder
{
 public:
  ptr<const module<R> > from, to;
  basedvector<linear_combination<R>, 1> columns;
  
  void init ();
  
 public:
  map_builder (ptr<const module<R> > fromto) : from(fromto), to(fromto) { init (); }
  map_builder (ptr<const module<R> > fromto, int i) : from(fromto), to(fromto)
  {
    init ();
    if (i == 1)
      {
	for (unsigned i = 1; i <= from->dim (); i ++)
	  columns[i].muladd (1, i);
      }
    else
      assert (i == 0);
    
  }
  
  map_builder (ptr<const module<R> > from_, ptr<const module<R> > to_)
    : from(from_), to(to_)
  {
    init ();
  }
  
  linear_combination<R> &operator [] (unsigned i) { return columns[i]; }
  const linear_combination<R> &operator [] (unsigned i) const { return columns[i]; }
};

template<class R> void
map_builder<R>::init ()
{
  columns.resize (from->dim ());
  for (unsigned i = 1; i <= from->dim (); i ++)
    columns[i] = linear_combination<R> (to);
}

template<class R>
class mod_map
{
  // ???
  enum impl_ctor { IMPL };

  ptr<const map_impl<R> > impl;
  
  mod_map (impl_ctor, ptr<const map_impl<R> > impl_) : impl(impl_) { }
  
 public:
  mod_map () { }
  mod_map (const mod_map &m) : impl(m.impl) { }
  
  mod_map (ptr<const module<R> > fromto) : impl(new zero_map_impl<R>(fromto)) { }
  mod_map (ptr<const module<R> > fromto, int i)
  {
    if (i == 1)
      impl = new id_map_impl<R> (fromto);
    else
      {
	assert (i == 0);
	impl = new zero_map_impl<R> (fromto);
      }
  }
  
  mod_map (ptr<const module<R> > from, ptr<const module<R> > to)
    : impl(new zero_map_impl<R> (from, to))
  { }
  
  mod_map (ptr<const module<R> > fromto,
	   basedvector<linear_combination<R>, 1> columns)
    : impl(new explicit_map_impl<R> (fromto, columns))
  { }
  mod_map (ptr<const module<R> > from,
	   ptr<const module<R> > to,
	   basedvector<linear_combination<R>, 1> columns)
    : impl(new explicit_map_impl<R> (from, to, columns))
  { }
  mod_map (const map_builder<R> &b)
    : impl(new explicit_map_impl<R> (b.from, b.to, b.columns))
  { }
  
  mod_map (reader &r)
  {
    ptr<const module<R> > from = r.read_mod<R> ();
    ptr<const module<R> > to = r.read_mod<R> ();
    basedvector<linear_combination<R>, 1> columns (r);
    impl = new explicit_map_impl<R> (from, to, columns);
  }
  
  ~mod_map () { }
  
  mod_map &operator = (const mod_map &m) { impl = m.impl; return *this; }
  
  ptr<const module<R> > domain () const { return impl->from; }
  ptr<const module<R> > codomain () const { return impl->to; }
  
  bool operator == (const mod_map &m) const
  {
    assert (impl->from == m.impl->from);
    assert (impl->to == m.impl->to);
    for (unsigned i = 1; i <= impl->from->dim (); i ++)
      {
	if (impl->column (i) != m.impl->column (i))
	  return 0;
      }
    return 1;
  }
  bool operator != (const mod_map &m) const { return !operator == (m); }
  
  bool operator == (int x) const
  {
    R c (x);
    assert (c == 0);
    
    for (unsigned i = 1; i <= impl->from->dim (); i ++)
      {
	if (impl->column (i) != 0)
	  return 0;
      }
    return 1;
  }
  
  bool operator != (int x) const { return !operator == (x); }
  
  const linear_combination<R> column (unsigned i) const { return impl->column (i); }
  const linear_combination<R> operator [] (unsigned i) const { return impl->column (i); }
  
  const linear_combination<R> column_copy (unsigned i) const { return impl->column_copy (i); }
  
  linear_combination<R> map (const linear_combination<R> &lc) const { return impl->map (lc); }
  mod_map compose (const mod_map &m) const
  {
    return mod_map (IMPL,
		    new composition_impl<R> (impl, m.impl));
  }
  
  // ??? in the sense of direct sum
  mod_map add (const mod_map &m) const
  {
    return mod_map (IMPL,
		    new direct_sum_impl<R> (impl, m.impl));
  }
  
  mod_map tensor (const mod_map &m) const
  {
    return mod_map (IMPL,
		    new tensor_impl<R> (impl, m.impl));
  }
  
  // ?? add and other map operations should not be explicit
  mod_map operator + (const mod_map &m) const;
  
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
  
  // ???
  basedvector<linear_combination<R>, 1> explicit_columns () const;
  
  void write_self (writer &w) const
  {
    // write explicitly
    write (w, *impl->from);
    write (w, *impl->to);
    write (w, explicit_columns ());
  }
  
  void show_self () const;
  void display_self () const;
};

template<class R> linear_combination<R> 
hom_module<R>::map_as_element (const mod_map<R> &m) const
{
  assert (from == m.domain ()
	  && to == m.codomain ());
  
  linear_combination<R> r (this);
  for (unsigned i = 1; i <= from->dim (); i ++)
    {
      for (linear_combination_const_iter<R> j = m.column (i); j; j ++)
	r.muladd (j.val (), generator (i, j.key ()));
    }
  return r;
}

template<class R>
class mod_span
{
 public:
  basedvector<linear_combination<R>, 1> gens;
  basedvector<unsigned, 1> pivots;
  
 public:
  mod_span (ptr<const module<R> > mod, basedvector<linear_combination<R>, 1> xs);
  ~mod_span () { }
};

template<class R>
class quotient_helper
{
 public:
  ptr<const module<R> > mod;
  
  // rows of the presentation matrix
  basedvector<linear_combination<R>, 1> rows;
  
  basedvector<linear_combination<R>, 1> generators;
  basedvector<linear_combination<R>, 1> generators_inv;
  
  bool improve_pivot_row (unsigned i, unsigned j, unsigned i2);
  bool improve_pivot_column (unsigned i, unsigned j, unsigned j2);
  void improve_pivot (unsigned i, unsigned j);
  
 public:
  quotient_helper (ptr<const module<R> > mod_, basedvector<linear_combination<R>, 1> rows);
  
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
				     basedvector<linear_combination<R>, 1> rows_)
  : mod(mod_),
    rows(rows_),
    generators(mod->dim ()),
    generators_inv(mod->dim ())
{
  assert (mod->dim () == mod->free_rank ());
  
  for (unsigned i = 1; i <= mod->dim (); i ++)
    {
      linear_combination<R> v (mod);
      v.muladd (1, i);
      generators[i] = v;
      
      linear_combination<R> vinv (mod);
      vinv.muladd (1, i);
      generators_inv[i] = vinv;
    }
}

template<class R> bool
quotient_helper<R>::improve_pivot_row (unsigned i, unsigned j, unsigned i2)
{
  assert (i != i2);
  
  const linear_combination<R> &r = rows[i],
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
  
  tuple<R, R, R> t = rc.extended_gcd (r2c);
  assert (get<0> (t) == rc*get<1> (t) + get<2> (t)*r2c);
  
  rows[i] = r*get<1> (t) + r2*get<2> (t);
  rows[i2] = (rc.div (get<0> (t)))*r2 - (r2c.div (get<0> (t)))*r;
  
  assert ((rc | r2c) == rc.divides (get<0> (t)));
  assert (!rc.divides (get<0> (t)) || rows[i2](j) == 0);
  
  return !rc.divides (get<0> (t));
}

template<class R> bool
quotient_helper<R>::improve_pivot_column (unsigned i, unsigned j, unsigned j2)
{
  assert (j != j2);
  
#if 0
  basedvector<linear_combination<R>, 1> orig_row_image (rows.size ());
  for (unsigned k = 1; k <= rows.size (); k ++)
    {
      linear_combination<R> r (mod);
      for (linear_combination_const_iter<R> l = rows[k]; l; l ++)
	r.muladd (l.val (), generators[l.key ()]);
      orig_row_image[k] = r;
    }
#endif
  
  const linear_combination<R> &r = rows[i];
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
	  linear_combination<R> &rk = rows[k];
	  rk.mulsub (rk(j) * q, j2);
	}
      
      assert (r(j2) == 0);
      
      return 0;
    }
#endif
  
  tuple<R, R, R> t = rc.extended_gcd (rc2);
  assert (get<0> (t) == rc*get<1> (t) + get<2> (t)*rc2);
  
  for (unsigned k = 1; k <= rows.size (); k ++)
    {
      linear_combination<R> &rk = rows[k];
      R rkc = rk(j),
	rkc2 = rk(j2);
      
      rk.set_coeff (rkc*get<1> (t) + rkc2*get<2> (t),
		    j);
      rk.set_coeff (rkc2*(rc.div (get<0> (t))) - rkc*(rc2.div (get<0> (t))),
		    j2);
    }
  
  linear_combination<R> g = generators[j],
    g2 = generators[j2];
  
  assert (g.hq () == g2.hq ());
  
  generators[j] = (rc.div (get<0> (t))) * g + (rc2.div (get<0> (t))) * g2;
  generators[j2] = get<1> (t) * g2 - get<2> (t) * g;
  
 #if 0
  for (unsigned k = 1; k <= rows.size (); k ++)
    {
      linear_combination<R> r2 (mod);
      for (linear_combination_const_iter<R> l = rows[k]; l; l ++)
	r2.muladd (l.val (), generators[l.key ()]);
      assert (r2 == orig_row_image[k]);
    }
#endif
  
  for (unsigned k = 1; k <= mod->dim (); k ++)
    {
      linear_combination<R> &ginv = generators_inv[k];
      
      R d = ginv(j),
	d2 = ginv(j2);
      
      ginv.set_coeff (get<1> (t)*d + get<2> (t)*d2, j);
      ginv.set_coeff (rc.div (get<0> (t)) * d2 - rc2.div (get<0> (t)) * d, j2);
    }
  
#if 0
  for (unsigned k = 1; k <= mod->dim (); k ++)
    {
      linear_combination<R> r (mod);
      r.muladd (1, k);
      
      linear_combination<R> r2 (mod);
      for (linear_combination_const_iter<R> l = generators_inv[k]; l; l ++)
	r2.muladd (l.val (), generators[l.key ()]);
      
      assert (r == r2);
    }
#endif
  
  assert ((rc | rc2) == rc.divides (get<0> (t)));
  assert (!rc.divides (get<0> (t)) || r(j2) == 0);
  
  return !rc.divides (get<0> (t));
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
      for (linear_combination_const_iter<R> k = rows[i]; k; k ++)
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
module<R>::quotient (const mod_span<R> &span) const
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
  basedvector<linear_combination<R>, 1> quot_rep;
  
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
      for (linear_combination_const_iter<R> j = h.generators_inv[i]; j; j ++)
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
module<R>::submodule (const mod_span<R> &span) const
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

template<class R> set<grading> 
module<R>::gradings () const
{
  set<grading> gs;
  for (unsigned i = 1; i <= dim (); i ++)
    gs += generator_grading (i);
  return gs;
}

template<class R> ptr<const free_submodule<R> > 
module<R>::graded_piece (grading hq) const
{
  basedvector<linear_combination<R>, 1> s;
  for (unsigned i = 1; i <= dim (); i ++)
    {
      grading ihq = generator_grading (i);
      if (ihq.h == hq.h
	  && ihq.q == hq.q)
	{
	  linear_combination<R> v (this);
	  v.muladd (1, i);
	  s.append (v);
	}
    }
  
  mod_span<R> span (this, s);
  return submodule (span);
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

template<class R> linear_combination<R>
free_submodule<R>::inject (linear_combination<R> v) const
{
  linear_combination<R> r (parent);
  for (linear_combination_const_iter<R> i = v; i; i ++)
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
  assert (new_to->parent_module () == impl->to);
  
  basedvector<linear_combination<R>, 1> v (impl->from->dim ());
  for (unsigned i = 1; i <= impl->from->dim (); i ++)
    v[i] = new_to->project (column (i));
  return new explicit_map_impl<R> (impl->from, new_to, v);
}

template<class R> mod_map<R>
mod_map<R>::induced_map (ptr<const quotient_module<R> > new_fromto)
{
  assert (impl->from == new_fromto->parent_module ());
  assert (impl->to == new_fromto->parent_module ());
  
  // ??? doesn't check induced map is well-defined
  
  basedvector<linear_combination<R>, 1> v (new_fromto->dim ());
  for (unsigned i = 1; i <= new_fromto->dim (); i ++)
    v[i] = new_fromto->project (map (new_fromto->generator_rep (i)));
  return new explicit_map_impl<R> (new_fromto, v);
}

template<class R> mod_map<R> 
mod_map<R>::restrict_from (ptr<const free_submodule<R> > new_from) const
{
  assert (new_from->parent_module () == impl->from);
  
  basedvector<linear_combination<R>, 1> v (new_from->dim ());
  for (unsigned i = 1; i <= new_from->dim (); i ++)
    v[i] = map (new_from->inject_generator (i));
  return mod_map (IMPL, new explicit_map_impl<R> (new_from, impl->to, v));
}

template<class R> mod_map<R> 
mod_map<R>::restrict_to (ptr<const free_submodule<R> > new_to) const
{
  assert (new_to->parent_module () == impl->to);
  
  basedvector<linear_combination<R>, 1> v (impl->from->dim ());
  for (unsigned i = 1; i <= impl->from->dim (); i ++)
    v[i] = new_to->restrict (column (i));
  return new explicit_map_impl<R> (impl->from, new_to, v);
}

template<class R> mod_map<R> 
mod_map<R>::restrict (ptr<const free_submodule<R> > new_from,
		      ptr<const free_submodule<R> > new_to) const
{
  assert (new_from->parent_module () == impl->from);
  assert (new_to->parent_module () == impl->to);
  
  basedvector<linear_combination<R>, 1> v (new_from->dim ());
  for (unsigned i = 1; i <= new_from->dim (); i ++)
    v[i] = new_to->restrict (map (new_from->inject_generator (i)));
  return mod_map (IMPL, new explicit_map_impl<R> (new_from, new_to, v));
}

template<class R> linear_combination<R>
free_submodule<R>::restrict (linear_combination<R> v0) const
{
  assert (v0.m == parent);
  
  linear_combination<R> v (COPY, v0);
  
  linear_combination<R> r (this);
  for (unsigned i = 1; i <= gens.size (); i ++)
    {
      unsigned j = pivots[i];
      R vc = v(j);
      if (vc != 0)
	{
	  const linear_combination<R> &g = gens[i];
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
  
  basedvector<linear_combination<R>, 1> span (m->dim ());
  for (unsigned i = 1; i <= m->dim (); i ++)
    span[i] = restrict (m->inject_generator (i));
  
  mod_span<R> span2 (this, span);
  return this->submodule (span2);
}

template<class R> ptr<const free_submodule<R> >
free_submodule<R>::intersection (ptr<const free_submodule<R> > m) const
{
  assert (parent == m->parent);
  
  unsigned md = m->dim (),
    d = dim ();
  
  basedvector<linear_combination<R>, 1> intr;
  
  basedvector<linear_combination<R>, 1> hperp,
    hproj;
  basedvector<unsigned, 1> hpivots;
  for (unsigned i = 1; i <= md; i ++)
    {
      linear_combination<R> perp (COPY, m->gens[i]),
	proj (parent);
      
      for (unsigned j = 1; j <= d; j ++)
	{
	  unsigned k = pivots[j];
	  if (perp % k)
	    {
	      const linear_combination<R> &g = gens[j];
	      R c = g(k);
	      R d = perp(k);
	      
	      assert (c | d);
	      R q = d.div (c);
	      
	      perp.mulsub (q, g);
	      proj.mulsub (q, g);
	      
	      assert (! (perp % k));
	    }
	}
      
      for (unsigned j = 1; j <= hpivots.size (); j ++)
	{
	  unsigned k = hpivots[j];
	  if (perp % k)
	    {
	      const linear_combination<R> &h = hperp[j];
	      R c = h(k);
	      R d = perp(k);
	      
	      assert (c | d);
	      R q = d.div (c);
	      
	      perp.mulsub (q, h);
	      proj.mulsub (q, hproj[j]);
	      
	      assert (! (perp % k));
	    }
	  
	}
      
      if (perp == 0)
	intr.append (proj);
      else
	{
	  hperp.append (perp);
	  hproj.append (proj);
	  hpivots.append (perp.head ().first);
	}
    }
  
  mod_span<R> span (parent, intr);
  return parent->submodule (span);
}

template<class R> ptr<const free_submodule<R> >
free_submodule<R>::plus (ptr<const free_submodule<R> > m) const
{
  assert (parent == m->parent);
  
  basedvector<linear_combination<R>, 1> s;
  for (unsigned i = 1; i <= dim (); i ++)
    s.append (gens[i]);
  
  for (unsigned i = 1; i <= m->dim (); i ++)
    s.append (m->gens[i]);
  
  mod_span<R> span (parent, s);
  return parent->submodule (span);
}

template<class R> bool
mod_map<R>::homogeneous () const
{
  for (unsigned i = 1; i <= impl->from->dim (); i ++)
    {
      if (column (i) != 0)
	{
	  grading dhq = column (i).hq () - impl->from->generator_grading (i);
	  for (unsigned j = i + 1; j <= impl->from->dim (); j ++)
	    {
	      if (column (j) != 0
		  && dhq != column (j).hq () - impl->from->generator_grading (j))
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
  for (unsigned i = 1; i <= impl->from->dim (); i ++)
    {
      if (column (i) != 0)
	assert (column (i).hq () - impl->from->generator_grading (i) == delta);
    }
}


template<class R> mod_map<R>
mod_map<R>::operator + (const mod_map &m) const
{
  assert (impl->from == m.impl->from && impl->to == m.impl->to);
  
  basedvector<linear_combination<R>, 1> v (impl->from->dim ());
  for (unsigned i = 1; i <= m.impl->from->dim (); i ++)
    v[i] = column (i) + m.column (i);
  return mod_map (IMPL, new explicit_map_impl<R> (impl->from, impl->to, v));
}

template<class R> ptr<const free_submodule<R> > 
mod_map<R>::kernel () const
{
  ptr<const module<R> > from = impl->from,
    to = impl->to;
  
  basedvector<linear_combination<R>, 1> from_xs (from->dim ());
  for (unsigned i = 1; i <= from->dim (); i ++)
    {
      linear_combination<R> x (from);
      x.muladd (1, i);
      from_xs[i] = x;
    }
  
  basedvector<linear_combination<R>, 1> to_xs (COPY2, explicit_columns ());
  
  for (unsigned i = 1; i <= to->dim (); i ++)
    {
      linear_combination<R> from_v (from),
	to_v (to);
      
      for (unsigned j = 1; j <= to_xs.size (); j ++)
	{
	  R to_vc = to_v(i);
	  if (to_vc.is_unit ())
	    break;
	  
	  linear_combination<R> &to_x = to_xs[j],
	    &from_x = from_xs[j];
	  R to_xc = to_x(i);
	  if (! (to_vc | to_xc))
	    {
	      tuple<R, R, R> t = to_vc.extended_gcd (to_xc);
	      assert (get<0> (t) == to_vc*get<1> (t) + get<2> (t)*to_xc);
	      
	      to_v = get<1> (t)*to_v + get<2> (t)*to_x;
	      from_v = get<1> (t)*from_v + get<2> (t)*from_x;
	      
	      assert (to_v(i) != 0);
	    }
	}
      
      R to_vc = to_v(i);
      if (to_vc != 0)
	{
	  for (unsigned j = 1; j <= to_xs.size (); j ++)
	    {
	      linear_combination<R> &to_x = to_xs[j],
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
  mod_span<R> span (impl->to, explicit_columns ());
  return impl->to->submodule (span);
}

template<class R> ptr<const quotient_module<R> > 
mod_map<R>::cokernel () const
{
  mod_span<R> span (impl->to, explicit_columns ());
  return impl->to->quotient (span);
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

template<class R> basedvector<linear_combination<R>, 1>
mod_map<R>::explicit_columns () const
{
  basedvector<linear_combination<R>, 1> v (impl->from->dim ());
  for (unsigned i = 1; i <= impl->from->dim (); i ++)
    v[i] = column (i);
  return v;
}

template<class R>
mod_span<R>::mod_span (ptr<const module<R> > mod,
		       basedvector<linear_combination<R>, 1> xs0)
{
  assert (mod->free_rank () == mod->dim ());
  
  basedvector<linear_combination<R>, 1> xs (COPY2, xs0);
  
  for (unsigned i = 1; i <= mod->dim (); i ++)
    {
      linear_combination<R> v (mod);
      
      for (unsigned j = 1; j <= xs.size (); j ++)
	{
	  R vc = v(i);
	  if (vc.is_unit ())
	    break;
	  
	  linear_combination<R> &x = xs[j];
	  R xc = x(i);
	  
	  if (! (vc | xc))
	    {
	      tuple<R, R, R> t = vc.extended_gcd (xc);
	      assert (get<0> (t) == vc*get<1> (t) + get<2> (t)*xc);
	      
	      v = get<1> (t)*v + get<2> (t)*x;
	      
	      assert (v(i) != 0);
	    }
	}
      
      R vc = v(i);
      if (vc != 0)
	{
	  for (unsigned j = 1; j <= xs.size (); j ++)
	    {
	      linear_combination<R> &x = xs[j];
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
  show (*impl->from);
  printf (" -> ");
  show (*impl->to);
}

template<class R> void
mod_map<R>::display_self () const
{
  show_self (); newline ();
  for (unsigned i = 1; i <= impl->from->dim (); i ++)
    {
      printf ("  %d ", i);
      impl->from->show_generator (i);
      printf (" ");
      show (impl->from->generator_grading (i));
      printf (": ");
      show (column (i));
      newline ();
    }
}

// ??? io

template<class R> void
writer::write_mod (ptr<const module<R> > m)
{
  pair<unsigned &, bool> p = aw->id_io_id.find (m->id_counter);
  if (p.second)
    {
      write_int ((int)p.first);
    }
  else
    {
      ++ aw->io_id_counter;
      unsigned io_id = aw->io_id_counter;
      
      p.first = io_id;
      
      write_int (- (int)io_id);
      
      unsigned n = m->dim (),
	r = m->free_rank ();
      write_unsigned (n);
      write_unsigned (r);
      for (unsigned i = 1; i <= n; i ++)
	write (*this, m->generator_grading (i));
      for (unsigned i = r + 1; i <= n; i ++)
	write (*this, m->generator_ann (i));
    }
}

template<class R> ptr<const module<R> > 
reader::read_mod ()
{
  int io_id = read_int ();
  if (io_id < 0)
    {
      unsigned n = read_unsigned ();
      unsigned r = read_unsigned ();
      basedvector<grading, 1> gr (n);
      for (unsigned i = 1; i <= n; i ++)
	gr[i] = grading (*this);
      basedvector<R, 1> ann (n - r);
      for (unsigned i = r + 1; i <= n; i ++)
	ann[i - r] = R (*this);
      
      ptr<const module<R> > m = new explicit_module<R> (r, ann, gr);
      ar->io_id_id.push ((unsigned)(-io_id), m->id);
      return m;
    }
  else
    {
      unsigned id = ar->io_id_id(io_id);
      return module<R>::id_module[id];
    }
}
