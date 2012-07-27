
#include <knotkit.h>

class rd_unify
{
public:
  ullmanmap<unsigned, 1> cpt_map,
    starting_circle_map,
    ending_circle_map;
  
  bool unify_starting_circle (const resolution_diagram &rd1, const resolution_diagram &rd2,
			      unsigned x1, unsigned x2);
  bool unify_ending_circle (const resolution_diagram &rd1, const resolution_diagram &rd2,
			    unsigned y1, unsigned y2);
  bool unify_crossing (const resolution_diagram &rd1, const resolution_diagram &rd2,
		       unsigned c1, unsigned c2);
  bool unify_cpt (const resolution_diagram &rd1, const resolution_diagram &rd2,
		  unsigned c1, unsigned c2);
  
public:
  rd_unify ();
  rd_unify (const rd_unify &) = delete;
  ~rd_unify () { }
  
  rd_unify &operator = (const rd_unify &) = delete;
  
  bool unify (const resolution_diagram &rd1, const resolution_diagram &rd2);
  unsigned map_starting_monomial (unsigned m);
  unsigned map_ending_monomial (unsigned m);
};

bool
rd_unify::unify_starting_circle (const resolution_diagram &rd1, const resolution_diagram &rd2,
				 unsigned x1, unsigned x2)
{
  pair<unsigned &, bool> p = starting_circle_map.find (x1);
  if (p.second)
    return p.first == x2;
  
  p.first = x2;
  return 1;
}

bool
rd_unify::unify_ending_circle (const resolution_diagram &rd1, const resolution_diagram &rd2,
			       unsigned y1, unsigned y2)
{
  pair<unsigned &, bool> p = ending_circle_map.find (y1);
  if (p.second)
    return p.first == y2;
  
  p.first = y2;
  return 1;
}

bool
rd_unify::unify_crossing (const resolution_diagram &rd1, const resolution_diagram &rd2,
			  unsigned c1, unsigned c2)
{
  return (rd1.resolved(c1) == rd2.resolved(c2)
	  && rd1.to_resolve(c1) == rd2.to_resolve(c2)
	  && unify_cpt (rd1, rd2,
			rd1.crossing_from_cpt (c1), rd2.crossing_from_cpt (c2))
	  && unify_cpt (rd1, rd2,
			rd1.crossing_to_cpt (c1), rd2.crossing_to_cpt (c2)));
}

bool
rd_unify::unify_cpt (const resolution_diagram &rd1, const resolution_diagram &rd2,
		     unsigned c1, unsigned c2)
{
  pair<unsigned &, bool> p = cpt_map.find (c1);
  if (p.second)
    return p.first == c2;
  
  p.first = c2;
  return (rd1.is_from_cpt (c1) == rd2.is_from_cpt (c2)
	  && rd1.cpt_inside(c1) == rd2.cpt_inside(c2)
	  && (rd1.marked_edge == c1) == (rd2.marked_edge == c2)
	  && (rd1.marked_edge2 == c1) == (rd2.marked_edge2 == c2)
	  && unify_crossing (rd1, rd2,
			     rd1.cpt_crossing (c1), rd2.cpt_crossing (c2))
	  && unify_starting_circle (rd1, rd2,
				    rd1.cpt_starting_circle[c1], rd2.cpt_starting_circle[c2])
	  && unify_ending_circle (rd1, rd2,
				  rd1.cpt_ending_circle[c1], rd2.cpt_ending_circle[c2])
	  && unify_cpt (rd1, rd2,
			rd1.next[c1], rd2.next[c2])
	  && unify_cpt (rd1, rd2,
			rd1.prev[c1], rd2.prev[c2]));
}

rd_unify::rd_unify ()
  : cpt_map(max_cpts),
    starting_circle_map(max_circles),
    ending_circle_map(max_circles)
{
}

bool
rd_unify::unify (const resolution_diagram &rd1,
		 const resolution_diagram &rd2)
{
  if (rd1.n_crossings != rd2.n_crossings
      || rd1.n_starting_circles != rd2.n_starting_circles
      || rd1.n_ending_circles != rd2.n_ending_circles
      || (rd1.marked_edge == 0) != (rd2.marked_edge == 0)
      || (rd1.marked_edge2 == 0) != (rd2.marked_edge2 == 0))
    return 0;
  
#if 0
  cpt_map.clear ();
  starting_circle_map.clear ();
  ending_circle_map.clear ();
  for (unsigned i = 1; i <= rd1.n_crossings; i ++)
    {
      if (!unify_crossing (rd1, rd2, i, i))
	return 0;
    }
  assert (cpt_map.card () == rd1.num_cpts ());
  assert (starting_circle_map.card () == rd1.n_starting_circles);
  assert (ending_circle_map.card () == rd1.n_ending_circles);
#endif  
  
  for (unsigned i = 1; i <= rd1.n_crossings; i ++)
    {
      cpt_map.clear ();
      starting_circle_map.clear ();
      ending_circle_map.clear ();
      if (unify_crossing (rd1, rd2, i, 1))
	{
	  assert (cpt_map.card () == rd1.num_cpts ());
	  assert (starting_circle_map.card () == rd1.n_starting_circles);
	  assert (ending_circle_map.card () == rd1.n_ending_circles);
	  return 1;
	}
    }
  
  return 0;
}

unsigned
rd_unify::map_starting_monomial (unsigned m1)
{
  unsigned m2 = 0;
  for (unsigned_const_iter i = m1; i; i ++)
    {
      m2 = unsigned_bitset (m2,
			    starting_circle_map(i.val ()));
    }
  return m2;
}

