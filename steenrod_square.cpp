
#include <knotkit.h>

steenrod_square::steenrod_square (const cube<Z2> &cor_,
				  mod_map<Z2> &d_,
				  const chain_complex_simplifier<Z2> &s_)
  : cor(cor_), d(d_), s(s_)
{
  for (unsigned i = 1; i <= cor.khC->dim (); i ++)
    {
      grading igr = cor.khC->generator_grading (i);
      KGij[igr].push (i);
    }
}

Z2
steenrod_square::sC (unsigned x, unsigned y) const
{
  pair<unsigned, unsigned> x_sm = cor.generator_state_monomial (x),
    y_sm = cor.generator_state_monomial (y);
  
  unsigned changed = x_sm.first ^ y_sm.first;
  assert (unsigned_bitcount (changed) == 1);
  unsigned i = unsigned_ffs (changed);
  
  unsigned k = 0;
  for (unsigned j = 1; j < i; j ++)
    {
      if (unsigned_bittest (x_sm.first, j))
	k ++;
    }
  
  return Z2 (k);
}

Z2
steenrod_square::fC (unsigned x, unsigned y) const
{
  pair<unsigned, unsigned> x_sm = cor.generator_state_monomial (x),
    y_sm = cor.generator_state_monomial (y);
  
  unsigned changed = x_sm.first ^ y_sm.first;
  assert (unsigned_bitcount (changed) == 2);
  
  unsigned i = unsigned_ffs (changed),
    j = unsigned_ffs (unsigned_bitclear (changed, i));
  assert (i < j);
  
  unsigned ki = 0;
  for (unsigned l = 1; l < i; l ++)
    {
      if (unsigned_bittest (x_sm.first, l))
	ki ++;
    }
  unsigned kj = 0;
  for (unsigned l = i + 1; l < j; l ++)
    {
      if (unsigned_bittest (x_sm.first, l))
	kj ++;
    }
  
  return Z2 (ki) * Z2 (kj);
}

triple<set<unsigned>, // G1cx
       map<unsigned, unsigned>, // bx
       map<unsigned, Z2> > // sx
steenrod_square::boundary_matching (grading cgr,
				    linear_combination<Z2> c,
				    unsigned x) const
{
  set<unsigned> G1cx;
  map<unsigned, unsigned> bx;
  map<unsigned, Z2> sx;
  
  for (linear_combination_const_iter<Z2> yy = c; yy; yy ++)
    {
      assert (yy.val () == 1);
      unsigned y = yy.key ();
      
      if (d[y](x) == 1)
	G1cx.push (y);
    }
  assert (is_even (G1cx.card ()));
  
  for (set_const_iter<unsigned> i = G1cx; i; i ++)
    {
      unsigned y = i.val ();
      i ++;
      unsigned bxy = i.val ();
      
      bx.push (y, bxy);
      bx.push (bxy, y);
      
      sx.push (y, 0);
      sx.push (bxy, Z2 (1) + sC (x, y) + sC (x, bxy));
    }
  
#ifndef NDEBUG
  for (set_const_iter<unsigned> yy = G1cx; yy; yy ++)
    {
      unsigned y = yy.val ();
      assert (sx(y) + sx(bx(y)) == Z2 (1) + sC (x, y) + sC (x, bx(y)));
    }
#endif
  
  return triple<set<unsigned>, // G1cx
		map<unsigned, unsigned>, // bx
		map<unsigned, Z2> > // sx
    (G1cx, bx, sx);
}

mod_map<Z2>
steenrod_square::sq1 () const
{
  map_builder<Z2> b (s.new_C);
  
  for (unsigned i = 1; i <= s.new_C->dim (); i ++)
    {
      grading cgr = s.new_C->generator_grading (i);
      linear_combination<Z2> c = s.iota[i];
      assert (d.map (c) == 0);
      
      grading gr2 (cgr.h + 1, cgr.q);
      if (! (KGij % gr2))
	continue;
      
      linear_combination<Z2> sq1c (cor.khC);
      
      for (set_const_iter<unsigned> x = KGij[gr2]; x; x ++)
	{
	  triple<set<unsigned>,
		 map<unsigned, unsigned>,
		 map<unsigned, Z2> > t = boundary_matching (cgr, c, x.val ());
	  
	  Z2 a = 0;
	  for (set_const_iter<unsigned> y = t.first; y; y ++)
	    a += t.third(y.val ());
	      
	  sq1c.muladd (a, x.val ());
	}
      // display ("sq1c:\n", sq1c);
      assert (d.map (sq1c) == 0);
      
      b[i].muladd (1, s.pi.map (sq1c));
    }
  
  return mod_map<Z2> (b);
}

