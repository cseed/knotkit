
template<class R>
class khC_generators
{
  const cube<R> &c;
  
public:
  khC_generators (const khC_generators &g) : c(g.c) { }
  khC_generators (const cube<R> &c_) : c(c_) { }
  ~khC_generators () { }
  
  khC_generators &operator = (const khC_generators &) = delete;
  
  unsigned dim () const { return c.n_generators; }
  unsigned free_rank () const { return c.n_generators; }
  grading generator_grading (unsigned i) const { return c.compute_generator_grading (i); }
  void show_generator (unsigned i) const
  {
    pair<unsigned, unsigned> sm = c.generator_state_monomial (i);
    c.show_state_monomial (sm.first, sm.second);
  }
  
  R generator_ann (unsigned i) const { abort (); }
};

template<class R> mod_map<R>
cube<R>::compute_map (unsigned dh, unsigned max_n,
		      bool mirror,
		      bool reverse_orientation,
		      unsigned to_reverse,
		      const map_rules &rules) const
{
  map_builder<R> b (khC);
  
  smoothing from_s (kd);
  smoothing to_s (kd);
  resolution_diagram_builder rdb;
  
  ullmanset<1> free_circles (max_circles);
  ullmanset<1> from_circles (max_circles);
  
  if (verbose)
    {
      fprintf (stderr, "computing differential...\n");
      fprintf (stderr, "%d resolutions.\n", n_resolutions);
    }
  
  basedvector<pair<unsigned, unsigned>, 1> out;
  for (unsigned fromstate = 0; fromstate < n_resolutions; fromstate ++)
    {
      if (verbose
	  &&  (fromstate & unsigned_fill (n_crossings > 4
					  ? n_crossings - 4
					  : 0)) == 0)
	fprintf (stderr, "%d / %d resolutions done.\n", fromstate, n_resolutions);
      
      unsigned n_zerocrossings = n_crossings - unsigned_bitcount (fromstate);
      unsigned n_cobordisms = ((unsigned)1) << n_zerocrossings;
      
      from_s.init (kd, smallbitset (n_crossings, fromstate));
      
      unionfind<1> u (from_s.n_circles);
      
      basedvector<unsigned, 1> from_circle_edge_rep (from_s.n_circles);
      for (unsigned j = 1; j <= kd.num_edges (); j ++)
	from_circle_edge_rep[from_s.edge_circle[j]] = j;
      
      for (unsigned j = 0; j < n_cobordisms; j ++)
	{
	  if (dh && unsigned_bitcount (j) != dh)
	    continue;
	  if (max_n && unsigned_bitcount (j) > max_n)
	    continue;
	  
	  unsigned tostate = unsigned_pack (n_crossings, fromstate, j);
	  unsigned crossings = tostate & ~fromstate;
	  
	  int sign = 1;
	  for (unsigned i = 1; i <= n_crossings; i ++)
	    {
	      if (unsigned_bittest (crossings, i))
		break;
	      if (unsigned_bittest (fromstate, i))
		sign *= -1;
	    }
	  // printf ("fromstate = %d, tostate = %d, sign = %d\n", fromstate, tostate, sign);
	  
	  u.clear ();
	  from_circles.clear ();
	  
	  for (unsigned_const_iter k = crossings; k; k ++)
	    {
	      unsigned c = k.val ();
	      
	      unsigned from = from_s.ept_circle (kd, kd.crossings[c][1]),
		to = from_s.ept_circle (kd, kd.crossings[c][3]);
	      from_circles += from;
	      from_circles += to;
	      u.join (from, to);
	    }
	  if (from_s.n_circles - from_circles.card () + 1 != u.num_sets ())
	    continue;
	  
	  to_s.init (kd, smallbitset (n_crossings, tostate));
	  rdb.init (kd,
		    smallbitset (n_crossings, fromstate), from_s,
		    smallbitset (n_crossings, tostate), to_s,
		    smallbitset (n_crossings, crossings));
	  if (mirror)
	    rdb.mirror ();
	  if (reverse_orientation)
	    rdb.reverse_orientation ();
	  if (to_reverse)
	    rdb.reverse_crossing (kd, from_s, to_s, to_reverse);
	  
	  // display (rdb.rd);
	  
	  out.resize (0);
	  rules.map (out, rdb);
	  if (out.size () == 0)
	    continue;
	  
	  free_circles.clear ();
	  for (unsigned i = 1; i <= from_s.n_circles; i ++)
	    {
	      if (! (rdb.gl_starting_circles % i))
		free_circles.push (i);
	    }
	  unsigned n_free_circles = free_circles.card ();
	  unsigned n_free_monomials = ((unsigned)1) << n_free_circles;
	  
	  for (unsigned i = 0; i < n_free_monomials; i ++)
	    {
	      unsigned m_from = 0,
		m_to = 0;
	      
	      for (unsigned_const_iter jj = i; jj; jj ++)
		{
		  unsigned s = free_circles.nth (jj.val () - 1);
		  m_from = unsigned_bitset (m_from, s);
		  m_to = unsigned_bitset (m_to, to_s.edge_circle[from_circle_edge_rep[s]]);
		}
	      
	      for (unsigned j = 1; j <= out.size (); j ++)
		{
		  unsigned v_from = m_from,
		    v_to = m_to;
		  
		  unsigned l_from = out[j].first,
		    l_to = out[j].second;
		  for (unsigned_const_iter kk = l_from; kk; kk ++)
		    {
		      unsigned s = rdb.gl_starting_circles.nth (kk.val () - 1);
		      v_from = unsigned_bitset (v_from, s);
		    }
		  for (unsigned_const_iter kk = l_to; kk; kk ++)
		    {
		      unsigned s = rdb.gl_ending_circles.nth (kk.val () - 1);
		      v_to = unsigned_bitset (v_to, s);
		    }
		  
		  if (markedp_only)
		    {
		      unsigned p = from_s.edge_circle[kd.marked_edge];
		      if (unsigned_bittest (v_from, p))
			continue;
		      assert (!unsigned_bittest (v_to, p));
		    }

		  b[generator (fromstate, v_from)].muladd
		    (sign, generator (tostate, v_to));
		}
	    }
	}
    }
  
  if (verbose)
    {
      fprintf (stderr, "%d / %d resolutions done.\n", n_resolutions, n_resolutions);
      fprintf (stderr, "computing differential done.\n");
    }
  
  return mod_map<R> (b);
}

