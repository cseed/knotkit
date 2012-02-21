
#include <surfacekit.h>

marked_vertex_diagram::marked_vertex_diagram (const std::string &name_, unsigned n_crossings, unsigned n_saddles, const int crossings_ar[][4], const int saddles_ar[][4])
  : name(name_),
    crossings(n_crossings),
    saddles(n_saddles)
{
    for(unsigned i = 1; i <= n_crossings; i++)
    {
      basedvector<int,1> this_crossing(4);
      for(unsigned j = 1; j <= 4; j++)
      {
        this_crossing[j] = crossings_ar[i-1][j-1];
      }
      crossings[i] = this_crossing;
    }
    
    for(unsigned i = 1; i <= n_saddles; i++)
    {
      basedvector<int,1> this_saddle(4);
      for(unsigned j = 1; j <= 4; j++)
        this_saddle[j] = saddles_ar[i-1][j-1];
      saddles[i] = this_saddle;
    }
}


marked_vertex_diagram::marked_vertex_diagram (mirror, const marked_vertex_diagram &mvd)
  : name("mirror(" + mvd.name + ")"),
    crossings(mvd.crossings.size()),
    saddles(mvd.saddles)
{
    for(unsigned i = 1; i <= crossings.size(); i++)
    {
      basedvector<int,1> this_crossing(4);
      for(unsigned j = 1; j <= 4; j++)
      {
        this_crossing[j] = mvd.crossings[i][add_base1_mod4(j,1)];
      }
      crossings[i] = this_crossing;
    }
}


planar_diagram 
marked_vertex_diagram::intermediate_knot(unsigned n)
{
  //make a pd from the crossings. make a unionfind, and go thru saddles identifying edges. then go thru and renumber the ones in the pd. make a knot_detector, and use its renumber_pd function.
  assert(n<=saddles.size());
  
  basedvector<basedvector<int, 1>, 1> crossings_copy(COPY2, crossings);
  planar_diagram pd(" ", crossings_copy);
  
  //printf("%d time thru\n",n);
  //pd.display_bohua();
  
  //how many edges?
  unsigned n_edges = 2*(crossings.size()+saddles.size());
  unionfind<1> edge_collapsed_name (n_edges);
  //do the zero res
  for(unsigned i = 1; i <= (saddles.size()-n); i++)
  {
      edge_collapsed_name.join(((unsigned) saddles[i][1]),((unsigned) saddles[i][2]));
      edge_collapsed_name.join(((unsigned) saddles[i][3]),((unsigned) saddles[i][4]));
  }
  //do the one res
  for(unsigned i = (saddles.size() - n + 1); i <= saddles.size(); i++)
  {
      edge_collapsed_name.join(((unsigned) saddles[i][1]),((unsigned) saddles[i][4]));
      edge_collapsed_name.join(((unsigned) saddles[i][2]),((unsigned) saddles[i][3]));
  }
  
  for(unsigned i = 1; i <= crossings.size(); i++)
  {
    for(unsigned j = 1; j <= 4; j++)
    {
        pd.crossings[i][j] = (int) edge_collapsed_name.find( (unsigned) crossings[i][j]);
    }
  }

  pd.renumber();

  return pd;
}

void
marked_vertex_diagram::print_knot_sequence()
{
  for(unsigned n = 0; n <= saddles.size(); n++)
  {
    knot_diagram kd(intermediate_knot(n));
    knot_detector detector(kd);
    //printf("Intermediate knot %d with %d components",n, detector.n_components);
    printf("%s\n",detector.guess_components().c_str());
  }

}

void
marked_vertex_diagram::display_bohua ()
{
  printf ("%s\nCrossings[", name.c_str ());
  for (unsigned i = 1; i <= crossings.size (); i ++)
    {
      if (i > 1)
	printf (",");
      printf ("[%d,%d,%d,%d]", 
	      crossings[i][1],
	      crossings[i][2],
	      crossings[i][3],
	      crossings[i][4]);
    }
  printf ("]\nSaddles[");
    for (unsigned i = 1; i <= saddles.size (); i ++)
    {
        if (i > 1)
            printf (",");
        printf ("[%d,%d,%d,%d]", 
                saddles[i][1],
                saddles[i][2],
                saddles[i][3],
                saddles[i][4]);
    }
    printf ("]\n");
  
      //printf("\nribbon euler characteristic: %d\n",  ribbon_euler_char());
			printf("open euler characteristic: %d\n", open_euler_char());
      printf("components: %d\n",  n_components());
      printf("framing shift: %d\n",  framing_shift());
      if(orientable())
        printf("Orientable\n");
      else
      {
        printf("Nonorientable\n");
        printf("________________________________\n");
      }
}

