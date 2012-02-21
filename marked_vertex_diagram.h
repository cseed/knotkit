/* Marked vertex diagram of a knot.  For details, see: Carter & Saito or 
   Yoshikawa.
   
   Our conventions:
    Crossing enumerations go under over under over.
    Saddle enumerations go (12)(34) for the zero res, the from saddle, so (14)(23) is the to.
*/

class marked_vertex_diagram
{
public:
  std::string name;
  basedvector<basedvector<int, 1>, 1> crossings;
  basedvector<basedvector<int, 1>, 1> saddles;

public:
  marked_vertex_diagram () { }
  
  //construct from a list of vertices and a list of their marks  
  marked_vertex_diagram (const std::string &name_, unsigned n_crossings, unsigned n_saddles, const int crossings_ar[][4], const int saddles_ar[][4]);

  //construct from all the right stuff
    marked_vertex_diagram (const std::string &name_,  const basedvector<basedvector<int, 1>, 1> &crossings_, const basedvector<basedvector<int, 1>, 1> &saddles_)
    : name(name_),
      crossings(crossings_), 
      saddles(saddles_)
  { }
  
  //construct from self
  marked_vertex_diagram (const marked_vertex_diagram &mvd)
    : name(mvd.name),
      crossings(mvd.crossings),
      saddles(mvd.saddles)
  { }
  
	//construct trivial mvd from pd
	marked_vertex_diagram (const planar_diagram &pd)
		: name(pd.name),
			crossings(pd.crossings),
			saddles(0)
	{ }

  marked_vertex_diagram (mirror, const marked_vertex_diagram &mvd);

  /*to add: constructor from marked black graphs */
  ~marked_vertex_diagram () { }
  
  void display_bohua ();
  void show_self () const { printf ("marked_vertex_diagram %s", name.c_str ()); }
  void display_self () const;
    
  planar_diagram initial_knot() {return intermediate_knot(0);}
  planar_diagram final_knot() {return intermediate_knot(saddles.size());}
  planar_diagram intermediate_knot(unsigned n);
  
  knot_diagram mvd_as_kd();
  
  void print_knot_sequence();

  //euler char, no capping
  int open_euler_char();
  
  //euler char if ribbon surface
  int ribbon_euler_char();
  
  unsigned n_edges(){ return 2*(crossings.size()+saddles.size());}
  
  unsigned n_components();
  int framing_shift();
  bool orientable();
};

class mvd_cube
{
public:
  marked_vertex_diagram mvd;
  knot_diagram kd;
  unsigned n_saddles;
  unsigned n_vertices;
	grading initial_adjust;
	grading final_adjust;

	//typedef Z R;
  //typedef Zp<3> R;
	typedef Z2 R;

  cube<R> c; //unreduced cube
  mod_map<R> d;
  mod_map<R> cob_map;
  
public:
  mvd_cube(marked_vertex_diagram &mvd_);
  ~mvd_cube() {}
    
  void compute_cob_map();
  void factorize_map();

  void display_self () const;
};

/*to compare two surfaces, make a big diagram containing the saddles required for each. the knot is the full 1 resolution, but now the 0 resolution isn't a shared starting point. instead, the zero res of the saddles for a fixed surface is the origin for that surface. to tell whether or not two surfaces are distinguishable by kh, we compare their images; if the sum of the image subspaces is equal to both of its summands, then they are the same; otherwise, different.*/

class mvd_comparer
{
public:
  marked_vertex_diagram mvd;
  basedvector<basedvector<int, 1>, 1> which_saddles; /*which_saddles[i] has a list of those used to build surface i*/
  knot_diagram kd;
  unsigned n_saddles;
  unsigned n_vertices;
  
  typedef Z2 R;
  
  cube<R> c; //unreduced cube
  mod_map<R> d;
  
public:
  mvd_comparer(marked_vertex_diagram &mvd_, basedvector<basedvector<int, 1> , 1> which_saddles_);
  ~mvd_comparer() {}
  
  void display_self () const;
  
  void compute_big_chain_map();
};

class mvd_playoffs
{
public:
  marked_vertex_diagram mvd;
  knot_diagram kd;
  unsigned n_vertices;
  
	//typedef Zp<3> R;
  typedef Z2 R;
  
  cube<R> c; //unreduced cube
  mod_map<R> d;
  
public:
  mvd_playoffs(marked_vertex_diagram &mvd_);
  ~mvd_playoffs() {}
    
  void compute_big_chain_map();
};


class mvd_helper
{
public:
	mvd_helper (){}
	~mvd_helper (){}
	
	marked_vertex_diagram swap_xing(marked_vertex_diagram mvd_in, unsigned crossing_to_swap);
	marked_vertex_diagram pinch_xing(marked_vertex_diagram mvd_in, unsigned crossing_to_pinch, unsigned position);
	marked_vertex_diagram j_trick(marked_vertex_diagram mvd_in, unsigned crossing_1, unsigned crossing_2);
	
};