class d_rules : public map_rules
{
public:
  d_rules () { }
  d_rules (const d_rules &) = delete;
  ~d_rules () { }
  
  d_rules &operator = (const d_rules &) = delete;
  
  void map (basedvector<pair<unsigned, unsigned>, 1> &out,
	    resolution_diagram_builder &rdb) const
  {
    rdb.rd.d (out);
  }
};

template<class R> mod_map<R>
cube<R>::compute_d (unsigned dh, unsigned max_n,
		    bool mirror,
		    bool reverse_orientation,
		    unsigned to_reverse) const
{
  return compute_map (dh, max_n,
		      mirror, reverse_orientation, to_reverse,
		      d_rules ());
}

class twin_arrows_P_rules : public map_rules
{
public:
  twin_arrows_P_rules () { }
  twin_arrows_P_rules (const twin_arrows_P_rules &) = delete;
  ~twin_arrows_P_rules () { }
  
  twin_arrows_P_rules &operator = (const twin_arrows_P_rules &) = delete;
  
  void map (basedvector<pair<unsigned, unsigned>, 1> &out,
	    resolution_diagram_builder &rdb) const
  {
    rdb.rd.twin_arrows_P (out);
  }
};

template<class R> mod_map<R>
cube<R>::compute_twin_arrows_P (bool mirror,
				bool reverse_orientation,
				unsigned to_reverse) const
{
  assert (kd.marked_edge);
  
  mod_map<R> one (khC, 1);
  mod_map<R> X = compute_X (kd.marked_edge);
  mod_map<R> preP = compute_map (0, 0, 
				 mirror, reverse_orientation, to_reverse,
				 twin_arrows_P_rules ());
  
  return X.compose (one + preP);
}