set<pair<unsigned, unsigned> > 
steenrod_square::make_G2cx (grading cgr,
			    linear_combination<Z2> c,
			    unsigned x) const
{
  set<pair<unsigned, unsigned> > G2cx;
  
  for (linear_combination_const_iter<Z2> yy = c; yy; yy ++)
    {
      unsigned y = yy.key ();
      
      for (linear_combination_const_iter<Z2> zz = d[y]; zz; zz ++)
	{
	  unsigned z = zz.key ();
	  
	  if (d[z](x) == 1)
	    G2cx.push (pair<unsigned, unsigned> (z, y));
	}
    }
  return G2cx;
}

class graph
{
public:
  unsigned n_vertices;
  unsigned n_edges;
  
  basedvector<unsigned, 1> z;
  basedvector<unsigned, 1> y;
  map<pair<unsigned, unsigned>, unsigned> zy_vertex;
  
  unsigned vertex (unsigned z, unsigned y) const
  {
    return zy_vertex(pair<unsigned, unsigned> (z, y));
  }
  
  basedvector<unsigned, 1> edge_from, edge_to;
  set<unsigned> edge_oriented;
  basedvector<Z2, 1> edge_label;
  
  map<unsigned, set<unsigned> > incident_edges;
  
public:
  graph (set<pair<unsigned, unsigned> > G2cx);
  ~graph () { }
  
  void add_edge (unsigned from_z, unsigned from_y,
		 unsigned to_z, unsigned to_y,
		 Z2 label,
		 bool oriented);
  
  unsigned num_components () const;
  Z2 f () const; // sum of weights
  Z2 g () const;
};

graph::graph (set<pair<unsigned, unsigned> > G2cx)
  : n_vertices(G2cx.card ()),
    n_edges(0),
    z(n_vertices),
    y(n_vertices)
{
  unsigned j = 1;
  for (set_const_iter<pair<unsigned, unsigned> > i = G2cx; i; i ++, j ++)
    {
      z[j] = i.val ().first;
      y[j] = i.val ().second;
      zy_vertex.push (i.val (), j);
    }
#ifndef NDEBUG
  for (unsigned i = 1; i <= n_vertices; i ++)
    {
      assert (i == vertex (z[i], y[i]));
    }
#endif
  assert (j == n_vertices + 1);
}

void
graph::add_edge (unsigned from_z, unsigned from_y,
		 unsigned to_z, unsigned to_y,
		 Z2 label,
		 bool oriented)
{
  unsigned e = ++ n_edges;
  
  unsigned from = vertex (from_z, from_y),
    to = vertex (to_z, to_y);
  
  edge_from.append (from);
  edge_to.append (to);
  edge_label.append (label);
  assert (e == edge_label.size ());
  
  if (oriented)
    edge_oriented.push (e);
  
  incident_edges[from].push (e);
  incident_edges[to].push (e);
}

unsigned
graph::num_components () const
{
  unionfind<1> u (n_vertices);
  for (unsigned i = 1; i <= n_edges; i ++)
    u.join (edge_from[i], edge_to[i]);
  return u.num_sets ();
}

Z2
graph::f () const
{
  Z2 a = 0;
  for (unsigned i = 1; i <= n_edges; i ++)
    a += edge_label[i];
  return a;
}

