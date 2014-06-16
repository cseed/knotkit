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
  
  marked_vertex_diagram (const std::string &name_, unsigned n_crossings, unsigned n_saddles, const int crossings_ar[][4], const int saddles_ar[][4]);
  
  marked_vertex_diagram (const std::string &name_,  const basedvector<basedvector<int, 1>, 1> &crossings_, const basedvector<basedvector<int, 1>, 1> &saddles_)
    : name(name_),
      crossings(crossings_), 
      saddles(saddles_)
  { }
  
  marked_vertex_diagram (const marked_vertex_diagram &mvd)
    : name(mvd.name),
      crossings(mvd.crossings),
      saddles(mvd.saddles)
  { }
  
  marked_vertex_diagram (const planar_diagram &pd)
    : name(pd.name),
      crossings(pd.crossings),
      saddles()
  { }
  
  marked_vertex_diagram (mirror, const marked_vertex_diagram &mvd);
  
  /* FIXME constructor from marked black graphs */
  
  ~marked_vertex_diagram () { }
  
  void display_bohua ();
  void show_self () const { printf ("marked_vertex_diagram %s", name.c_str ()); }
  void display_self () const;
  
  planar_diagram intermediate_knot(unsigned n);
  planar_diagram initial_knot() {return intermediate_knot(0);}
  planar_diagram final_knot() {return intermediate_knot(saddles.size());}
  
  knot_diagram mvd_as_kd();
  
  //euler char, no capping
  int open_euler_char();
  
  unsigned n_edges() { return 2*(crossings.size()+saddles.size()); }
  
  unsigned n_components();
  int framing_shift();
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
  
  void display_self () const;
};

template<class R, theory th>
mvd_cube<R, th>::mvd_cube(marked_vertex_diagram &mvd_)
  : mvd(mvd_),
    kd(mvd_.mvd_as_kd()),
    n_saddles(mvd_.saddles.size()),
    n_vertices(mvd_.saddles.size() + mvd_.crossings.size()),
    c(kd)
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
  
  chain_map<R> cm(d1, d2, f);
  cob_map = cm.induced_map_on_homology();
  // cob_map = cm.induced_map_on_homology_q();
}

template<class R, theory th> void 
mvd_cube<R, th>::display_self() const
{
  printf("Source rank: %d\nTarget rank: %d\n",
	 cob_map.domain()->dim(),
	 cob_map.codomain()->dim());
  
  printf("Kh(K1):\n");
  ptr<const module<R>> H1 = cob_map.domain();
  show(*H1);  newline();
  printf ("  dim = %d\n", H1->dim());
  printf ("  free_rank = %d\n", H1->free_rank ());
  for (unsigned i = 1; i <= H1->dim(); i ++)
    {
      printf ("  %d ", i);
      show (H1->generator_grading (i) + initial_adjust);
      printf (" ");
      H1->show_generator (i);
      
      if (i > H1->free_rank ())
	{
	  printf (": ");
	  R iann = H1->generator_ann (i);
	  if (iann.is_unit ())
	    printf ("0\n");
	  else
	    {
	      R::show_ring ();
	      if (iann != 0)
		{
		  printf ("/(");
		  show (iann);
		  printf (")");
		}
	    }
	}
      newline ();
    }
  
  printf("Kh(K2):\n");
  ptr<const module<R>> H2 = cob_map.codomain();
  show(*H2);  newline();
  printf ("  dim = %d\n", H2->dim());
  printf ("  free_rank = %d\n", H2->free_rank ());
  for (unsigned i = 1; i <= H2->dim(); i ++)
    {
      printf ("  %d ", i);
      show (H2->generator_grading (i) + final_adjust);
      printf (" ");
      H2->show_generator (i);
      
      if (i > H2->free_rank ())
	{
	  printf (": ");
	  R iann = H2->generator_ann (i);
	  if (iann.is_unit ())
	    printf ("0\n");
	  else
	    {
	      R::show_ring ();
	      if (iann != 0)
		{
		  printf ("/(");
		  show (iann);
		  printf (")");
		}
	    }
	}
      newline ();
    }
  
  display("Kh(S) : Kh(K1) -> Kh(K2):\n", cob_map);
  
#if 0
  printf("The map, in h/q grading:\n");
  for (unsigned i = 1; i <= H1->dim(); i++)
    {
      printf ("  %d: ", i);
      show (H1->generator_grading(i) + initial_adjust);
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
  for (unsigned i = 1; i <= H1->dim(); i ++)
    {
      printf ("  %d: ", i);
      grading source_hq = H1->generator_grading(i) + initial_adjust;
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
  for (unsigned i = 1; i <= H1->dim(); i ++)
    {
      printf ("  %d: ", i);
      grading source_hq = H1->generator_grading(i) + initial_adjust;
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
#endif
}