template<class R> mod_map<R>
cube<R>::compute_nu () const
{
  assert (!markedp_only);
  
  map_builder<R> b (khC);
  for (unsigned i = 0, j = 1; i < n_resolutions; i ++)
    {
      smoothing s (kd, smallbitset (n_crossings, i));
      for (unsigned j = 0; j < s.num_monomials (); j ++)
	{
	  for (unsigned k = 1; k <= s.n_circles; k ++)
	    {
	      if (!unsigned_bittest (j, k))
		{
		  unsigned j2 = unsigned_bitset (j, k);
		  b[generator (i, j)].muladd (1, generator (i, j2));
		}
	    }
	}
    }
  
  mod_map<R> nu (b);
  
  nu.check_grading (0, 2);
  assert (nu.compose (nu) == 0);
  
  return nu;
}

template<class R> mod_map<R>
cube<R>::compute_X (unsigned p) const
{
  assert (!markedp_only);
  
  /* define Khovanov's map X */
  map_builder<R> b (khC);
  for (unsigned i = 0, j = 1; i < n_resolutions; i ++)
    {
      smoothing r (kd, smallbitset (n_crossings, i));
      for (unsigned j = 0; j < r.num_monomials (); j ++)
	{
	  unsigned s = r.edge_circle[p];
	  if (unsigned_bittest (j, s))
	    {
	      unsigned j2 = unsigned_bitclear (j, s);
	      b[generator (i, j)].muladd (1, generator (i, j2));
	    }
	}
    }

  mod_map<R> X (b);
  assert (X.compose (X) == 0);
  
  return X;
}

template<class R> mod_map<R>
cube<R>::H_i (unsigned c)
{
  mod_map<R> b (khC, 0);
  for (unsigned i = 0; i < n_resolutions; i ++)
    {
      if (unsigned_bittest (i, c))
	continue;
      
      smoothing from_s (kd, smallbitset (n_crossings, i));
      
      unsigned i2 = unsigned_bitset (i, c);
      smoothing to_s (kd, smallbitset (n_crossings, i2));
      
      basedvector<unsigned, 1> from_circle_edge_rep (from_s.n_circles);
      for (unsigned j = 1; j <= kd.num_edges (); j ++)
	from_circle_edge_rep[from_s.edge_circle[j]] = j;
      
      unsigned from = from_s.crossing_from_circle (kd, c),
	to = from_s.crossing_to_circle (kd, c);
      for (unsigned j = 0; j < from_s.num_monomials (); j ++)
	{
	  unsigned j2 = 0;
	  for (unsigned_const_iter k = j; k; k ++)
	    {
	      unsigned s = k.val ();
	      j2 = unsigned_bitset (j2, to_s.edge_circle[from_circle_edge_rep[s]]);
	    }
	  
	  linear_combination &v = b[generator (i, j)];
	  if (from == to
	      && unsigned_bittest (j, from))
	    {
	      j2 = unsigned_bitset (j2,
				    to_s.crossing_from_circle (kd, c));
	      j2 = unsigned_bitset (j2,
				    to_s.crossing_to_circle (kd, c));
	      v.muladd (1, generator (i2, j2));
	    }
	  else if (from != to
		   && !unsigned_bittest (j, from)
		   && !unsigned_bittest (j, to))
	    {
	      assert (! unsigned_bittest (j2,
					  to_s.crossing_from_circle (kd, c)));
	      v.muladd (1, generator (i2, j2));
	    }
	}
    }
  
  mod_map<R> H_c (b);
  
  H_c.check_grading (grading (1, 2));
  
  return H_c;
}