unsigned
rd_unify::map_ending_monomial (unsigned m1)
{
  unsigned m2 = 0;
  for (unsigned_const_iter i = m1; i; i ++)
    {
      m2 = unsigned_bitset (m2,
			    ending_circle_map(i.val ()));
    }
  return m2;
}

rd_unify the_rd_unifier;

bool
resolution_diagram::crossing_orientation (unsigned common, unsigned i) const
{
  if (crossing_is_split (i))
    {
      if (crossing_from_outside (i))
	return next[crossing_from_cpt (i)] == crossing_to_cpt (i);
      else
	return next[crossing_to_cpt (i)] == crossing_from_cpt (i);
    }
  else
    return crossing_to (i) == common;
}

bool
resolution_diagram::marked_edge_left (unsigned pt) const
{
  assert (marked_edge);
  
  if (cpt_inside % pt)
    return marked_edge == pt;
  else
    return marked_edge == prev[pt];
}

resolution_diagram::resolution_diagram ()
  : n_crossings(0),
    n_starting_circles(0),
    n_ending_circles(0),
    marked_edge(0),
    marked_edge2(0)
{
}

void
resolution_diagram::compute_prev ()
{
  for (unsigned i = 1; i <= num_cpts (); i ++)
    prev[next[i]] = i;
}

void
resolution_diagram::check ()
{
  for (unsigned i = 1; i <= num_cpts (); i ++)
    {
      assert (prev[next[i]] == i);
      assert (next[prev[i]] == i);
      assert (cpt_starting_circle[i] == cpt_starting_circle[next[i]]);
      assert (1 <= cpt_starting_circle[i]
	      && cpt_starting_circle[i] <= n_starting_circles);
      assert (1 <= cpt_ending_circle[i]
	      && cpt_ending_circle[i] <= n_ending_circles);
    }
}

resolution_diagram::resolution_diagram (unsigned n_crossings_,
					unsigned n_starting_circles_,
					unsigned n_ending_circles_)
  : n_crossings(n_crossings_),
    resolved(n_crossings_),
    to_resolve(n_crossings_, ~0),
    n_starting_circles(n_starting_circles_),
    n_ending_circles(n_ending_circles_),
    marked_edge(0),
    marked_edge2(0),
    cpt_inside(num_cpts ()),
    prev(num_cpts ()),
    next(num_cpts ()),
    cpt_starting_circle(num_cpts ()),
    cpt_ending_circle(num_cpts ())
{
}

resolution_diagram::resolution_diagram (reverse_crossing_t,
					const resolution_diagram &rd,
					smallbitset to_reverse)
  : n_crossings(rd.n_crossings),
    resolved(rd.resolved),
    to_resolve(rd.to_resolve),
    n_starting_circles(rd.n_starting_circles),
    n_ending_circles(rd.n_ending_circles),
    marked_edge(rd.marked_edge),
    marked_edge2(rd.marked_edge2),
    cpt_inside(rd.cpt_inside),
    prev(num_cpts ()),
    next(num_cpts ()),
    cpt_starting_circle(num_cpts ()),
    cpt_ending_circle(num_cpts ())
{
  map<unsigned, unsigned> m;
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      if (to_reverse % i)
	{
	  m.push (crossing_from_cpt (i),
		  crossing_to_cpt (i));
	  m.push (crossing_to_cpt (i),
		  crossing_from_cpt (i));
	}
      else
	{
	  m.push (crossing_from_cpt (i),
		  crossing_from_cpt (i));
	  m.push (crossing_to_cpt (i),
		  crossing_to_cpt (i));
	}
    }
  
  for (unsigned i = 1; i <= num_cpts (); i ++)
    {
      next[i] = m(rd.next[m(i)]);
      prev[i] = m(rd.prev[m(i)]);
      
      cpt_starting_circle[i] = rd.cpt_starting_circle[m(i)];
      cpt_ending_circle[i] = rd.cpt_ending_circle[m(i)];
    }
  
#ifndef NDEBUG
  check ();
#endif
}

resolution_diagram::resolution_diagram (reverse_orientation_t,
					const resolution_diagram &rd)
  : n_crossings(rd.n_crossings),
    resolved(rd.resolved),
    to_resolve(rd.to_resolve),
    n_starting_circles(rd.n_starting_circles),
    n_ending_circles(rd.n_ending_circles),
    marked_edge(rd.marked_edge),
    marked_edge2(rd.marked_edge2),
    cpt_inside(rd.cpt_inside),
    prev(num_cpts (), rd.next),
    next(num_cpts (), rd.prev),
    cpt_starting_circle(num_cpts (), rd.cpt_starting_circle),
    cpt_ending_circle(num_cpts (), rd.cpt_ending_circle)
{
  cpt_inside.toggle ();
  
#ifndef NDEBUG
  check ();
#endif
}

resolution_diagram::resolution_diagram (const resolution_diagram &rd)
  : n_crossings(rd.n_crossings),
    resolved(rd.resolved),
    to_resolve(rd.to_resolve),
    n_starting_circles(rd.n_starting_circles),
    n_ending_circles(rd.n_ending_circles),
    marked_edge(rd.marked_edge),
    marked_edge2(rd.marked_edge2),
    cpt_inside(rd.cpt_inside),
    prev(num_cpts (), rd.prev),
    next(num_cpts (), rd.next),
    cpt_starting_circle(num_cpts (), rd.cpt_starting_circle),
    cpt_ending_circle(num_cpts (), rd.cpt_ending_circle)
{
}