Z2
graph::g () const
{
  set<unsigned> done;
  
  set<unsigned> A,
    B;
  for (unsigned i = 1; i <= n_edges; i ++)
    {
      if (done % i)
	continue;
      
      unsigned j = i;
      unsigned v = edge_to[j];
      for (;;)
	{
	  if (edge_oriented % j)
	    {
	      if (edge_to[j] == v)
		A.push (j);
	      else
		B.push (j);
	    }
	  done.push (j);
	  
	  if (incident_edges[v].head () == j)
	    j = incident_edges[v].tail ();
	  else
	    {
	      assert (j == incident_edges[v].tail ());
	      j = incident_edges[v].head ();
	    }
	  if (v == edge_to[j])
	    v = edge_from[j];
	  else
	    {
	      assert (v == edge_from[j]);
	      v = edge_to[j];
	    }
	  
	  if (j == i)
	    {
	      assert (v == edge_to[i]);
	      break;
	    }
	}
    }
  
  assert (Z2 (A.card ()) == Z2 (B.card ()));
  return Z2 (A.card ());
}

pair<set<unsigned>,
     map<unsigned, unsigned> > 
steenrod_square::ladybug_matching (unsigned x, unsigned y) const
{
  set<unsigned> Gxy;
  map<unsigned, unsigned> lxy;
  
  for (linear_combination_const_iter<Z2> zz = d[y]; zz; zz ++)
    {
      unsigned z = zz.key ();
      
      if (d[z](x) == 1)
	Gxy.push (z);
    }
  
  if (Gxy.card () == 2)
    {
      unsigned z1 = Gxy.head (),
	z2 = Gxy.tail ();
      assert (z1 != z2);
      
      lxy.push (z1, z2);
      lxy.push (z2, z1);
    }
  else if (Gxy.card () == 4)
    {
      pair<unsigned, unsigned> x_sm = cor.generator_state_monomial (x),
	y_sm = cor.generator_state_monomial (y);
      
      unsigned changed = x_sm.first ^ y_sm.first;
      assert (unsigned_bitcount (changed) == 2);
      unsigned i = unsigned_ffs (changed),
	j = unsigned_ffs (unsigned_bitclear (changed, i));
      
      unsigned s00 = y_sm.first;
      unsigned s01 = unsigned_bitset (s00, i);
      unsigned s10 = unsigned_bitset (s00, j);
      
      smoothing from_s (cor.kd, smallbitset (cor.n_crossings, s00));
      
      basedvector<unsigned, 1> from_circle_edge_rep (from_s.n_circles);
      for (unsigned j = 1; j <= cor.kd.num_edges (); j ++)
	from_circle_edge_rep[from_s.edge_circle[j]] = j;
      
      unsigned e1 = cor.kd.ept_edge (cor.kd.crossings[i][1]),
	e3 = cor.kd.ept_edge (cor.kd.crossings[i][3]);
      
      unsigned p = from_s.edge_circle[e1];
      assert (p == from_s.edge_circle[e3]);
      assert (unsigned_bittest (y_sm.second, p));  // p+
      
      smoothing conf01 (cor.kd, smallbitset (cor.n_crossings, s01)),
	conf10 (cor.kd, smallbitset (cor.n_crossings, s10));
      
      assert (conf01.edge_circle[e1] != conf01.edge_circle[e3]);
      assert (conf10.edge_circle[e1] != conf10.edge_circle[e3]);
      
      unsigned m01 = 0,
	m10 = 0;
      for (unsigned i = 1; i <= from_s.n_circles; i ++)
	{
	  if (unsigned_bittest (y_sm.second, i))
	    {
	      m01 = unsigned_bitset (m01, conf01.edge_circle[from_circle_edge_rep[i]]);
	      m10 = unsigned_bitset (m10, conf10.edge_circle[from_circle_edge_rep[i]]);
	    }
	}
      
      unsigned m01_a = unsigned_bitclear (unsigned_bitset (m01,
							   conf01.edge_circle[e1]),
					  conf01.edge_circle[e3]),
	m10_a = unsigned_bitclear (unsigned_bitset (m10,
						    conf10.edge_circle[e1]),
				   conf10.edge_circle[e3]);
      unsigned z01_a = cor.generator (s01, m01_a),
	z10_a = cor.generator (s10, m10_a);
      assert (Gxy(z01_a) && Gxy(z10_a));
      lxy.push (z01_a, z10_a);
      lxy.push (z10_a, z01_a);
      
      unsigned m01_b = unsigned_bitclear (unsigned_bitset (m01,
							   conf01.edge_circle[e3]),
					  conf01.edge_circle[e1]),
	m10_b = unsigned_bitclear (unsigned_bitset (m10,
						    conf10.edge_circle[e3]),
				   conf10.edge_circle[e1]);
      unsigned z01_b = cor.generator (s01, m01_b),
	z10_b = cor.generator (s10, m10_b);
      assert (Gxy(z01_b) && Gxy(z10_b));
      lxy.push (z01_b, z10_b);
      lxy.push (z10_b, z01_b);
    }
  else
    assert (Gxy.card () == 0);
  
  assert (Gxy.card () == lxy.card ());
  
  return pair<set<unsigned>, map<unsigned, unsigned> > (Gxy, lxy);
}