template<class R> mod_map<R>
cube<R>::compute_dinv (unsigned c)
{
  map_builder<R> p (khC);
  for (unsigned i = 0; i < n_resolutions; i ++)
    {
      if (!unsigned_bittest (i, c))
	continue;

      int sign = 1;
      for (unsigned j = 1; j < c; j ++)
	{
	  if (unsigned_bittest (i, j))
	    sign *= -1;
	}
      
      smoothing from_s (kd, smallbitset (n_crossings, i));
      
      unsigned i2 = unsigned_bitclear (i, c);
      smoothing to_s (kd, smallbitset (n_crossings, i2));
      
      basedvector<unsigned, 1> from_circle_edge_rep (from_s.n_circles);
      for (unsigned j = 1; j <= kd.num_edges (); j ++)
	from_circle_edge_rep[from_s.edge_circle[j]] = j;
      
      unsigned a = from_s.crossing_from_circle (kd, c),
	b = from_s.crossing_to_circle (kd, c);
      unsigned x = to_s.crossing_from_circle (kd, c),
	y = to_s.crossing_to_circle (kd, c);
      for (unsigned j = 0; j < from_s.num_monomials (); j ++)
	{
	  unsigned j2 = 0;
	  for (unsigned_const_iter k = j; k; k ++)
	    {
	      unsigned s = k.val ();
	      j2 = unsigned_bitset (j2, to_s.edge_circle[from_circle_edge_rep[s]]);
	    }
	  
	  if (a == b)
	    {
	      // split
	      assert (x != y);
	      
	      if (unsigned_bittest (j, a))
		{
		  // 1 -> x + y
		  j2 = unsigned_bitset (j2, x);
		  j2 = unsigned_bitclear (j2, y);
		  
		  p[generator (i, j)].muladd (sign, generator (i2, j2));
		  
		  j2 = unsigned_bitclear (j2, x);
		  j2 = unsigned_bitset (j2, y);
		  
		  p[generator (i, j)].muladd (sign, generator (i2, j2));
		}
	      else
		{
		  // a -> xy
		  j2 = unsigned_bitclear (j2, x);
		  j2 = unsigned_bitclear (j2, y);
		  
		  p[generator (i, j)].muladd (sign, generator (i2, j2));
		}
	    }
	  else
	    {
	      // join
	      assert (x == y);
	      
	      if (unsigned_bittest (j, a)
		  && unsigned_bittest (j, b))
		{
		  // 1 -> 1
		  j2 = unsigned_bitset (j2, x);
		  
		  p[generator (i, j)].muladd (sign, generator (i2, j2));
		}
	      else if ((unsigned_bittest (j, a)
			&& !unsigned_bittest (j, b))
		       || (!unsigned_bittest (j, a)
			   && unsigned_bittest (j, b)))
		{
		  // a, b -> x
		  j2 = unsigned_bitclear (j2, x);
		  
		  p[generator (i, j)].muladd (sign, generator (i2, j2));
		}
	    }
	}
    }
  
  mod_map<R> dinv (p);
  dinv.check_grading (grading (-1, -2));
  return dinv;
}

template<class R> void
cube<R>::check_reverse_crossings ()
{
  for (unsigned c = 1; c <= n_crossings; c ++)
    {
      mod_map<R> H_c = H_i (c);
      
      mod_map<R> d = compute_d (2, 0, 0, 0, 0, 0),
	r_d = compute_d (2, 0, 0, 0, 0, c);
      
      mod_map<R> d_1 = d.graded_piece (1, 0);
      assert (d_1 == r_d.graded_piece (1, 0));
      assert (d_1.compose (d_1) == 0);
      
      mod_map<R> d_2 = d.graded_piece (2, 2),
	r_d_2 = r_d.graded_piece (2, 2);
      assert (d_1.compose (d_2) == d_2.compose (d_1));
      assert (d_1.compose (r_d_2) == r_d_2.compose (d_1));
      
      assert (d_2 + r_d_2 == H_c.compose (d_1) + d_1.compose (H_c));
    }
}

template<class R> void
cube<R>::check_reverse_orientation ()
{
  mod_map<R> d = compute_d (2, 0, 0, 0, 0, 0),
    n_d = compute_d (2, 0, 0, 0, 1, 0);
  
  mod_map<R> d_1 = d.graded_piece (1, 0);
  assert (d_1 == n_d.graded_piece (1, 0));
  assert (d_1.compose (d_1) == 0);
  
  mod_map<R> d_2 = d.graded_piece (2, 2),
    n_d_2 = n_d.graded_piece (2, 2);
  
  // in fact, Szabo's d_2 matches on the nose
  assert (d_2 == n_d_2);
}