resolution_diagram::resolution_diagram (copy, const resolution_diagram &rd)
  : n_crossings(rd.n_crossings),
    resolved(rd.resolved),
    to_resolve(rd.to_resolve),
    n_starting_circles(rd.n_starting_circles),
    n_ending_circles(rd.n_ending_circles),
    marked_edge(rd.marked_edge),
    marked_edge2(rd.marked_edge2),
    cpt_inside(rd.cpt_inside),
    prev(num_cpts (), rd.prev),
    next(num_cpts (), rd.next),
    cpt_starting_circle(num_cpts (), rd.cpt_starting_circle),
    cpt_ending_circle(num_cpts (), rd.cpt_ending_circle)
{
}

resolution_diagram::resolution_diagram (reader &r)
{
  read (r, n_crossings);
  read (r, resolved);
  read (r, to_resolve);
  read (r, n_starting_circles);
  read (r, n_ending_circles);
  read (r, marked_edge);
  read (r, marked_edge2);
  read (r, cpt_inside);
  read (r, next);
  read (r, cpt_starting_circle);
  read (r, cpt_ending_circle);
  
  prev = basedvector<unsigned, 1> (num_cpts ());
  for (unsigned i = 1; i <= num_cpts (); i ++)
    prev[next[i]] = i;
}

resolution_diagram &
resolution_diagram::operator = (const resolution_diagram &rd)
{
  n_crossings = rd.n_crossings;
  resolved = rd.resolved;
  to_resolve = rd.to_resolve;
  n_starting_circles = rd.n_starting_circles;
  n_ending_circles = rd.n_ending_circles;
  marked_edge = rd.marked_edge;
  marked_edge2 = rd.marked_edge2;
  cpt_inside = rd.cpt_inside;
  prev = rd.prev;
  next = rd.next;
  cpt_starting_circle = rd.cpt_starting_circle;
  cpt_ending_circle = rd.cpt_ending_circle;
  return *this;
}

bool
resolution_diagram::operator == (const resolution_diagram &rd) const
{
  return the_rd_unifier.unify (*this, rd);
}

void
resolution_diagram::write_self (writer &w) const
{
  write (w, n_crossings);
  write (w, resolved);
  write (w, to_resolve);
  write (w, n_starting_circles);
  write (w, n_ending_circles);
  write (w, marked_edge);
  write (w, marked_edge2);
  write (w, cpt_inside);
  write (w, next);
  write (w, cpt_starting_circle);
  write (w, cpt_ending_circle);
}

hash_t
resolution_diagram::hash_self () const
{
  assert (cpt_inside.size () == num_cpts ());
  
  hash_t h = hash (n_crossings);
  h = hash_combine (h, hash (n_starting_circles));
  h = hash_combine (h, hash (n_ending_circles));
  h = hash_combine (h, hash (marked_edge == 0));
  h = hash_combine (h, hash (marked_edge2 == 0));
  return h;
}

resolution_diagram_builder::resolution_diagram_builder ()
  : gl_crossings(max_crossings),
    gl_starting_circles(max_circles),
    gl_ending_circles(max_circles)
{
  rd.prev = basedvector<unsigned, 1> (max_cpts);
  rd.next = basedvector<unsigned, 1> (max_cpts);
  rd.cpt_starting_circle = basedvector<unsigned, 1> (max_cpts);
  rd.cpt_ending_circle = basedvector<unsigned, 1> (max_cpts);
}

