
template<class F>
class spanning_tree_complex
{
 public:  
  // twisted by default
  typedef fraction_field<polynomial<F> > R;
  
  const knot_diagram &kd;
  
  directed_multigraph bg;
  basedvector<unsigned, 1> edge_height;
  
  basedvector<set<unsigned>, 1> trees;
  map<set<unsigned>, unsigned> tree_idx;
  
 public:
  ptr<const module<R> > C;
  
 public:
  spanning_tree_complex (const knot_diagram &kd_);
  
  grading tree_grading (unsigned i) const;
  void show_tree (unsigned i) const;

  mod_map<R> totally_twisted_kh_d () const;
  mod_map<R> twisted_d2 () const;
};

template<class F>
class tree_generators
{
  typedef fraction_field<polynomial<F> > R;
  
  const spanning_tree_complex<F> &c;
  
public:
  tree_generators (const tree_generators &g) : c(g.c) { }
  tree_generators (const spanning_tree_complex<F> &c_) : c(c_) { }
  ~tree_generators () { }
  
  tree_generators &operator = (const tree_generators &) = delete;
  
  unsigned dim () const { return c.trees.size (); }
  unsigned free_rank () const { return c.trees.size (); }
  grading generator_grading (unsigned i) const { return c.tree_grading (i); }
  void show_generator (unsigned i) const { c.show_tree (i); }
  R generator_ann (unsigned i) const { abort (); }
};

template<class F>
spanning_tree_complex<F>::spanning_tree_complex (const knot_diagram &kd_)
  : kd(kd_)
{
  bg = kd.black_graph (edge_height);
  
  trees = bg.spanning_trees ();
  for (unsigned i = 1; i <= trees.size (); i ++)
    tree_idx.push (trees[i], i);
  
  C = new base_module<R, tree_generators<F> > (tree_generators<F> (*this));
}

template<class F> grading
spanning_tree_complex<F>::tree_grading (unsigned i) const
{
  unsigned h = 0;
  for (unsigned j = 1; j <= bg.num_edges (); j ++)
    {
      if (trees[i] % j)
	h += edge_height[j];
      else
	h += (1 - edge_height[j]);
    }
  // return grading (0, - h + kd.nplus); // ??? homologically graded
  return grading (h - kd.nminus, h + kd.nplus - 2 * kd.nminus);
}

template<class F> void
spanning_tree_complex<F>::show_tree (unsigned i) const
{
  // ???
  printf ("%d", i);
}

extern map<unsigned, int> find_cycle (const directed_multigraph &bg,
				      set<unsigned> edges,
				      unsigned f, unsigned f_to);

template<class F> mod_map<fraction_field<polynomial<F> > >
spanning_tree_complex<F>::twisted_d2 () const
{
  assert (kd.marked_edge);

  map_builder<R> b (C);
  
  basedvector<int, 1> edge_weight (kd.num_edges ());
  for (unsigned i = 1; i <= kd.num_edges (); i ++)
    edge_weight[i] = 1; // (1 << i);
  
  for (unsigned i = 1; i <= trees.size (); i ++)
    {
      set<unsigned> t = trees[i];
      
      smallbitset r (kd.n_crossings);
      for (unsigned k = 1; k <= kd.n_crossings; k ++)
	{
	  if ((edge_height[k] == 1) == (t % k))
	    r.push (k);
	}
      smoothing s (kd, r);
      
      for (set_const_iter<unsigned> ee = t; ee; ee ++)
	{
	  unsigned e = ee.val ();
	  
	  if (edge_height[e] != 0)
	    continue;
	  
	  for (unsigned f = 1; f <= bg.num_edges (); f ++)
	    {
	      if (edge_height[f] != 1 || (t % f))
		continue;
	      
	      set<unsigned> t2 (COPY, t);
	      t2.yank (e);
	      t2.push (f);
	      unsigned j = tree_idx(t2, 0);
	      if (j == 0)
		continue;
	      
	      bool neg = 0;
	      for (unsigned a = kd.marked_edge;;)
		{
		  unsigned p = s.edge_to (kd, a);
		  if (kd.ept_crossing[p] == e)
		    {
		      if (s.is_crossing_from_ept (kd, r, p))
			neg = s.crossing_from_inside (kd, r, e);
		      else
			neg = s.crossing_to_inside (kd, r, e);
		      break;
		    }
		  else if (kd.ept_crossing[p] == f)
		    {
		      if (s.is_crossing_from_ept (kd, r, p))
			neg = s.crossing_from_inside (kd, r, f);
		      else
			neg = s.crossing_to_inside (kd, r, f);
		      break;
		    }
		  
                  a = s.next_edge (kd, r, a);
		  assert (a != kd.marked_edge);
		}
	      
	      set<unsigned> neither (COPY, t);
	      neither.yank (e);
	      
	      smallbitset neither_r (kd.n_crossings);
	      for (unsigned k = 1; k <= kd.n_crossings; k ++)
		{
		  if ((edge_height[k] == 1) == (neither % k))
		    neither_r.push (k);
		}
	      smoothing neither_s (kd, neither_r);
	      
	      set<unsigned> both (COPY, t);
	      both.push (f);
	      
	      unsigned A = 0;
	      for (unsigned k = 1; k <= kd.num_edges (); k ++)
		{
		  if (neither_s.edge_circle[k]
		      != neither_s.edge_circle[kd.marked_edge])
		    A += edge_weight[k];
		}
	      
	      smallbitset both_r (kd.n_crossings);
	      for (unsigned k = 1; k <= kd.n_crossings; k ++)
		{
		  if ((edge_height[k] == 1) == (both % k))
		    both_r.push (k);
		}
	      smoothing both_s (kd, both_r);
	      
	      unsigned B = 0;
	      for (unsigned k = 1; k <= kd.num_edges (); k ++)
		{
		  if (both_s.edge_circle[k]
		      != both_s.edge_circle[kd.marked_edge])
		    B += edge_weight[k];
		}
	      
	      R x;
	      
	      if (neg)
		x += R (1);
	      
	      x += R (polynomial<F> (1),
		      polynomial<F> (1) + polynomial<F> (1, A));
	      x += R (polynomial<F> (1),
		      polynomial<F> (1) + polynomial<F> (1, B));
	      
	      b[i].muladd (x, j);
	    }
	}
    }
  
  return mod_map<R> (b);
}

