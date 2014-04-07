
// for building knot_diagram
inline unsigned edge_from_ept (unsigned e)
{
  return e * 2 - 1;
}
inline unsigned edge_to_ept (unsigned e)
{
  return e * 2;
}

static inline unsigned
add_base1_mod4 (unsigned x, unsigned y)
{
  assert (between (1, x, 4));
  assert (between (0, y, 4));
  
  return ((x - 1 + y) % 4) + 1;
}

enum mirror { MIRROR };
enum connect_sum { CONNECT_SUM };
enum sublink { SUBLINK };
enum disjoint_union { DISJOINT_UNION };

class knot_diagram
{
 public:
  std::string name;
  
  unsigned n_crossings;
  unsigned marked_edge;
  
  basedvector<basedvector<unsigned, 1>, 1> crossings;
  basedvector<unsigned, 1> ept_crossing;
  basedvector<unsigned, 1> ept_index;
  
  /* true if the smoothing orientation for an edge matches the link
     orientation */
  set<unsigned> edge_smoothing_oriented;
  
  unsigned nminus, nplus;
  
  unsigned num_edges () const { return n_crossings * 2; }
  unsigned num_epts () const { return n_crossings * 4; }
  unsigned ept_edge (unsigned p) const
  {
    assert (between (1, p, num_epts ()));
    return (p + 1) / 2;
  }
  unsigned edge_other_ept (unsigned p) const
  {
    assert (between (1, p, num_epts ()));
    if (is_odd (p))
      return p + 1;
    else
      return p - 1;
  }
  
  unsigned edge_from_ept (unsigned e) const
  {
    assert (between (1, e, num_edges ()));
    return e * 2 - 1;
  }
  unsigned edge_to_ept (unsigned e) const
  {
    assert (between (1, e, num_edges ()));
    return e * 2;
  }
  unsigned is_from_ept (unsigned p) const
  {
    assert (between (1, p, num_epts ()));
    return is_odd (p);
  }
  unsigned is_to_ept (unsigned p) const { return !is_from_ept (p); }
  
  unsigned is_under_ept (unsigned p) const
  {
    assert (between (1, p, num_epts ()));
    return ept_index[p] == 1 || ept_index[p] == 3;
  }
  unsigned is_over_ept (unsigned p) const { return !is_under_ept (p); }
  
  bool is_crossing_from_ept (unsigned p) const
  {
    assert (is_under_ept (p));
    return ept_index[p] == 1;
  }
  bool is_crossing_to_ept (unsigned p) const { return !is_crossing_from_ept (p); }
  
  /* wrt smoothing orientation */
  unsigned edge_smoothing_from_ept (unsigned e) const
  {
    return edge_smoothing_oriented % e
      ? edge_from_ept (e)
      : edge_to_ept (e);
  }
  unsigned edge_smoothing_to_ept (unsigned e) const
  {
    return edge_smoothing_oriented % e
      ? edge_to_ept (e)
      : edge_from_ept (e);
  }
  bool is_smoothing_from_ept (unsigned p) const
  {
    unsigned e = ept_edge (p);
    return p == edge_smoothing_from_ept (e);
  }
  bool is_smoothing_to_ept (unsigned p) const { return !is_smoothing_from_ept (p); }
  
  unsigned num_components () const;
  
  void orient ();
  void calculate_nminus_nplus ();
  void calculate_smoothing_orientation ();
  
  void check_crossings ();
  void rotate_crossing (unsigned c);
  
 public:
  knot_diagram ()
    : n_crossings(0),
      marked_edge(0),
      nminus(0),
      nplus(0)
  { }
  explicit knot_diagram (const planar_diagram &pd);
  explicit knot_diagram (const dt_code &dt);
  knot_diagram (mirror, const knot_diagram &kd);
  knot_diagram (connect_sum,
		const knot_diagram &d1,
		const knot_diagram &d2);
  knot_diagram (sublink,
		smallbitset c,
		const knot_diagram &kd);
  knot_diagram (disjoint_union,
		const knot_diagram &kd1,
		const knot_diagram &kd2);
  
  knot_diagram (const std::string &name_, unsigned n_crossings_, unsigned crossings_ar[][4]);
  knot_diagram (const std::string &name_, const basedvector<basedvector<unsigned, 1>, 1> &crossings_);
  knot_diagram (const knot_diagram &kd)
    : name(kd.name),
      n_crossings(kd.n_crossings),
      marked_edge(kd.marked_edge),
      crossings(kd.crossings),
      ept_crossing(kd.ept_crossing),
      ept_index(kd.ept_index),
      edge_smoothing_oriented(kd.edge_smoothing_oriented),
      nminus(kd.nminus), nplus(kd.nplus)
  { }
  ~knot_diagram () { }
  
  knot_diagram &operator = (const knot_diagram &kd)
  {
    name = kd.name;
    n_crossings = kd.n_crossings;
    marked_edge = kd.marked_edge;
    crossings = kd.crossings;
    ept_crossing = kd.ept_crossing;
    ept_index = kd.ept_index;
    edge_smoothing_oriented = kd.edge_smoothing_oriented;
    nminus = kd.nminus;
    nplus = kd.nplus;
    return *this;
  }
  
  directed_multigraph black_graph (basedvector<unsigned, 1> &bg_edge_height) const;
  
  unsigned resolve_next_ept (unsigned p, bool resolution) const;
  
  int writhe () const { return (int)nplus - (int)nminus; }
  
  unsigned total_linking_number () const;
  
  basedvector<basedvector<int, 1>, 1> planar_diagram_crossings () const;
  
	basedvector<basedvector<int, 1>, 1> as_gauss_code () const;

  hash_t hash_self () const;
  
  void show_ept (unsigned p) const;
  void show_self () const;
  void display_self () const;
};