void
resolution_diagram_builder::init (const knot_diagram &d,
				  smallbitset from_state, const smoothing &from_s,
				  smallbitset to_state, const smoothing &to_s,
				  smallbitset crossings)
{
  gl_crossings.clear ();
  gl_starting_circles.clear ();
  gl_ending_circles.clear ();
  
  for (smallbitset_const_iter i = crossings; i; i ++)
    {
      unsigned c = i.val ();
      
      gl_crossings += c;
      
      unsigned starting_from = from_s.ept_circle (d, d.crossings[c][2]),
	starting_to = from_s.ept_circle (d, d.crossings[c][4]);
      gl_starting_circles += starting_from;
      gl_starting_circles += starting_to;
      
      unsigned ending_from = to_s.ept_circle (d, d.crossings[c][2]),
	ending_to = to_s.ept_circle (d, d.crossings[c][4]);
      gl_ending_circles += ending_from;
      gl_ending_circles += ending_to;
    }
  
#ifndef NDEBUG
  for (ullmanset_const_iter<1> i = gl_crossings; i; i ++)
    {
      unsigned c1 = i.val ();
      unsigned lc1 = i.pos () + 1;
      for (ullmanset_const_iter<1> j = gl_crossings; j; j ++)
	{
	  unsigned c2 = j.val ();
	  unsigned lc2 = j.pos () + 1;
	  assert ((c1 < c2) == (lc1 < lc2));
	}
    }
#endif
  
  rd.n_crossings = gl_crossings.card ();
  rd.resolved = smallbitset (rd.n_crossings);
  rd.to_resolve = smallbitset (rd.n_crossings);
  rd.n_starting_circles = gl_starting_circles.card ();
  rd.n_ending_circles = gl_ending_circles.card ();
  rd.marked_edge = 0;
  rd.marked_edge2 = 0;
  rd.cpt_inside = smallbitset (rd.num_cpts ());
  
  for (smallbitset_const_iter i = crossings; i; i ++)
    {
      unsigned c = i.val ();
      unsigned lc = gl_crossings.position (c) + 1;
      
      if (from_state(c))
	rd.resolved.push (lc);
      else if (to_state(c))
	rd.to_resolve.push (lc);
      
      if (from_s.crossing_from_inside (d, from_state, c))
	rd.cpt_inside.push (rd.crossing_from_cpt (lc));
      if (from_s.crossing_to_inside (d, from_state, c))
	rd.cpt_inside.push (rd.crossing_to_cpt (lc));
      
      unsigned from = from_s.ept_circle (d, d.crossings[c][2]),
	to = from_s.ept_circle (d, d.crossings[c][4]);
      unsigned lfrom = gl_starting_circles.position (from) + 1,
	lto = gl_starting_circles.position (to) + 1;
      rd.cpt_starting_circle[rd.crossing_from_cpt (lc)] = lfrom;
      rd.cpt_starting_circle[rd.crossing_to_cpt (lc)] = lto;
      
      unsigned ending_from = to_s.ept_circle (d, d.crossings[c][2]),
	ending_to = to_s.ept_circle (d, d.crossings[c][4]);
      unsigned lending_from = gl_ending_circles.position (ending_from) + 1,
	lending_to = gl_ending_circles.position (ending_to) + 1;
      rd.cpt_ending_circle[rd.crossing_from_cpt (lc)] = lending_from;
      rd.cpt_ending_circle[rd.crossing_to_cpt (lc)] = lending_to;
    }
  
  lg_edges = basedvector<set<unsigned>, 1> (rd.num_cpts ());
  
  smallbitset done (d.num_edges ());
  for (unsigned i = 1; i <= d.num_edges (); i ++)
    {
      if (done % i)
	continue;
      
      bool saw_marked_edge = 0,
	first_saw_marked_edge = 0;
      unsigned first_lcpt = 0,
	prev_lcpt = 0;
      set<unsigned> first_saw_gedges;
      for (unsigned e = i;;)
	{
	  done.push (e);
	  
	  if (e == d.marked_edge)
	    saw_marked_edge = 1;
	  
	  if (prev_lcpt)
	    lg_edges[prev_lcpt].push (e);
	  else
	    first_saw_gedges.push (e);
	  
	  unsigned p = from_s.edge_to (d, e);
	  unsigned c = d.ept_crossing (p);
	  if (crossings % c)
	    {
	      unsigned lc = gl_crossings.position (c) + 1;
	      unsigned next_lcpt = from_s.is_crossing_from_ept (d, from_state, p)
		? rd.crossing_from_cpt (lc)
		: rd.crossing_to_cpt (lc);
	      
	      if (!first_lcpt)
		{
		  assert (prev_lcpt == 0);
		  first_lcpt = next_lcpt;
		  
		  assert (first_saw_marked_edge == 0);
		  first_saw_marked_edge = saw_marked_edge;
		  saw_marked_edge = 0;
		}
	      if (prev_lcpt)
		{
		  rd.next[prev_lcpt] = next_lcpt;
		  rd.prev[next_lcpt] = prev_lcpt;
		  if (saw_marked_edge)
		    {
		      assert (rd.marked_edge == 0);
		      rd.marked_edge = prev_lcpt;
		    }
		  saw_marked_edge = 0;
		}
	      prev_lcpt = next_lcpt;
	    }
	  
	  e = from_s.next_edge (d, from_state, e);
	  if (e == i)
	    break;
	}
      if (first_lcpt)
	{
	  assert (prev_lcpt);
	  
	  rd.next[prev_lcpt] = first_lcpt;
	  rd.prev[first_lcpt] = prev_lcpt;
	  if (saw_marked_edge || first_saw_marked_edge)
	    {
	      assert (rd.marked_edge == 0);
	      rd.marked_edge = prev_lcpt;
	      first_saw_marked_edge = saw_marked_edge = 0;
	    }
	  
	  lg_edges[prev_lcpt] |= first_saw_gedges;
	}
      else
	assert (!prev_lcpt);
      assert (!first_saw_marked_edge);
    }
#ifndef NDEBUG
  if (rd.marked_edge)
    {
      assert (d.marked_edge);
      assert (rd.marked_starting_circle ()
	      == gl_starting_circles.position (from_s.edge_circle[d.marked_edge]) + 1);
#if 0
      assert (rd.marked_ending_circle ()
	      == gl_ending_circles.position (to_s.edge_circle[d.marked_edge]) + 1);
#endif
    }
  else if (d.marked_edge)
    {
      assert (!gl_starting_circles(from_s.edge_circle[d.marked_edge]));
      assert (!gl_ending_circles(to_s.edge_circle[d.marked_edge]));
    }
#endif
  assert (done.card () == d.num_edges ());
}

void
resolution_diagram_builder::mirror ()
{
  abort (); // ???
}

void
resolution_diagram_builder::reverse_orientation ()
{
  abort (); // ???
}