template<class R>
cube<R>::cube (knot_diagram &kd_, bool markedp_only_)
  : markedp_only(markedp_only_),
    kd(kd_),
    n_crossings(kd.n_crossings),
    n_resolutions(((unsigned)1) << n_crossings),
    n_generators(0),
    resolution_circles(n_resolutions),
    resolution_generator1(n_resolutions)
{
  // printf ("%% %s\n", kd.name.c_str ());
  
  // printf ("smoothings:\n");
  
  smoothing s (kd);
  for (unsigned i = 0; i < n_resolutions; i ++)
    {
      smallbitset state (n_crossings, i);
      s.init (kd, state);
      
#if 1
      unsigned fromstate = i;
      smoothing &from_s = s;
      
      unsigned n_zerocrossings = n_crossings - unsigned_bitcount (fromstate);
      unsigned n_cobordisms = ((unsigned)1) << n_zerocrossings;
      for (unsigned j = 0; j < n_cobordisms; j ++)
	{
	  unsigned tostate = unsigned_pack (n_crossings, fromstate, j);
	  unsigned crossings = tostate & ~fromstate;
	  
	  smoothing to_s (kd, smallbitset (n_crossings, tostate));
	  
	  set<unsigned> starting_circles,
	    ending_circles;
	  for (unsigned_const_iter k = crossings; k; k ++)
	    {
	      unsigned c = k.val ();
	      
	      unsigned starting_from = s.ept_circle (kd, kd.crossings[c][2]),
		starting_to = s.ept_circle (kd, kd.crossings[c][4]);
	      starting_circles += starting_from;
	      starting_circles += starting_to;
	      
	      unsigned ending_from = to_s.ept_circle (kd, kd.crossings[c][2]),
		ending_to = to_s.ept_circle (kd, kd.crossings[c][4]);
	      ending_circles += ending_from;
	      ending_circles += ending_to;
	    }
	  if (starting_circles.card () == 1
	      && ending_circles.card () == 1)
	    {
	      unsigned k = unsigned_bitcount (crossings);
	      
#if 0
	      s.show_self (kd, state);
	      printf (" crossings ");  show (smallbitset (n_crossings, crossings));
	      newline ();
#endif
	    }
	}
#endif
      
      resolution_circles[i] = s.n_circles;
      resolution_generator1[i] = n_generators + 1;
      n_generators += s.num_generators (markedp_only);
      
#if 0
      s.show_self (kd, state);
      newline ();
#endif
    }
  
  // printf ("(cube) n_generators = %d\n", n_generators);
  khC = new base_module<R, khC_generators<R> > (khC_generators<R> (*this));
}

template<class R> unsigned
cube<R>::generator (unsigned i, unsigned j) const
{
  if (markedp_only)
    {
      smoothing s (kd, smallbitset (n_crossings, i));
      unsigned p = s.edge_circle[kd.marked_edge];
      assert (!unsigned_bittest (j, p));
      return resolution_generator1[i] + unsigned_discard_bit (j, p);
    }
  else
    return resolution_generator1[i] + j;
}

template<class R> pair<unsigned, unsigned> 
cube<R>::generator_state_monomial (unsigned g) const
{
  unsigned i = resolution_generator1.upper_bound (g) - 1;
  assert (g >= resolution_generator1[i]
	  && (i + 1 >= n_resolutions
	      || g < resolution_generator1[i + 1]));
  return pair<unsigned, unsigned> (i, g - resolution_generator1[i]);
}

template<class R> grading
cube<R>::compute_generator_grading (unsigned g) const
{
  pair<unsigned, unsigned> sm = generator_state_monomial (g);
  return compute_state_monomial_grading (sm.first, sm.second);
}

template<class R> grading
cube<R>::compute_state_monomial_grading (unsigned state, unsigned monomial) const
{
  grading r;
  r.h = unsigned_bitcount (state) - kd.nminus;
  r.q = 2 * unsigned_bitcount (monomial) - resolution_circles[state]
    + unsigned_bitcount (state) + kd.nplus - 2 * kd.nminus;
  
  /* reduced-only is shifted up 1 in q-grading */
  if (markedp_only)
    r.q ++;
  
  return r;
}

template<class R> ptr<const module<R> >
cube<R>::compute_kh () const
{
  // display ("khC:\n", *khC);
  
  mod_map<R> d = compute_d (1, 0, 0, 0, 0);
  
  chain_complex_simplifier<Z> s (khC, d, 1);
  // display ("s.new_C:\n", *s.new_C);
  
  return s.new_d.homology ();
}

template<class R> void
cube<R>::show_state (unsigned state) const
{
  for (unsigned i = n_crossings; i >= 1; i --)
    printf (unsigned_bittest (state, i) ? "1" : "0");
}