/*our convention: put the saddles first.*/
knot_diagram
marked_vertex_diagram::mvd_as_kd()
{
  basedvector<basedvector<int, 1>, 1> vertices;
  for(unsigned i = 1; i <= saddles.size(); i++)
  {
    vertices.append(saddles[i]);
  }
  for(unsigned i = 1; i <= crossings.size(); i++)
  {
    vertices.append(crossings[i]);
  }
  planar_diagram pd(name, vertices);
  knot_diagram kd(pd);
  return kd;
}

int
marked_vertex_diagram::open_euler_char()
{
  return -saddles.size();
}

int
marked_vertex_diagram::ribbon_euler_char()
{
  //compute number of components of initial knot, ie, how many caps are necessary
  planar_diagram pd = initial_knot();

  unionfind<1> collapsed_edges (2*pd.crossings.size());
  
  for(unsigned i = 1; i <= pd.crossings.size(); i++)
  {
      collapsed_edges.join(((unsigned) pd.crossings[i][1]),((unsigned) pd.crossings[i][3]));
      collapsed_edges.join(((unsigned) pd.crossings[i][2]),((unsigned) pd.crossings[i][4]));
  }

  //caps minues saddles
  //printf("unknots %d saddles %d\n", collapsed_edges.num_sets(), saddles.size());
  return collapsed_edges.num_sets() - saddles.size();
}


bool
marked_vertex_diagram::orientable()
{
  basedvector<basedvector<basedvector<int,1>, 1>,1> edges(n_edges());
  /*each edge will have starting vertex vector and ending vertex vector. each vertex vector has three entries: number of vertex, position of entry, and 1 if crossing 0 if saddle. this is a terrible hack.*/
  for(unsigned i = 1; i <= crossings.size(); i++)
  {
    for(unsigned j = 1; j<= 4; j++)
    {
      basedvector<int, 1> this_vertex(3);
      this_vertex[1] = i; //i^th crossing
      this_vertex[2] = j; //j^th edge at that crossing
      this_vertex[3] = 1; //it's a crossing
      edges[crossings[i][j]].append(this_vertex);
    }
  }
  
  for(unsigned i = 1; i <= saddles.size(); i++)
  {
    for(unsigned j = 1; j<= 4; j++)
    {
      basedvector<int, 1> this_vertex(3);
      this_vertex[1] = i; //i^th saddle
      this_vertex[2] = j; //j^th edge at that saddle
      this_vertex[3] = 0; //it's a saddle
      edges[saddles[i][j]].append(this_vertex);
    }
  }

  for(unsigned i = 1; i <= edges.size(); i++)
    assert(edges[i].size()==2);
    
  basedvector<bool, 1> visited(edges.size());
  for(unsigned i = 1; i <= visited.size(); i++)
    visited[i] = false;
  unsigned edges_visited = 1;
  
  
  //orient the 1-resolution. each loop, orient the edge after current edge to agree with current edge, then move to it
  int current_edge = 1;
  int dummy_counter = 1;
  while(edges_visited < edges.size() && (dummy_counter++ < 30))
  {
    if(visited[current_edge])
    {
      current_edge = 1;
      while(visited[current_edge]){
        current_edge++;
        }
      //printf("jump to %d",current_edge);
    }
    else
    {
      visited[current_edge] = true;
      edges_visited++;
      
      basedvector<int , 1> current_end(COPY, edges[current_edge][2]);
      
      int next_edge;
      if(current_end[3])
        next_edge = crossings[current_end[1]][add_base1_mod4(current_end[2],2)];
      else
        next_edge = saddles[current_end[1]][5-current_end[2]];
      
      //we should be at the tail of the next edge. if not, reverse it
      if((edges[next_edge][1][1] != current_end[1] )||( edges[next_edge][1][3] != current_end[3]))
      {
        assert(edges[next_edge][2][1] == current_end[1]);
        basedvector<int , 1> next_start(COPY, edges[next_edge][2]);
        basedvector<int , 1> next_end(COPY, edges[next_edge][1]);
        edges[next_edge][1] = next_start;
        edges[next_edge][2] = next_end;
      }
      
      //printf("from %d to %d\n",current_edge,next_edge);
      //printf("next_edge points from %d to %d\n",edges[next_edge][1][1],edges[next_edge][2][1]);
      current_edge = next_edge;
    }
  }
  
  //check at each crossing to make sure that things are oriented oppositely
  bool is_oriented = true;
  for(unsigned i = 1; i<=saddles.size(); i++)
  {
    basedvector<int , 1> pointing_out(4);
    for(int j = 1; j<=4; j++)
    {
      if((edges[saddles[i][j]][1][1] == (int) i ) && (edges[saddles[i][j]][1][3] == 0))
        pointing_out[j] = 1;
      else
        pointing_out[j] = 0;
    }
    if(pointing_out[1]+pointing_out[2] != 1)
      is_oriented = false;
    if(pointing_out[2]+pointing_out[3] != 1)
      is_oriented = false;
    if(pointing_out[3]+pointing_out[4] != 1)
      is_oriented = false;
  }
  
  return is_oriented;
}


