/*
 Initialization: partitions edges into components, computes khovanov homology of the link and stashes it. Computes the poincare polynomial.
 
 Compute the khovanov homology of the link itself, with the algebra action.
 
 Tests for being unlink via algebra action.
 
 Computes the khovanov homology of each component of a link, in hopes of identifying each component of the link from a table.
 */

class knot_detector 
{
 public:  
  knot_diagram &kd;
  unsigned n_components;
  
  map<unsigned, unsigned> edge_component;
  basedvector<unsigned, 1> component_favorite_edge;
  
  typedef Z2 R;
  
  cube<R> c; //unreduced cube
  ptr<const module<R> > Kh; //khovanov homology over F2
  multivariate_laurentpoly<Z> p_poly; //p_poly for the whole link
  
  bool algebra_action_init;
  ptr<const quotient_module<R> > H;  //khovanov homology as a quotient
  basedvector<mod_map<R>,1> x_list; //algebra action on H
  
  bool component_p_poly_init;
  basedvector<multivariate_laurentpoly<Z>,1> component_p_poly;

  basedvector<std::string, 1> name_table;
  basedvector<multivariate_laurentpoly<Z>, 1> p_poly_table;

  unsigned next_e (unsigned e)
  {
    unsigned ept = kd.edge_to_ept(e);
    unsigned c = kd.ept_crossing(ept);
    unsigned next_ept = kd.crossings[c][add_base1_mod4(kd.ept_index[ept],2)];
    assert(kd.is_from_ept(next_ept));
    return kd.ept_edge(next_ept);
  }
  
public:
  knot_detector( knot_diagram &kd_);
  ~knot_detector () { }
  
  void load_table();

  void init_algebra_action();
  
  bool is_unlink();
  
  void compute_component_polys();
  
  bool is_product();
  
  multivariate_laurentpoly<Z> p_poly_from_kd(knot_diagram &kd);
  
  void show_self();
  
  /*p_poly lookup gueses*/
  std::string guess_link();
  std::string guess_components();
  std::string guess_knot(knot_diagram &kd);
  std::string guess_knot(multivariate_laurentpoly<Z> p);
};