void
resolution_diagram_builder::reverse_crossing (const knot_diagram &d,
					      const smoothing &from_s, const smoothing &to_s,
					      unsigned c)
{
#ifndef NDEBUG
  for (unsigned i = 1; i <= rd.num_cpts (); i ++)
    {
      assert (rd.prev[rd.next[i]] == i);
      assert (rd.next[rd.prev[i]] == i);
    }
#endif
  
  if (! (gl_crossings % c))
    return;
  
  unsigned lc = gl_crossings.position (c) + 1;
  unsigned from = rd.crossing_from_cpt (lc),
    to = rd.crossing_to_cpt (lc);
  
  unsigned from_prev = rd.prev[from],
    from_next = rd.next[from],
    to_prev = rd.prev[to],
    to_next = rd.next[to];
      
  if ((from_next == to
       && to_next == from)
      || (from_next == from
	  && to_next == to))
    ;
  else if (from_next == to)
    {
      rd.next[from_prev] = to;
      rd.prev[to] = from_prev;
      rd.next[to] = from;
      rd.prev[from] = to;
      rd.next[from] = to_next;
      rd.prev[to_next] = from;
    }
  else if (to_next == from)
    {
      rd.next[to_prev] = from;
      rd.prev[from] = to_prev;
      rd.next[from] = to;
      rd.prev[to] = from;
      rd.next[to] = from_next;
      rd.prev[from_next] = to;
    }
  else if (from_next == from)
    {
      rd.next[from] = to_next;
      rd.prev[from] = to_prev;
      rd.next[to_prev] = from;
      rd.prev[to_next] = from;
	  
      rd.prev[to] = rd.next[to] = to;
    }
  else if (to_next == to)
    {
      rd.next[to] = from_next;
      rd.prev[to] = from_prev;
      rd.next[from_prev] = to;
      rd.prev[from_next] = to;
	  
      rd.prev[from] = rd.next[from] = from;
    }
  else
    {
      rd.prev[from] = to_prev;
      rd.next[from] = to_next;
      rd.prev[to] = from_prev;
      rd.next[to] = from_next;
	  
      rd.next[from_prev] = to;
      rd.prev[from_next] = to;
      rd.next[to_prev] = from;
      rd.prev[to_next] = from;
    }
  
#ifndef NDEBUG
  for (unsigned i = 1; i <= rd.num_cpts (); i ++)
    {
      assert (rd.prev[rd.next[i]] == i);
      assert (rd.next[rd.prev[i]] == i);
    }
#endif
  
  bool from_inside = rd.cpt_inside % from,
    to_inside = rd.cpt_inside % to;
  rd.cpt_inside -= from;
  rd.cpt_inside -= to;
  if (from_inside)
    rd.cpt_inside.push (to);
  if (to_inside)
    rd.cpt_inside.push (from);
  
  unsigned cfrom = rd.cpt_starting_circle[from],
    cto = rd.cpt_starting_circle[to],
    c_ending_from = rd.cpt_ending_circle[from],
    c_ending_to = rd.cpt_ending_circle[to];
  
  rd.cpt_starting_circle[from] = cto;
  rd.cpt_starting_circle[to] = cfrom;
  rd.cpt_ending_circle[from] = c_ending_to;
  rd.cpt_ending_circle[to] = c_ending_from;
  
  if (rd.marked_edge == from)
    rd.marked_edge = to;
  else if (rd.marked_edge == to)
    rd.marked_edge = from;
  
  set<unsigned> t = lg_edges[from];
  lg_edges[from] = lg_edges[to];
  lg_edges[to] = t;
  
#if 0
  gl_starting_circles.clear ();
  gl_ending_circles.clear ();
  for (ullmanset<1>::const_iter i = gl_crossings; i; i ++)
    {
      unsigned c2 = i.val (),
	lc2 = i.pos () + 1;
	  
      unsigned from, to, left, right;
      if (c2 == c)
	{
	  from = from_s.ept_circle (d, d.crossings[c2][3]);
	  to = from_s.ept_circle (d, d.crossings[c2][1]);
	  
	  left = to_s.ept_circle (d, d.crossings[c2][3]);
	  right = to_s.ept_circle (d, d.crossings[c2][1]);
	}
      else
	{
	  from = from_s.ept_circle (d, d.crossings[c2][1]);
	  to = from_s.ept_circle (d, d.crossings[c2][3]);
	      
	  left = to_s.ept_circle (d, d.crossings[c2][1]);
	  right = to_s.ept_circle (d, d.crossings[c2][3]);
	}
      
      gl_starting_circles += from;
      gl_starting_circles += to;
      gl_ending_circles += left;
      gl_ending_circles += right;
      
      unsigned lfrom = gl_starting_circles.position (from) + 1,
	lto = gl_starting_circles.position (to) + 1;
      unsigned lleft = gl_ending_circles.position (left) + 1,
	lright = gl_ending_circles.position (right) + 1;
      
      rd.cpt_starting_circle[rd.crossing_to_cpt (lc2)] = lto;
      rd.cpt_starting_circle[rd.crossing_from_cpt (lc2)] = lfrom;
      
      rd.cpt_ending_circle[rd.crossing_to_cpt (lc2)] = lleft;
      rd.cpt_ending_circle[rd.crossing_from_cpt (lc2)] = lright;
    }
#endif
}

void
resolution_diagram::reverse ()
{
  basedvector<unsigned, 1> new_prev (num_cpts ()),
    new_next (num_cpts ()),
    new_cpt_starting_circle (num_cpts ()),
    new_cpt_ending_circle (num_cpts ());
  
  for (unsigned i = 1; i <= num_cpts (); i ++)
    {
      unsigned j = other_cpt (i);
      new_prev[i] = other_cpt (prev[j]);
      new_next[i] = other_cpt (next[j]);
      new_cpt_starting_circle[i] = cpt_starting_circle[j];
      new_cpt_ending_circle[i] = cpt_ending_circle[j];
    }
  unsigned new_marked_edge = marked_edge ? other_cpt (marked_edge) : 0;
  
  prev = new_prev;
  next = new_next;
  cpt_starting_circle = new_cpt_starting_circle;
  cpt_ending_circle = new_cpt_ending_circle;
  marked_edge = new_marked_edge;
}

void
resolution_diagram::reverse_orientation ()
{
  if (marked_edge)
    marked_edge = next[marked_edge];
  
  for (unsigned i = 1; i <= num_cpts (); i ++)
    std::swap (prev[i], next[i]);
  cpt_inside.toggle ();
}

