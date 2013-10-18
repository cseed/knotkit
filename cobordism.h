
static const unsigned max_cpts = max_crossings * 2;

enum reverse_crossing_t { REVERSE_CROSSING };
enum reverse_orientation_t { REVERSE_ORIENTATION };

class resolution_diagram
{
 public:
  unsigned n_crossings;
  smallbitset resolved;
  smallbitset to_resolve;
  unsigned n_starting_circles;
  unsigned n_ending_circles;
  
  unsigned marked_edge;  // edge from cpt
  unsigned marked_edge2;
  
  smallbitset cpt_inside;
  /* prev, next cpt on circle in some orientation */
  basedvector<unsigned, 1> prev, next;
  basedvector<unsigned, 1> cpt_starting_circle;
  basedvector<unsigned, 1> cpt_ending_circle;
  
  unsigned num_cpts () const { return n_crossings * 2; }
  unsigned crossing_from_cpt (unsigned c) const
  {
    assert (between (1, c, n_crossings));
    return c * 2 - 1;
  }
  unsigned crossing_to_cpt (unsigned c) const
  {
    assert (between (1, c, n_crossings));
    return c * 2;
  }
  unsigned is_from_cpt (unsigned pt) const
  {
    assert (between (1, pt, num_cpts ()));
    return is_odd (pt);
  }
  unsigned is_to_cpt (unsigned pt) const
  {
    assert (between (1, pt, num_cpts ()));
    return is_even (pt);
  }
  unsigned cpt_crossing (unsigned pt) const
  {
    assert (between (1, pt, num_cpts ()));
    return (pt + 1) / 2;
  }
  unsigned other_cpt (unsigned pt) const
  {
    assert (between (1, pt, num_cpts ()));
    if (is_odd (pt))
      return pt + 1;
    else
      return pt - 1;
  }
  unsigned marked_starting_circle () const
  {
    assert (marked_edge);
    return cpt_starting_circle[marked_edge];
  }
  unsigned marked_ending_circle () const
  {
    assert (marked_edge);
    unsigned c = cpt_crossing (marked_edge);
    
    if (to_resolve(c))
      {
	if (is_to_cpt (marked_edge) != cpt_inside(marked_edge))
	  return cpt_ending_circle[crossing_from_cpt (c)];
	else
	  return cpt_ending_circle[crossing_to_cpt (c)];
      }
    else
      {
	if (is_from_cpt (marked_edge))
	  return cpt_ending_circle[crossing_from_cpt (c)];
	else
	  return cpt_ending_circle[crossing_to_cpt (c)];
      }
  }
  
  unsigned crossing_from (unsigned c) const
  {
    assert (between (1, c, n_crossings));
    return cpt_starting_circle[crossing_from_cpt (c)];
  }
  unsigned crossing_to (unsigned c) const
  {
    assert (between (1, c, n_crossings));
    return cpt_starting_circle[crossing_to_cpt (c)];
  }
  unsigned crossing_ending_from (unsigned c) const
  {
    assert (between (1, c, n_crossings));
    return cpt_ending_circle[crossing_from_cpt (c)];
  }
  unsigned crossing_ending_right (unsigned c) const { return crossing_ending_from (c); }
  unsigned crossing_ending_to (unsigned c) const
  {
    assert (between (1, c, n_crossings));
    return cpt_ending_circle[crossing_to_cpt (c)];
  }
  unsigned crossing_ending_left (unsigned c) const { return crossing_ending_to (c); }
  bool crossing_from_inside (unsigned c) const
  {
    assert (between (1, c, n_crossings));
    return cpt_inside % crossing_from_cpt (c);
  }
  bool crossing_from_outside (unsigned c) const { return !crossing_from_inside (c); }
  bool crossing_to_inside (unsigned c) const
  {
    assert (between (1, c, n_crossings));
    return cpt_inside % crossing_to_cpt (c);
  }
  bool crossing_to_outside (unsigned c) const { return !crossing_to_inside (c); }
  
  unsigned left_cpt (unsigned pt) const
  {
    if (cpt_inside % pt)
      return next[pt];
    else
      return prev[pt];
  }
  unsigned right_cpt (unsigned pt) const
  {
    if (cpt_inside % pt)
      return prev[pt];
    else
      return next[pt];
  }
  
  bool crossing_orientation (unsigned common, unsigned i) const;
  bool crossing_is_split (unsigned c) const { return crossing_from (c) == crossing_to (c); }
  bool crossing_is_join (unsigned c) const { return crossing_from (c) != crossing_to (c); }
  bool marked_edge_left (unsigned p) const;
  
  void compute_prev ();
  void check ();
  
 public:
  resolution_diagram ();
  
  resolution_diagram (unsigned n_crossings_,
		      unsigned n_starting_circles_,
		      unsigned n_ending_circles_);
  resolution_diagram (reverse_crossing_t,
		      const resolution_diagram &rd,
		      smallbitset to_reverse);
  resolution_diagram (reverse_orientation_t,
		      const resolution_diagram &rd);
  
  resolution_diagram (const resolution_diagram &rd);  // copies
  resolution_diagram (copy, const resolution_diagram &rd);
  resolution_diagram (reader &r);
  ~resolution_diagram () { }
  
  resolution_diagram &operator = (const resolution_diagram &rd);
  
  bool operator == (const resolution_diagram &rd) const;
  bool operator != (const resolution_diagram &rd) const { return !operator == (rd); }
  
  /* reverse, reverse_orientation leave starting/ending circle numbers
     unchanged. */
  void reverse ();
  void reverse_orientation ();
  
  knot_diagram as_knot_diagram () const;
  
  void d (basedvector<pair<unsigned, unsigned>, 1> &out) const;
  void twisted_barE (basedvector<triple<unsigned, unsigned, set<unsigned> >, 1> &out) const;
  void twin_arrows_P (basedvector<pair<unsigned, unsigned>, 1> &out) const;
  
  void write_self (writer &w) const;
  hash_t hash_self () const;
  void show_self () const;
  void display_self () const { show_self (); newline (); }
};

class resolution_diagram_builder
{
 public:
  ullmanset<1> gl_crossings;
  ullmanset<1> gl_starting_circles, gl_ending_circles;
  
  basedvector<set<unsigned>, 1> lg_edges;
  
  resolution_diagram rd;
  
 public:
  resolution_diagram_builder ();
  resolution_diagram_builder (const resolution_diagram_builder &) = delete;
  ~resolution_diagram_builder () { }
  
  resolution_diagram_builder &operator = (const resolution_diagram_builder &) = delete;
  
  void init (const knot_diagram &d,
	     smallbitset from_state, const smoothing &from_s,
	     smallbitset to_state, const smoothing &to_s,
	     smallbitset crossings);
  
  void mirror ();
  void reverse_orientation ();
  void reverse_crossing (const knot_diagram &d,
			 const smoothing &from_s, const smoothing &to_s,
			 unsigned c);
};
