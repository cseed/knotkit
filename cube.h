
class map_rules
{
 public:
  map_rules () { }
  map_rules (const map_rules &); // doesn't exist
  virtual ~map_rules () { }
  
  map_rules &operator = (const map_rules &);  // doesn't exist
  
  virtual void map (basedvector<pair<unsigned, unsigned>, 1> &out,
		    resolution_diagram_builder &rdb) const = 0;
};

template<class R>
class cube /* of resolutions */
{
public:
  typedef typename R::linear_combination linear_combination;
  typedef typename R::linear_combination_const_iter linear_combination_const_iter;
  
 public:
  bool markedp_only;
  
  knot_diagram &kd;
  unsigned n_crossings;
  unsigned n_resolutions;
  
  unsigned n_generators;
  vector<unsigned> resolution_circles;
  vector<unsigned> resolution_generator1;
  
  ptr<const module<R> > khC;
  
  mod_map<R> compute_map (unsigned dh, unsigned max_n,
			  bool mirror,
			  bool reverse_orientation,
			  unsigned to_reverse,
			  const map_rules &rules) const;
  
  mod_map<R> compute_d (unsigned dh, unsigned max_n,
			bool mirror,
			bool reverse_orientation,
			unsigned to_reverse) const;
  
  mod_map<R> compute_twin_arrows_P (bool mirror,
				    bool reverse_orientation,
				    unsigned to_reverse) const;
  
  mod_map<R> H_i (unsigned c);
  
  mod_map<R> compute_nu () const;
  mod_map<R> compute_X (unsigned p) const;
  
  mod_map<R> compute_alg_action(unsigned e);  
  mod_map<R> compute_projector(basedvector<unsigned,1> which_proj);
    
  void check_reverse_crossings ();
  void check_reverse_orientation ();
  
public:
  cube (knot_diagram &d_, bool markedp_only_ = 0);
  ~cube () { }
  
  grading compute_generator_grading (unsigned g) const;
  grading compute_state_monomial_grading (unsigned state, unsigned monomial) const;
  
  unsigned generator (unsigned i, unsigned j) const;
  pair<unsigned, unsigned> generator_state_monomial (unsigned g) const;
  
  ptr<const module<R> > compute_kh () const;
  
  void show_state (unsigned state) const;
  void show_state_monomial (unsigned state, unsigned monomial) const;
  void show_self () const { printf ("cube"); }
  void display_self () const;
};

class twisted_map_rules
{
 public:
  twisted_map_rules () { }
  twisted_map_rules (const twisted_map_rules &); // doesn't exist
  virtual ~twisted_map_rules () { }
  
  map_rules &operator = (const twisted_map_rules &);  // doesn't exist
  
  virtual void map (basedvector<triple<unsigned, unsigned, set<unsigned> >, 1> &out,
		    resolution_diagram_builder &rdb) const = 0;
};

template<class F>
class twisted_cube
{
 public:
  typedef fraction_field<polynomial<F> > R;
  
 public:
  const cube<R> &c;
  
 public:
  twisted_cube (const cube<R> &c_)
    : c(c_)
  { }
  
  twisted_cube (const twisted_cube &); // doesn't exist
  ~twisted_cube () { }
  
  twisted_cube &operator = (const twisted_cube &); // doesn't exist
  
  mod_map<R> compute_twisted_map (basedvector<int, 1> edge_weight,
				  unsigned dh,
				  unsigned to_reverse,
				  const twisted_map_rules &rules) const;
  
  mod_map<R> compute_twisted_barE (basedvector<int, 1> edge_weight,
				   unsigned dh, unsigned to_reverse) const;
  mod_map<R> twisted_d0 (basedvector<int, 1> edge_weight) const;
};

extern mod_map<fraction_field<polynomial<Z2> > >
  twisted_differential (const cube<fraction_field<polynomial<Z2> > > &c);

extern sseq compute_szabo_sseq (const cube<Z2> &c);

#include <cube_impl.h>