template<class F> mod_map<fraction_field<polynomial<F> > >
spanning_tree_complex<F>::totally_twisted_kh_d () const
{
  assert (kd.marked_edge);
  
  map_builder<R> b (C);
  
  basedvector<int, 1> edge_weight (kd.num_edges ());
  for (unsigned i = 1; i <= kd.num_edges (); i ++)
    {
      edge_weight[i] = i;
      // edge_weight[i] = (1 << i);
      // edge_weight[i] = 1;
    }
  
  for (unsigned i = 1; i <= trees.size (); i ++)
    {
      set<unsigned> t = trees[i];
      
      smallbitset r (kd.n_crossings);
      for (unsigned k = 1; k <= kd.n_crossings; k ++)
	{
	  if ((edge_height[k] == 1) == (t % k))
	    r.push (k);
	}
      smoothing s (kd, r);
      
      for (set_const_iter<unsigned> ee = t; ee; ee ++)
	{
	  unsigned e = ee.val ();
	  
	  if (edge_height[e] != 0)
	    continue;
	  
	  for (unsigned f = 1; f <= bg.num_edges (); f ++)
	    {
	      if (edge_height[f] != 1 || (t % f))
		continue;
	      
	      set<unsigned> t2 (COPY, t);
	      t2.yank (e);
	      t2.push (f);
	      unsigned j = tree_idx(t2, 0);
	      if (j == 0)
		continue;
	      
	      set<unsigned> neither (COPY, t);
	      neither.yank (e);
	      
	      smallbitset neither_r (kd.n_crossings);
	      for (unsigned k = 1; k <= kd.n_crossings; k ++)
		{
		  if ((edge_height[k] == 1) == (neither % k))
		    neither_r.push (k);
		}
	      smoothing neither_s (kd, neither_r);
	      
	      set<unsigned> both (COPY, t);
	      both.push (f);
	      
	      R A = 0;
	      for (unsigned k = 1; k <= kd.num_edges (); k ++)
		{
		  if (neither_s.edge_circle[k]
		      != neither_s.edge_circle[kd.marked_edge])
		    A += polynomial<F> (1, edge_weight[k]);
		}
	      
	      smallbitset both_r (kd.n_crossings);
	      for (unsigned k = 1; k <= kd.n_crossings; k ++)
		{
		  if ((edge_height[k] == 1) == (both % k))
		    both_r.push (k);
		}
	      smoothing both_s (kd, both_r);
	      
	      R B = 0;
	      for (unsigned k = 1; k <= kd.num_edges (); k ++)
		{
		  if (both_s.edge_circle[k]
		      != both_s.edge_circle[kd.marked_edge])
		    B += polynomial<F> (1, edge_weight[k]);
		}
	      
	      R x;
	      
	      x += R (polynomial<F> (1)) / A;
	      x += R (polynomial<F> (1)) / B;
	      
	      b[i].muladd (x, j);
	    }
	}
    }
  
  return mod_map<R> (b);
}