knot_diagram
resolution_diagram::as_knot_diagram () const
{
  unsigned n_edges = num_cpts ();
  unsigned n_epts = n_edges * 2;
  
  basedvector<basedvector<unsigned, 1>, 1> crossings (n_crossings);
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      basedvector<unsigned, 1> v (4);
      v[1] = v[2] = v[3] = v[4] = 0;
      crossings[i] = v;
    }
  
  for (unsigned pt = 1; pt <= num_cpts (); pt ++)
    {
      unsigned c = cpt_crossing (pt);
      
      unsigned prev_pt = prev[pt];
      unsigned prev_c = cpt_crossing (prev_pt);
      
      if (is_to_cpt (pt))
	{
	  assert (crossings[c][3] == 0
		  && crossings[c][4] == 0);
	  if (cpt_inside % pt)
	    {
	      crossings[c][4] = 2 * pt - 1;
	      crossings[c][3] = 2 * prev_pt;
	    }
	  else
	    {
	      crossings[c][3] = 2 * pt - 1;
	      crossings[c][4] = 2 * prev_pt;
	    }
	}
      else
	{
	  assert (crossings[c][1] == 0
		  && crossings[c][2] == 0);
	  if (cpt_inside % pt)
	    {
	      crossings[c][2] = 2 * pt - 1;
	      crossings[c][1] = 2 * prev_pt;
	    }
	  else
	    {
	      crossings[c][1] = 2 * pt - 1;
	      crossings[c][2] = 2 * prev_pt;
	    }
	}
    }
  
  knot_diagram kd ("<from rd>", crossings);
  if (marked_edge)
    kd.marked_edge = marked_edge;
  return kd;
}

void
resolution_diagram::d (basedvector<pair<unsigned, unsigned>, 1> &out) const
{
  if (n_crossings == 1)
    {
      if (n_starting_circles == 1)
	{
	  assert (n_ending_circles == 2);
	  
	  /* 1 -> x + y */
	  out.append (pair<unsigned, unsigned> (unsigned_fill (n_starting_circles),
						unsigned_bitmask (1)));
	  out.append (pair<unsigned, unsigned> (unsigned_fill (n_starting_circles),
						unsigned_bitmask (2)));
	  
	  /* a -> xy */
	  out.append (pair<unsigned, unsigned> (0, 0));
	}
      else
	{
	  assert (n_starting_circles == 2);
	  
	  /* 1 -> 1 */
	  out.append (pair<unsigned, unsigned> (unsigned_fill (n_starting_circles),
						unsigned_fill (n_ending_circles)));
	  
	  /* a -> x, b -> x */
	  out.append (pair<unsigned, unsigned> (unsigned_bitmask (1), 0));
	  out.append (pair<unsigned, unsigned> (unsigned_bitmask (2), 0));
	}
      return;
    }
  
  unsigned self_crossed = 0;
  bool self_inside = 0,
    self_outside = 0;
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      unsigned from = crossing_from (i),
	to = crossing_to (i);
      
      if (from == to)
	{
	  if (self_crossed)
	    {
	      if (self_crossed != from)
		{
		  assert (out.size () == 0);
		  return;
		}
	    }
	  else
	    self_crossed = from;
	  
	  if (crossing_from_inside (i))
	    self_inside = 1;
	  else
	    self_outside = 1;
	}
    }
  
  unsigned common_from = crossing_from (1);
  for (unsigned i = 2; i <= n_crossings; i ++)
    {
      if (common_from != crossing_from (i))
	{
	  common_from = 0;
	  break;
	}
    }
  unsigned common_to = crossing_to (1);
  for (unsigned i = 2; i <= n_crossings; i ++)
    {
      if (common_to != crossing_to (i))
	{
	  common_to = 0;
	  break;
	}
    }
  
  basedvector<unsigned, 1> incoming (n_starting_circles),
    outgoing (n_starting_circles);
  for (unsigned i = 1; i <= n_starting_circles; i ++)
    incoming[i] = outgoing[i] = 0;
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      incoming[crossing_to (i)] ++;
      outgoing[crossing_from (i)] ++;
    }
  
  bool single_entry_single_exit = 1;
  unsigned double_entry_double_exit = 0;
  for (unsigned i = 1; i <= n_starting_circles; i ++)
    {
      if (incoming[i] != 1
	  || outgoing[i] != 1)
	single_entry_single_exit = 0;
      
      if (incoming[i] == 2
	  && outgoing[i] == 2)
	{
	  if (double_entry_double_exit)
	    {
	      double_entry_double_exit = 0;
	      break;
	    }
	  else
	    double_entry_double_exit = i;
	}
    }
  
  /* type A_k */
  if (common_from && common_to && n_starting_circles == 2)
    {
      /* 1 -> 1 */
      out.append (pair<unsigned, unsigned> (unsigned_fill (n_starting_circles),
					    unsigned_fill (n_ending_circles)));
    }
  
  /* type B_k */
  if (single_entry_single_exit)
    {
      /* prod a_i -> xy */
      out.append (pair<unsigned, unsigned> (0, 0));
    }
  
  /* type C_p,q */
  // ?? make this code logical like the new type E_p,q
  if (n_starting_circles == 1)
    {
      unsigned inout_changes = 0;
      for (unsigned i = 1; i <= n_crossings; i ++)
	{
	  unsigned n = next[crossing_to_cpt (i)],
	    m = next[crossing_from_cpt (i)];
	  if (crossing_from_inside (i))
	    {
	      if (is_to_cpt (n) != (cpt_inside % n))
		goto D;
	      if (is_to_cpt (m) == (cpt_inside % m))
		goto D;
	    }
	  else
	    {
	      if (!is_to_cpt (n))
		goto D;
	      if (is_to_cpt (m))
		goto D;
	    }
	  if (crossing_from_inside (i) != (cpt_inside % n))
	    inout_changes ++;
	  if (crossing_from_inside (i) != (cpt_inside % m))
	    inout_changes ++;
	}
      if (inout_changes != 4)
	goto D;
      
      /* 1 -> 1 */
      out.append (pair<unsigned, unsigned> (unsigned_fill (n_starting_circles),
					    unsigned_fill (n_ending_circles)));
    }
  
  /* case D_p,q */
 D:
  if (double_entry_double_exit)
    {
      /* check all others are single entry, single exit */
      // ??? fold into double_entry_double_exit code
      for (unsigned i = 1; i <= n_starting_circles; i ++)
	{
	  if (i != double_entry_double_exit
	      && (incoming[i] != 1 || outgoing[i] != 1))
	    goto E;
	}
      
      unsigned inside_count = 0;
      for (unsigned i = 1; i <= n_crossings; i ++)
	{
	  if (crossing_to_inside (i) && crossing_to (i) == double_entry_double_exit)
	    {
	      unsigned n = next[crossing_to_cpt (i)];
	      if ((cpt_inside % n) || is_to_cpt (n))
		goto E;
	      inside_count ++;
	    }
	  if (crossing_from_inside (i) && crossing_from (i) == double_entry_double_exit)
	    {
	      unsigned n = next[crossing_from_cpt (i)];
	      if ((cpt_inside % n) || ! is_to_cpt (n))
		goto E;
	      inside_count ++;
	    }
	}
      
      if (inside_count != 2)
	goto E;
      
      /* prod a_i -> x */
      out.append (pair<unsigned, unsigned> (0, 0));
    }
  
  /* case E_p,q */
 E:
  if (common_from || common_to)
    {
      unsigned common = common_from ? common_from : common_to;
      assert (common);
      
      /* make sure the x_i have single circles */
      for (unsigned i = 1; i <= n_starting_circles; i ++)
	{
	  if (i != common
	      && incoming[i] + outgoing[i] != 1)
	    goto Ldone;
	}
      
      for (unsigned i = 1; i <= n_crossings; i ++)
	{
	  if (crossing_is_split (i))
	    {
	      unsigned ifrom = crossing_from_cpt (i),
		ito = crossing_to_cpt (i);

	      if (next[ifrom] != ito
		  && prev[ifrom] != ito)
		goto Ldone;
	    }
	}
      
      bool orient = crossing_orientation (common, 1);
      for (unsigned i = 1; i <= n_crossings; i ++)
	{
	  bool o = crossing_orientation (common, i);
	  if (o != orient)
	    goto Ldone;
	}
      
      set<unsigned> b_i;
      for (unsigned i = 1; i <= n_crossings; i ++)
	{
	  if (crossing_is_split (i))
	    {
	      if (orient)
		b_i += crossing_ending_left (i);
	      else
		b_i += crossing_ending_right (i);
	    }
	}
      
      /* prod a_i -> x */
      out.append (pair<unsigned, unsigned> (unsigned_bitmask (common),
					    unsigned_set (b_i)));
    }
  
 Ldone: 
  return;
}