template<class R> void
cube<R>::show_state_monomial (unsigned state, unsigned monomial) const
{
  show_state (state);
  printf (":");
  smoothing s (kd, smallbitset (n_crossings, state));
  char c = 'a';
  for (unsigned i = 1; i <= s.n_circles; i ++, c ++)
    {
      if (!unsigned_bittest (monomial, i))
	printf ("%c", c);
    }
}

template<class R> void
cube<R>::display_self () const
{
  for (unsigned i = 0, j = 1; i < n_resolutions; i ++)
    {
      smoothing s (kd, smallbitset (n_crossings, i));

      printf (" %03d: ", i);
      s.show_self (kd, smallbitset (n_crossings, i));
      newline ();
    }
  
  // khC->display_self ();
  // d.display_self ();
}

template<class F> mod_map<typename twisted_cube<F>::R> 
twisted_cube<F>::compute_twisted_map (basedvector<int, 1> edge_weight,
				      unsigned dh, unsigned to_reverse,
				      const twisted_map_rules &rules) const
{
  map_builder<R> b (c.khC);
  
  knot_diagram &kd = c.kd;
  unsigned n_crossings = c.n_crossings;
  unsigned n_resolutions = c.n_resolutions;
  
  smoothing from_s (kd);
  smoothing to_s (kd);
  resolution_diagram_builder rdb;
  
  ullmanset<1> free_circles (max_circles);
  ullmanset<1> from_circles (max_circles);
  
  basedvector<triple<unsigned, unsigned, set<unsigned> >, 1> out;
  for (unsigned fromstate = 0; fromstate < n_resolutions; fromstate ++)
    {
      unsigned n_zerocrossings = n_crossings - unsigned_bitcount (fromstate);
      unsigned n_cobordisms = ((unsigned)1) << n_zerocrossings;
      
      from_s.init (kd, smallbitset (n_crossings, fromstate));
      
      unionfind<1> u (from_s.n_circles);
      
      basedvector<unsigned, 1> from_circle_edge_rep (from_s.n_circles);
      for (unsigned j = 1; j <= kd.num_edges (); j ++)
	from_circle_edge_rep[from_s.edge_circle[j]] = j;
      
      for (unsigned j = 0; j < n_cobordisms; j ++)
	{
	  if (dh && unsigned_bitcount (j) != dh)
	    continue;
	  
	  unsigned tostate = unsigned_pack (n_crossings, fromstate, j);
	  unsigned crossings = tostate & ~fromstate;
	  
	  int sign = 1;
	  for (unsigned i = 1; i <= n_crossings; i ++)
	    {
	      if (unsigned_bittest (crossings, i))
		break;
	      if (unsigned_bittest (fromstate, i))
		sign *= -1;
	    }
	  // printf ("fromstate = %d, tostate = %d, sign = %d\n", fromstate, tostate, sign);
	  
	  u.clear ();
	  from_circles.clear ();
	  
	  for (unsigned_const_iter k = crossings; k; k ++)
	    {
	      unsigned c = k.val ();
	      
	      unsigned from = from_s.ept_circle (kd, kd.crossings[c][1]),
		to = from_s.ept_circle (kd, kd.crossings[c][3]);
	      from_circles += from;
	      from_circles += to;
	      u.join (from, to);
	    }
	  if (from_s.n_circles - from_circles.card () + 1 != u.num_sets ())
	    continue;
	  
	  to_s.init (kd, smallbitset (n_crossings, tostate));
	  rdb.init (kd,
		    smallbitset (n_crossings, fromstate), from_s,
		    smallbitset (n_crossings, tostate), to_s,
		    smallbitset (n_crossings, crossings));
	  if (to_reverse)
	    rdb.reverse_crossing (kd, from_s, to_s, to_reverse);
	  
	  // display (rdb.rd);
	  
	  out.resize (0);
	  rules.map (out, rdb);
	  if (out.size () == 0)
	    continue;
	  
	  free_circles.clear ();
	  for (unsigned i = 1; i <= from_s.n_circles; i ++)
	    {
	      if (! (rdb.gl_starting_circles % i))
		free_circles.push (i);
	    }
	  unsigned n_free_circles = free_circles.card ();
	  unsigned n_free_monomials = ((unsigned)1) << n_free_circles;
	  
	  for (unsigned i = 0; i < n_free_monomials; i ++)
	    {
	      unsigned m_from = 0,
		m_to = 0;
	      
	      for (unsigned_const_iter jj = i; jj; jj ++)
		{
		  unsigned s = free_circles.nth (jj.val () - 1);
		  m_from = unsigned_bitset (m_from, s);
		  m_to = unsigned_bitset (m_to, to_s.edge_circle[from_circle_edge_rep[s]]);
		}
	      
	      for (unsigned j = 1; j <= out.size (); j ++)
		{
		  unsigned v_from = m_from,
		    v_to = m_to;
		  
		  unsigned l_from = out[j].first,
		    l_to = out[j].second;
		  for (unsigned_const_iter kk = l_from; kk; kk ++)
		    {
		      unsigned s = rdb.gl_starting_circles.nth (kk.val () - 1);
		      v_from = unsigned_bitset (v_from, s);
		    }
		  for (unsigned_const_iter kk = l_to; kk; kk ++)
		    {
		      unsigned s = rdb.gl_ending_circles.nth (kk.val () - 1);
		      v_to = unsigned_bitset (v_to, s);
		    }

		  int w = 0;
		  for (set_const_iter<unsigned> le = out[j].third; le; le ++)
		    {
		      for (set_const_iter<unsigned> ge = rdb.lg_edges[le.val ()]; ge; ge ++)
			w += edge_weight[ge.val ()];
		    }
		  
		  if (c.markedp_only)
		    {
		      unsigned p = from_s.edge_circle[kd.marked_edge];
		      if (unsigned_bittest (v_from, p))
			continue;
		      assert (!unsigned_bittest (v_to, p));
		    }
		  
		  // ??? sign
		  b[c.generator (fromstate, v_from)].muladd
		    (polynomial<F> (1) + polynomial<F> (1, w),
		     c.generator (tostate, v_to));
		}
	    }
	}
    }
  
  return mod_map<R> (b);
}

