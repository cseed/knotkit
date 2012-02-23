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

template<class R, theory th>
class mvd_cube
{
public:
  marked_vertex_diagram mvd;
  knot_diagram kd;
  unsigned n_saddles;
  unsigned n_vertices;
  
  grading initial_adjust;
  grading final_adjust;
  
  cube<R> c;
  
  mod_map<R> d;
  mod_map<R> cob_map;
  
public:
  mvd_cube(marked_vertex_diagram &mvd_);
  ~mvd_cube() {}
    
  void compute_cob_map();
  void factorize_map();

  void display_self () const;
};

template<class R, theory th>
mvd_cube<R, th>::mvd_cube(marked_vertex_diagram &mvd_)
  : mvd(mvd_),
    kd(mvd_.mvd_as_kd()),
    n_saddles(mvd_.saddles.size()),
    n_vertices(mvd_.saddles.size() + mvd_.crossings.size()),
    c(kd,1)
{
  // ??? too late?
  kd.marked_edge = 1;
  
  d = differential<R, th> (c);
  
	knot_diagram kd_initial(mvd.initial_knot());
	knot_diagram kd_final(mvd.final_knot());
	
	/*
	Recall: h is number of 1-res, less nminus
	p is the sum of \pm 1 for the v+ v_
	q is p + h + nplus - nminus
	
	The knots of interest are embedded in the mvd_as_kd as the xxxx00 res and the xxxx11 res.
	*/
	
	initial_adjust.h = -(kd_initial.nminus - kd.nminus);
	initial_adjust.q = initial_adjust.h + (kd_initial.nplus - kd.nplus) - (kd_initial.nminus - kd.nminus);
	
	final_adjust.h = -(kd_final.nminus - kd.nminus) - n_saddles;
	final_adjust.q = final_adjust.h + (kd_final.nplus - kd.nplus) - (kd_final.nminus - kd.nminus);

	compute_cob_map();
	display_self();
  #if 0
   if(n_saddles>1)
    factorize_map();
  #endif
}

template<class R, theory th> void
mvd_cube<R, th>::factorize_map()
{
  /*construct projectors. eg, four saddles. then proj 2 hits xxxx0011, using the lowest bits for our purposes */
  basedvector< mod_map<R>,0 > projectors(n_saddles+1);
  for(unsigned i = 0; i <= n_saddles; i++)
  {
    basedvector<unsigned, 1> proj_vec(n_vertices);
    for(unsigned j = 1; j <= n_vertices; j++)
    {
      if(j <= (n_saddles - i))
        proj_vec[j] = 0;
      if(j<= n_saddles && (j > (n_saddles -i)))
        proj_vec[j] = 1;
      if(j>n_saddles)
        proj_vec[j] = 4;
    }
    projectors[i] = c.compute_projector(proj_vec);
  }
    printf("\n FACTORIZING \n");
  for(unsigned i = 0; i< n_saddles; i++)
  {
    ptr<const free_submodule<R> > dom = projectors[i].image();
    mod_map<R> big_d1 = projectors[i].compose(d);
    mod_map<R> d1 = big_d1.restrict(dom, dom);
  
    ptr<const free_submodule<R> > range = projectors[i+1].image();
    mod_map<R> big_d2 = projectors[i+1].compose(d);
    mod_map<R> d2 = big_d2.restrict(range, range);
  
    mod_map<R> big_f = projectors[i];
    big_f = projectors[i+1].compose(d.compose(big_f));
    
    mod_map<R> f = big_f.restrict(dom, range);

    chain_map<R> cm(dom,range, d1, d2, f);
    chain_map_helper<R> helper;
    helper.map_info_from_bigq(cm);
  }
}

template<class R, theory th> void
mvd_cube<R, th>::compute_cob_map()
{
  /*construct projectors. eg, four saddles. then proj 2 hits xxxx0011, using the lowest bits for our purposes */
  basedvector< mod_map<R>,0 > projectors(n_saddles+1);
  for(unsigned i = 0; i <= n_saddles; i++)
  {
    basedvector<unsigned, 1> proj_vec(n_vertices);
    for(unsigned j = 1; j <= n_vertices; j++)
    {
      if(j <= (n_saddles - i))
        proj_vec[j] = 0;
      if(j<= n_saddles && (j > (n_saddles -i)))
        proj_vec[j] = 1;
      if(j>n_saddles)
        proj_vec[j] = 4;
    }
    projectors[i] = c.compute_projector(proj_vec);
  }

  ptr<const free_submodule<R> > dom = projectors[0].image();
  mod_map<R> big_d1 = projectors[0].compose(d);
  mod_map<R> d1 = big_d1.restrict(dom, dom);
  
  ptr<const free_submodule<R> > range = projectors[n_saddles].image();
  mod_map<R> big_d2 = projectors[n_saddles].compose(d);
  mod_map<R> d2 = big_d2.restrict(range, range);
  
  mod_map<R> big_f = projectors[0];
  for(unsigned i = 1; i<=n_saddles; i++)
    big_f = projectors[i].compose(d.compose(big_f));
    
  mod_map<R> f = big_f.restrict(dom, range);

  chain_map<R> cm(dom,range, d1, d2, f);
  chain_map_helper<R> helper;
  //helper.map_info_from_bigq(cm);
  //helper.map_info_from_simplifier(cm);
	cob_map = helper.induced_map_on_homology_s(cm);
}

template<class R, theory th> void 
mvd_cube<R, th>::display_self() const
{
	printf("Source rank: %d\nTarget rank: %d\n",cob_map.from_dim(),cob_map.to_dim());
	
	printf("The map, in h/q grading:\n");
  for (unsigned i = 1; i <= cob_map.from_dim(); i++)
    {
      printf ("  %d: ", i);
			show (cob_map.domain()->generator_grading(i) + initial_adjust);
      //show (columns[i]);
      if(cob_map[i] != 0)
			{
				printf(" -> ");
				show (cob_map[i].hq() + final_adjust);
			}
			else
				printf("x");
      newline ();
    }

		printf("The map, in tau/e grading:\n");
	  for (unsigned i = 1; i <= cob_map.from_dim(); i ++)
	    {
	      printf ("  %d: ", i);
				grading source_hq = cob_map.domain()->generator_grading(i) + initial_adjust;
				printf("(%d,%d)",source_hq.tau(),source_hq.e());
	      if(cob_map[i] != 0)
				{
					printf(" -> ");
					grading target_hq = cob_map[i].hq() + final_adjust;
					printf("(%d,%d)",target_hq.tau(),target_hq.e());
				}
				else
					printf("x");
	      newline ();
	    }
	
			printf("The map, in delta grading:\n");
		  for (unsigned i = 1; i <= cob_map.from_dim(); i ++)
		    {
		      printf ("  %d: ", i);
					grading source_hq = cob_map.domain()->generator_grading(i) + initial_adjust;
					printf("(%d)",source_hq.delta());
		      if(cob_map[i] != 0)
					{
						printf(" -> ");
						grading target_hq = cob_map[i].hq() + final_adjust;
						printf("(%d)",target_hq.delta());
					}
					else
						printf("x");
		      newline ();
		    }
}

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