void
resolution_diagram::twisted_barE (basedvector<triple<unsigned, unsigned, set<unsigned> >, 1> &out) const
{
  if (n_crossings == 1)
    {
      if (n_starting_circles == 1)
	{
	  assert (n_ending_circles == 2);

	  unsigned a = crossing_from (1);
	  unsigned x = crossing_ending_left (1),
	    y = crossing_ending_right (1);
	  
	  unsigned e = cpt_inside % crossing_to_cpt (1)
	    ? crossing_to_cpt (1)
	    : crossing_from_cpt (1);
	  
	  set<unsigned> edges;
	  edges.push (e);
	  
  	  /* 1 -> T^x x + y */
	  out.append (triple<unsigned, unsigned, set<unsigned> >
		      (1, unsigned_bitmask (y), edges));
	  
	  out.append (triple<unsigned, unsigned, set<unsigned> >
		      (1, unsigned_bitmask (x), set<unsigned> ()));
	  
	  /* a -> xy */
	  out.append (triple<unsigned, unsigned, set<unsigned> >
		      (0, 0, set<unsigned> ()));
	}
      else
	{
	  assert (n_starting_circles == 2);
	  
	  unsigned a = crossing_from (1),
	    b = crossing_to (1);
	  unsigned x = crossing_ending_left (1);
	  
	  /* 1 -> 1 */
	  out.append (triple<unsigned, unsigned, set<unsigned> >
		      (3, 1, set<unsigned> ()));
	  
	  unsigned e = crossing_from_cpt (1);
	  
	  set<unsigned> edges;
	  edges.push (e);
	  
	  /* a -> x, b -> T^a x */
	  out.append (triple<unsigned, unsigned, set<unsigned> >
			(unsigned_bitmask (b), 0, set<unsigned> ()));
	  out.append (triple<unsigned, unsigned, set<unsigned> >
		      (unsigned_bitmask (a), 0, edges));
	}
      return;
    }
  
  unsigned common_from = crossing_from (1);
  for (unsigned i = 2; i <= n_crossings; i ++)
    {
      if (common_from != crossing_from (i))
	{
	  common_from = 0;
	  break;
	}
    }
  unsigned common_to = crossing_to (1);
  for (unsigned i = 2; i <= n_crossings; i ++)
    {
      if (common_to != crossing_to (i))
	{
	  common_to = 0;
	  break;
	}
    }
  
  basedvector<unsigned, 1> incoming (n_starting_circles),
    outgoing (n_starting_circles);
  for (unsigned i = 1; i <= n_starting_circles; i ++)
    incoming[i] = outgoing[i] = 0;
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      incoming[crossing_to (i)] ++;
      outgoing[crossing_from (i)] ++;
    }
  
  if (common_from || common_to)
    {
      unsigned common = common_from ? common_from : common_to;
      assert (common);
      
      /* make sure the x_i have single circles */
      for (unsigned i = 1; i <= n_starting_circles; i ++)
	{
	  if (i != common
	      && incoming[i] + outgoing[i] != 1)
	    return;
	}
      
      for (unsigned i = 1; i <= n_crossings; i ++)
	{
	  if (crossing_is_split (i))
	    {
	      unsigned ifrom = crossing_from_cpt (i),
		ito = crossing_to_cpt (i);
	      
	      if (next[ifrom] != ito
		  && prev[ifrom] != ito)
		return;
	    }
	}
      
      bool orient = crossing_orientation (common, 1);
      if (orient)
	return;
      
      for (unsigned i = 1; i <= n_crossings; i ++)
	{
	  bool o = crossing_orientation (common, i);
	  if (o != orient)
	    return;
	}
      
      set<unsigned> b_i;
      for (unsigned i = 1; i <= n_crossings; i ++)
	{
	  if (crossing_is_split (i))
	    {
	      if (orient)
		b_i += crossing_ending_left (i);
	      else
		b_i += crossing_ending_right (i);
	    }
	}
      
      set<unsigned> edges;
      for (unsigned i = 1; i <= n_crossings; i ++)
	{
	  if (crossing_is_split (i))
	    {
	      if (crossing_from_inside (i))
		edges.push (crossing_to_cpt (i));
	      else
		edges.push (crossing_from_cpt (i));
	    }
	  else
	    {
	      assert (crossing_from (i) == common);
	      edges.push (crossing_from_cpt (i));
	    }
	}
      
      /* prod a_i -> x */
      out.append (triple<unsigned, unsigned, set<unsigned> >
		  (unsigned_bitmask (common), unsigned_set (b_i), edges));
    }
}