class twisted_barE_rules : public twisted_map_rules
{
public:
  twisted_barE_rules () { }
  twisted_barE_rules (const twisted_barE_rules &) = delete;
  ~twisted_barE_rules () { }
  
  twisted_barE_rules &operator = (const twisted_barE_rules &) = delete;
  
  void map (basedvector<triple<unsigned, unsigned, set<unsigned> >, 1> &out,
	    resolution_diagram_builder &rdb) const
  {
    rdb.rd.twisted_barE (out);
  }
};

template<class F> mod_map<typename twisted_cube<F>::R> 
twisted_cube<F>::compute_twisted_barE (basedvector<int, 1> edge_weight,
				       unsigned dh, unsigned to_reverse) const
{
  return compute_twisted_map (edge_weight,
			      dh, to_reverse,
			      twisted_barE_rules ());
}

template<class F> mod_map<typename twisted_cube<F>::R> 
twisted_cube<F>::twisted_d0 (basedvector<int, 1> edge_weight) const
{
  map_builder<R> b (c.khC);
  for (unsigned i = 0, j = 1; i < c.n_resolutions; i ++)
    {
      smoothing r (c.kd, smallbitset (c.n_crossings, i));
      
      unsigned marked_s = c.markedp_only
	? r.edge_circle[c.kd.marked_edge]
	: 0;
      
      for (unsigned j = 0; j < r.num_monomials (); j ++)
	{
	  if (c.markedp_only
	      && unsigned_bittest (j, marked_s))
	    continue;
	  
	  for (unsigned s = 1; s <= r.n_circles; s ++)
	    {
	      if (unsigned_bittest (j, s))
		{
		  int w = 0;
		  
		  for (unsigned k = 1; k <= c.kd.num_edges (); k ++)
		    if (r.edge_circle[k] == s)
		      w += edge_weight[k];
		  
		  unsigned j2 = unsigned_bitclear (j, s);
		  b[c.generator (i, j)].muladd (polynomial<F> (1) + polynomial<F> (1, w),
						c.generator (i, j2));
		}
	    }
	}
    }
  return mod_map<R> (b);
}
