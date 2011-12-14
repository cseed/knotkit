
class smoothing : public refcounted
{
 public:
  unsigned n_circles;
  
  unsigned num_monomials () const { return ((unsigned)1) << n_circles; }
  unsigned num_generators (bool markedp_only)
  {
    return markedp_only
      ? ((unsigned)1) << (n_circles - 1)
      : ((unsigned)1) << n_circles;
  }
  
  //for the i^th edge of the knot diagram, edge_circle[i] is the circle it's part of in the resolution
  basedvector<unsigned, 1> edge_circle;
  
  unsigned crossing_from_circle (const knot_diagram &d, unsigned c) const
  {
    return edge_circle[d.ept_edge (d.crossings[c][2])];
  }
  unsigned crossing_to_circle (const knot_diagram &d, unsigned c) const
  {
    return edge_circle[d.ept_edge (d.crossings[c][4])];
  }
  
  unsigned ept_circle (const knot_diagram &d, unsigned p) const
  {
    return edge_circle[d.ept_edge (p)];
  }
  
  unsigned edge_from (const knot_diagram &d, unsigned e) const
  {
    return d.edge_smoothing_from_ept (e);
  }
  unsigned edge_to (const knot_diagram &d, unsigned e) const
  {
    return d.edge_smoothing_to_ept (e);
  }
  
  bool is_from_ept (const knot_diagram &d, unsigned p) const
  {
    return d.is_smoothing_from_ept (p);
  }
  bool is_to_ept (const knot_diagram &d, unsigned p) const
  {
    return d.is_smoothing_to_ept (p);
  }
  
  bool crossing_from_inside (const knot_diagram &d, smallbitset state, unsigned c) const
  {
    unsigned p = d.crossings[c][2];
    return (state % c) == is_to_ept (d, p);
  }
  bool crossing_from_outside (const knot_diagram &d, smallbitset state, unsigned c) const
  {
    return !crossing_from_inside (d, state, c);
  }
  bool crossing_to_inside (const knot_diagram &d, smallbitset state, unsigned c) const
  {
    unsigned p = d.crossings[c][4];
    unsigned e = d.ept_edge (p);
    return (state % c) == is_to_ept (d, p);
  }
  
  bool is_crossing_from_ept (const knot_diagram &d, smallbitset state, unsigned p) const
  {
    unsigned c = d.ept_crossing (p);
    return p == d.crossings[c][2]
      || (state % c
	  ? p == d.crossings[c][3]
	  : p == d.crossings[c][1]);
  }
  
  unsigned next_edge (const knot_diagram &d, smallbitset state, unsigned e) const
  {
    unsigned p = edge_to (d, e);
    unsigned p2 = d.resolve_next_ept (p, state % d.ept_crossing[p]);
    assert (is_from_ept (d, p2));
    return d.ept_edge (p2);
  }
  unsigned prev_edge (const knot_diagram &d, smallbitset state, unsigned e) const
  {
    unsigned p = edge_from (d, e);
    unsigned p2 = d.resolve_next_ept (p, state % d.ept_crossing[p]);
    assert (is_to_ept (d, p2));
    return d.ept_edge (p2);
  }
  
 public:
  smoothing () : n_circles(0) { }
  smoothing (const knot_diagram &d)
    : edge_circle(d.num_edges ())
  { }
  smoothing (const knot_diagram &d, smallbitset state)
    : edge_circle(d.num_edges ())
  {
    init (d, state);
  }
  smoothing (const smoothing &s)
    : n_circles(s.n_circles),
      edge_circle(s.edge_circle)
  { }
  ~smoothing () { }
  
  void init (const knot_diagram &d, smallbitset state);
  
  smoothing &operator = (const smoothing &s)
  {
    n_circles = s.n_circles;
    edge_circle = s.edge_circle;
    return *this;
  }
  
  unsigned monomial_from (const knot_diagram &d, const smoothing &from_s, unsigned j) const;
  unsigned monomial_from (const knot_diagram &d, const smoothing &from_s, unsigned j,
			  smallbitset skip) const;
  
  void show_self (const knot_diagram &d, smallbitset state) const;
};