void
resolution_diagram::twin_arrows_P (basedvector<pair<unsigned, unsigned>, 1> &out) const
{
  if (!marked_edge)
    return;
  unsigned marked_circle = cpt_starting_circle[marked_edge];
  
  basedvector<unsigned, 1> incoming (n_starting_circles),
    outgoing (n_starting_circles);
  for (unsigned i = 1; i <= n_starting_circles; i ++)
    incoming[i] = outgoing[i] = 0;
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      incoming[crossing_to (i)] ++;
      outgoing[crossing_from (i)] ++;
    }
  
  unsigned common_circle = marked_circle; // from circle
  unsigned common_ending_circle = 0;
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      if (crossing_from (i) != common_circle)
	return;
      
      if (crossing_from (i) == crossing_to (i))
	{
	  if (crossing_to_inside (i))
	    {
	      if (prev[crossing_to_cpt (i)] != crossing_from_cpt (i)
		  || marked_edge == crossing_from_cpt (i))
		return;
	    }
	  else
	    {
	      assert (crossing_to_outside (i));
	      if (next[crossing_to_cpt (i)] != crossing_from_cpt (i)
		  || marked_edge == crossing_to_cpt (i))
		return;
	    }
	  
	  common_ending_circle = crossing_ending_left (i);
	}
      else
	{
	  if (incoming[crossing_to (i)] != 1
	      || outgoing[crossing_to (i)] != 0)
	    return;
	  
	  common_ending_circle = crossing_ending_left (i);
	}
    }
  
#if 1
  assert (common_ending_circle);
  out.append (pair<unsigned, unsigned> (unsigned_bitmask (common_circle),
					unsigned_fill (n_ending_circles)
#if 0
					unsigned_bitclear (unsigned_fill (n_ending_circles),
							   common_ending_circle)
#endif
					));
  return;
#endif
}

void
resolution_diagram::show_self () const
{
  assert (resolved.size () == n_crossings);
  assert (to_resolve.size () == n_crossings);
  
  printf ("%d,", n_crossings);
  
  if (resolved.card () != 0
      || to_resolve.card () != n_crossings)
    {
      show (resolved);
      printf (" -> ");
      show (to_resolve);
      printf (",");
    }
  
  basedvector<unsigned, 1> start (n_starting_circles);
  for (unsigned i = 1; i <= n_starting_circles; i ++)
    start[i] = 0;
  for (unsigned i = 1; i <= num_cpts (); i ++)
    {
      unsigned s = cpt_starting_circle[i];
      if (start[s] == 0)
	start[s] = i;
    }
  
  for (unsigned i = 1; i <= n_starting_circles; i ++)
    {
      printf ("(");
      bool first = 1;
      for (unsigned j = start[i];;)
	{
	  if (first)
	    first = 0;
	  else
	    printf (", ");
	  printf ("%c%d%s",
		  is_from_cpt (j) ? 't' : 'h',
		  cpt_crossing (j),
		  cpt_inside % j ? "in" : "out");
	  if (j == marked_edge)
	    printf ("*");
	  if (j == marked_edge2)
	    printf ("+");
	  j = next[j];
	  if (j == start[i])
	    break;
	}
      printf (")");
    }
  
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      printf (",");
      printf ("%d|%d",
	      crossing_ending_to (i),
	      crossing_ending_from (i));
    }
}