Z2
steenrod_square::sq2_coeff (grading cgr,
			    linear_combination<Z2> c,
			    unsigned x) const
{
  set<pair<unsigned, unsigned> > G2cx = make_G2cx (cgr, c, x);
  
  graph G (G2cx);
  
  set<unsigned> ys;
  for (unsigned i = 1; i <= G.n_vertices; i ++)
    ys += G.y[i];
  
  for (set_const_iter<unsigned> yy = ys; yy; yy ++)
    {
      unsigned y = yy.val ();
      
      pair<set<unsigned>,
	   map<unsigned, unsigned> > p = ladybug_matching (x, y);
      
      set<unsigned> Gxy = p.first;
      map<unsigned, unsigned> lxy = p.second;
      
      for (set_const_iter<unsigned> zz = Gxy; zz; zz ++)
	{
	  unsigned z = zz.val ();
	  unsigned lxyz = lxy(z);
	  if (z < lxyz)
	    {
	      G.add_edge (z, y,
			  lxyz, y,
			  fC (x, y), 0);
	    }
	}
    }
  
  set<unsigned> zs;
  for (unsigned i = 1; i <= G.n_vertices; i ++)
    zs += G.z[i];
  
  for (set_const_iter<unsigned> zz = zs; zz; zz ++)
    {
      unsigned z = zz.val ();
      triple<set<unsigned>,
	     map<unsigned, unsigned>,
	     map<unsigned, Z2> > t = boundary_matching (cgr, c, z);
      
      set<unsigned> Gcz = t.first;
      map<unsigned, unsigned> bz = t.second;
      map<unsigned, Z2> sz = t.third;
      
      for (set_const_iter<unsigned> yy = Gcz; yy; yy ++)
	{
	  unsigned y = yy.val ();
	  
	  unsigned bzy = bz(y);
	  if (y < bzy)
	    {
	      if (sz(y) == 0
		  && sz(bzy) == 1)
		{
		  G.add_edge (z, y,
			      z, bzy,
			      0, 1);
		}
	      else if (sz(y) == 1
		       && sz(bzy) == 0)
		{
		  G.add_edge (z, bzy,
			      z, y,
			      0, 1);
		}
	      else
		{
		  assert (sz(y) == sz(bzy));
		  G.add_edge (z, y,
			      z, bzy,
			      0, 0);
		}
	    }
	}
    }
  
#ifndef NDEBUG
  for (unsigned i = 1; i <= G.n_edges; i ++)
    {
      assert (G.incident_edges[i].card () == 2);
    }
#endif
  
  // printf ("G.n_edges = %d\n", G.n_edges);
  // printf ("#|G| = %d\n", G.num_components ());
  
  return (Z2 (G.num_components ())
	  + G.f ()
	  + G.g ());
}

mod_map<Z2>
steenrod_square::sq2 () const
{
  map_builder<Z2> b (s.new_C);
  
  for (unsigned i = 1; i <= s.new_C->dim (); i ++)
    {
      grading cgr = s.new_C->generator_grading (i);
      linear_combination<Z2> c = s.iota[i];
      assert (d.map (c) == 0);
      
      grading gr2 (cgr.h + 2, cgr.q);
      if (! (KGij % gr2))
	continue;
      
      linear_combination<Z2> sq2c (cor.khC);
      for (set_const_iter<unsigned> x = KGij[gr2]; x; x ++)
	sq2c.muladd (sq2_coeff (cgr, c, x.val ()), x.val ());
      
      // display ("sq2c:\n", sq2c);
      assert (d.map (sq2c) == 0);
      
      b[i].muladd (1, s.pi.map (sq2c));
    }
  
  return mod_map<Z2> (b);
}