unsigned
marked_vertex_diagram::n_components()
{
  unionfind<1> collapsed_edges (n_edges());
  
  for(unsigned i = 1; i <= crossings.size(); i++)
  {
      collapsed_edges.join(((unsigned) crossings[i][1]),((unsigned) crossings[i][3]));
      collapsed_edges.join(((unsigned) crossings[i][2]),((unsigned) crossings[i][4]));
  }

  for(unsigned i = 1; i <= saddles.size(); i++)
  {
      collapsed_edges.join(((unsigned) saddles[i][1]),((unsigned) saddles[i][2]));
      collapsed_edges.join(((unsigned) saddles[i][3]),((unsigned) saddles[i][4]));
      collapsed_edges.join(((unsigned) saddles[i][1]),((unsigned) saddles[i][3]));
  }

  return collapsed_edges.num_sets();
}

int
marked_vertex_diagram::framing_shift()
{
  knot_diagram kd_init(initial_knot());
  knot_diagram kd_final(final_knot());
  
  return kd_final.writhe() - kd_init.writhe();
}



mvd_cube::mvd_cube(marked_vertex_diagram &mvd_)
  : mvd(mvd_),
    kd(mvd_.mvd_as_kd()),
    n_saddles(mvd_.saddles.size()),
    n_vertices(mvd_.saddles.size() + mvd_.crossings.size()),
    c(kd,0)
{
   //d = c.compute_d (0, 0, 0, 0, 0);
  d = c.compute_d (1, 0, 0, 0, 0);

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

void
mvd_cube::factorize_map()
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

void
mvd_cube::compute_cob_map()
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

void 
mvd_cube::display_self() const
{
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
}


mvd_comparer::mvd_comparer(marked_vertex_diagram &mvd_, basedvector<basedvector<int, 1> ,1 > which_saddles_)
  : mvd(mvd_),
    which_saddles(which_saddles_),
    kd(mvd_.mvd_as_kd()),
    n_saddles(mvd_.saddles.size()),
    n_vertices(mvd_.saddles.size() + mvd_.crossings.size()),
    c(kd,0)
    
{
   d = c.compute_d (1, 0, 0, 0, 0);
   compute_big_chain_map();
}

void
mvd_comparer::compute_big_chain_map()
{
  /*construct projectors. eg, four saddles. then proj 2 hits xxxx0011, using the lowest bits for our purposes */
  
    printf("made it to big_chain");

  
  unsigned n_surfaces = which_saddles.size();
  basedvector<basedvector< mod_map<R>,0 >, 1> projectors_list(n_surfaces);
  
  for(unsigned k = 1; k <= n_surfaces; k++)
  {
    unsigned n_surface_saddles = which_saddles[k].size();
    for(unsigned i = 0; i <= n_surface_saddles; i++)
    {
      basedvector<unsigned, 1> proj_vec(n_vertices);
      //initialize proj vector to 1 resolution on saddles
      for(unsigned j = 1; j<= n_saddles; j++)
        proj_vec[j] = 1;
      //make sure to let honest crossings vary
      for(unsigned j = n_saddles+1; j<= n_vertices; j++)
        proj_vec[j] = 4;
      //downgrade the first few saddles for the surface to the zero resolution
      for(unsigned j = 1; j <= n_surface_saddles - i; j++)
        proj_vec[which_saddles[k][j]] = 0;
        
      projectors_list[k][i] = c.compute_projector(proj_vec);
    }
  }

  printf("computed projectors");

  mod_map<R> projector_target = projectors_list[1][which_saddles[1].size()];
  ptr<const free_submodule<R> > range = projector_target.image();
  mod_map<R> big_d_target = projector_target.compose(d);
  mod_map<R> d_target = big_d_target.restrict(range, range);

  //hardcoding here that we're comparing two surfaces
  
    printf("computed target");

  //surface 1
  ptr<const free_submodule<R> > dom1 = projectors_list[1][0].image();
  mod_map<R> big_d1 = projectors_list[1][0].compose(d);
  mod_map<R> d1 = big_d1.restrict(dom1, dom1);

  mod_map<R> big_f1 = projectors_list[1][0];
  for(unsigned i = 1; i<=which_saddles[1].size(); i++)
    big_f1 = projectors_list[1][i].compose(d.compose(big_f1));
    
  mod_map<R> f1 = big_f1.restrict(dom1, range);
  
  chain_map<R> cm1(dom1, range, d1, d_target, f1);
  
    printf("computed s1");

  
  //surface 2
  ptr<const free_submodule<R> > dom2 = projectors_list[2][0].image();
  mod_map<R> big_d2 = projectors_list[2][0].compose(d);
  mod_map<R> d2 = big_d2.restrict(dom2, dom2);

  mod_map<R> big_f2 = projectors_list[2][0];
  for(unsigned i = 1; i<=which_saddles[2].size(); i++)
    big_f2 = projectors_list[2][i].compose(d.compose(big_f2));
    
  mod_map<R> f2 = big_f2.restrict(dom2, range);
  
  chain_map<R> cm2(dom2, range, d2, d_target, f2);
  
    printf("computed s2");

     
  chain_map_helper<R> helper;
  helper.compare_images_bigq(cm1, cm2);

}


mvd_playoffs::mvd_playoffs(marked_vertex_diagram &mvd_)
  : mvd(mvd_),
    kd(mvd_.mvd_as_kd()),
    n_vertices(mvd_.saddles.size() + mvd_.crossings.size()),
    c(kd,0)
{
  assert(mvd.saddles.size() == 2);
  d = c.compute_d (1, 0, 0, 0, 0);
  compute_big_chain_map();
}

void
mvd_playoffs::compute_big_chain_map()
{
  /*construct projectors. eg, four saddles. then proj 2 hits xxxx0011, using the lowest bits for our purposes */

  unsigned n_surfaces = 2;
  mod_map<R> proj_1;
  mod_map<R> proj_2;
  mod_map<R> proj_target;
  
  basedvector<unsigned, 1> proj_vec(n_vertices);
  //intialize to let all vary  
  for(unsigned j = 1; j<= n_vertices; j++)
    proj_vec[j] = 4;
  
  proj_vec[1] = 0;
  proj_vec[2] = 1;
  proj_1 = c.compute_projector(proj_vec);
  
  proj_vec[1] = 1;
  proj_vec[2] = 0;
  proj_2 = c.compute_projector(proj_vec);

  proj_vec[1] = 1;
  proj_vec[2] = 1;
  proj_target = c.compute_projector(proj_vec);

  ptr<const free_submodule<R> > range = proj_target.image();
  mod_map<R> big_d_target = proj_target.compose(d);
  mod_map<R> d_target = big_d_target.restrict(range, range);
  

  ptr<const free_submodule<R> > dom1 = proj_1.image();
  mod_map<R> big_d1 = proj_1.compose(d);
  mod_map<R> d1 = big_d1.restrict(dom1, dom1);

  mod_map<R> big_f1 = proj_target.compose(d.compose(proj_1));
  mod_map<R> f1 = big_f1.restrict(dom1, range);
  
  chain_map<R> cm1(dom1, range, d1, d_target, f1);
  

  ptr<const free_submodule<R> > dom2 = proj_2.image();
  mod_map<R> big_d2 = proj_2.compose(d);
  mod_map<R> d2 = big_d2.restrict(dom2, dom2);

  mod_map<R> big_f2 = proj_target.compose(d.compose(proj_2));
  mod_map<R> f2 = big_f2.restrict(dom2, range);
  
  chain_map<R> cm2(dom2, range, d2, d_target, f2);
       
  chain_map_helper<R> helper;
  helper.compare_images_bigq(cm1, cm2);

}

marked_vertex_diagram
mvd_helper::pinch_xing(marked_vertex_diagram mvd_in, unsigned crossing_to_pinch, unsigned position)
{
	assert(position < 2);
	int edges = 2*mvd_in.crossings.size() + 2*mvd_in.saddles.size();
  basedvector<basedvector<int, 1>, 1> crossings(COPY, mvd_in.crossings);
	basedvector<basedvector<int, 1>, 1> saddles(COPY, mvd_in.saddles);

	if(position == 0)
	{
		basedvector<int, 1> new_xing(4);
		basedvector<int, 1> old_xing(crossings[crossing_to_pinch]);
		new_xing[1] = old_xing[1];
		new_xing[2] = old_xing[2];
		new_xing[3] = edges+1;
		new_xing[4] = edges+2;
		crossings[crossing_to_pinch] = new_xing;

		basedvector<int, 1> new_saddle(4);
		new_saddle[1] = edges+2;
		new_saddle[2] = edges+1;
		new_saddle[3] = old_xing[3];
		new_saddle[4] = old_xing[4];	
		saddles.append(new_saddle);
		marked_vertex_diagram mvd(mvd_in.name, crossings, saddles);
		return mvd;
	}
	else
	{
		basedvector<int, 1> new_xing(4);
		basedvector<int, 1> old_xing(crossings[crossing_to_pinch]);
		new_xing[1] = old_xing[1];
		new_xing[2] = edges+1;
		new_xing[3] = edges+2;
		new_xing[4] = old_xing[4];
		crossings[crossing_to_pinch] = new_xing;

		basedvector<int, 1> new_saddle(4);
		new_saddle[1] = edges+2;
		new_saddle[2] = edges+1;
		new_saddle[3] = old_xing[2];
		new_saddle[4] = old_xing[3];
		saddles.append(new_saddle);
		marked_vertex_diagram mvd(mvd_in.name, crossings, saddles);
		return mvd;
	}
}

marked_vertex_diagram
mvd_helper::swap_xing(marked_vertex_diagram mvd_in, unsigned crossing_to_swap)
{
	int edges = 2*mvd_in.crossings.size() + 2*mvd_in.saddles.size();
  basedvector<basedvector<int, 1>, 1> crossings(COPY, mvd_in.crossings);
	basedvector<basedvector<int, 1>, 1> saddles(COPY, mvd_in.saddles);

	basedvector<int, 1> old_xing(crossings[crossing_to_swap]);

	basedvector<int, 1> new_xing_1(4);
	new_xing_1[1] = old_xing[4];
	new_xing_1[2] = edges + 1;
	new_xing_1[3] = edges + 2;
	new_xing_1[4] = edges + 3;
	crossings[crossing_to_swap] = new_xing_1;

	basedvector<int, 1> new_xing_2(4);
	new_xing_2[1] = edges + 5;
	new_xing_2[2] = edges + 6;
	new_xing_2[3] = old_xing[3];
	new_xing_2[4] = edges + 3;
	crossings.append(new_xing_2);

	basedvector<int, 1> new_saddle_1(4);
	new_saddle_1[1] = edges + 1;
	new_saddle_1[2] = old_xing[1];
	new_saddle_1[3] = edges + 4;
	new_saddle_1[4] = edges + 2;
	saddles.append(new_saddle_1);

	basedvector<int, 1> new_saddle_2(4);
	new_saddle_2[1] = edges + 4;
	new_saddle_2[2] = old_xing[2];
	new_saddle_2[3] = edges + 6;
	new_saddle_2[4] = edges + 5;
	saddles.append(new_saddle_2);

	marked_vertex_diagram mvd(mvd_in.name, crossings, saddles);
	return mvd;
}
marked_vertex_diagram 
mvd_helper::j_trick(marked_vertex_diagram mvd_in, unsigned crossing_1, unsigned crossing_2)
{
	
	return mvd_in;

}





