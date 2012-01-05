#include <surfacekit.h>

knot_detector::knot_detector ( knot_diagram &kd_)
  : kd(kd_),
    c(kd_,0),
    algebra_action_init(false),
    component_p_poly_init(false)
{
	load_table();
	
  Kh = c.compute_kh();
  p_poly = Kh->free_poincare_polynomial();
  
  unsigned n_edges = kd.num_edges ();
  
  /*partition into components*/
  
  unionfind<1> edges_ufind (n_edges);
  for (unsigned i=1; i <= kd.n_crossings; i++)
  {
    unsigned e1 = kd.ept_edge(kd.crossings[i][1]);
    unsigned e2 = kd.ept_edge(kd.crossings[i][2]);
    unsigned e3 = kd.ept_edge(kd.crossings[i][3]);
    unsigned e4 = kd.ept_edge(kd.crossings[i][4]);

    edges_ufind.join(e1,e3);
    edges_ufind.join(e2,e4);
  }
  
  n_components = edges_ufind.num_sets();
  
  set<unsigned> value_edges;
  map<unsigned, unsigned> fav_edge_to_component;
  unsigned components_so_far=0;
  for (unsigned i=1; i <= kd.num_edges(); i++)
  {
    unsigned c_edge = edges_ufind.find(i);
    if(value_edges % c_edge)
    {
      assert(fav_edge_to_component.find(c_edge).second);
    }
    else
    {
      components_so_far++;
      value_edges.push(c_edge);
      fav_edge_to_component.push(c_edge,components_so_far);
      component_favorite_edge.append(c_edge);
    }
  }
  
  for (unsigned i=1; i <= kd.num_edges(); i++)
  {
    unsigned c = fav_edge_to_component.find(edges_ufind.find(i)).first;
    edge_component.push(i,c);
  }
  
  assert(components_so_far==n_components);
  

}

void knot_detector::init_algebra_action()
{
  mod_map<R> d1 = c.compute_d (1, 0, 0, 0, 0);
 
  ptr<const free_submodule<R> > d1_ker = d1.kernel (),
  d1_im = d1.image (); //image as part of whole cube
  
  ptr<const free_submodule<R> > d1_im2 = d1_ker->restrict_submodule (d1_im); //image as part of kernel
  
  H = d1_ker->quotient (d1_im2); //quotient gives homology
  
  assert(H->dim()==Kh->dim());
  
  for(unsigned i=1; i <= n_components; i++)
  {
    mod_map<R> x = c.compute_alg_action(component_favorite_edge[i]);
    
    assert (x.compose (d1) == d1.compose (x));
    
    mod_map<R> x_ker = x.restrict (d1_ker); //the action of x on the kernel
    mod_map<R> x_H = x_ker.induced_map (H); //the action of x on homology

    x_list.append(x_H);
  }
  
  for(unsigned i=1;i<n_components;i++)
  {
    for(unsigned j=i+1;j<=n_components;j++)
    {
      assert(x_list[i].compose(x_list[j]) == x_list[j].compose(x_list[i]));
    }
  }
             
  algebra_action_init = true;
  
}

bool knot_detector::is_unlink()
{
  //check dimension
  if(Kh->dim() != ((unsigned)1 << n_components))
    return false;
  
  //check poincare_polynomial
  multivariate_laurentpoly<Z> U_p;
	U_p.muladdeq (1, multivariate_laurent_monomial (VARIABLE, 2, 1));
	U_p.muladdeq (1, multivariate_laurent_monomial (VARIABLE, 2, -1));
      
  multivariate_laurentpoly<Z> UL_p=U_p;
  for(unsigned i = 1; i< n_components;i++){
    UL_p = UL_p * U_p;
  }
           
  if(UL_p != p_poly)
    return false;
  
  return true;
  
  //check algebra structure
  if(!algebra_action_init)
    init_algebra_action();
  
  unsigned bottom_gen = 0;
  //find bottom generator
  for(unsigned i = 1; i < H->dim(); i++)
  {
    if(H->generator_grading(i).q== - ((int) n_components))
      bottom_gen=i;
  }
  assert(bottom_gen > 0);
  
  //basedvector<modulinear_combination,1> img_alg_action;
  
}


/*
 make list of PD crossings. for each, if both edges belong to the same component, throw it in a list for that component. if not, discard. this gives a bunch of PD's. use them to create knot_diagrams, feed them to cubes, get their khovonov homologies and their poincare polynomials.
 */
void knot_detector::compute_component_polys()
{
  basedvector<planar_diagram, 1> component_pds;
  for(unsigned i = 1; i <= n_components; i++)
  {
    char name_str[30];
    //sprintf(name_str, "component %d", i);
    planar_diagram blank_pd;
    blank_pd.name = name_str;
    component_pds.append(blank_pd);
  }
  
  
  /*each crossing is either internal to a component, in which case it gets included in that component, or it creates an artificial separation between edges of the same component.*/
  unionfind<1> edge_collapsed_name(kd.num_edges());
    
  for(unsigned i = 1; i <= kd.crossings.size(); i++)
  {
    unsigned comp = (edge_component.find(kd.ept_edge(kd.crossings[i][1]))).first;
    bool homogeneous = true;
    
    assert(kd.is_under_ept(kd.crossings[i][1]));
    
    basedvector<unsigned, 1> e(4);
    e[1] = kd.ept_edge(kd.crossings[i][1]);
    e[2] = kd.ept_edge(kd.crossings[i][2]);
    e[3] = kd.ept_edge(kd.crossings[i][3]);
    e[4] = kd.ept_edge(kd.crossings[i][4]);

    for(unsigned j = 1; j<=4; j++)
    {
      unsigned e_comp = (edge_component.find(e[j])).first;
      //printf("%d:%d\t",e[j],e_comp);
      if(comp!=e_comp)
         homogeneous=false;
    }
    
    //determine whether or not the orientation is correct    
    if(homogeneous)
    {
      basedvector<int, 1> this_crossing;
      this_crossing.append(((int) e[1]));
      this_crossing.append(((int) e[2]));
      this_crossing.append(((int) e[3]));
      this_crossing.append(((int) e[4]));
      
      component_pds[comp].crossings.append(this_crossing);
      //printf("homogeneous crossing hit\n");
    }
    else
    {
      //printf("inhomogeneous crossing hit\n");
      edge_collapsed_name.join(e[1], e[3]);
      edge_collapsed_name.join(e[2], e[4]);
    }
  }
  
  for(unsigned i = 1; i <= n_components; i++)
  {
      for(unsigned j = 1; j <= component_pds[i].crossings.size(); j++)
      {
        for(unsigned k = 1; k <= 4 ; k++)
        {
          component_pds[i].crossings[j][k] = (int) edge_collapsed_name.find((unsigned) component_pds[i].crossings[j][k]);
        }
      }
  }

  
  //components with no self-crossings are unknots. this is a hack to give them a crossing.
  
  basedvector<int, 1> one_crossing_unknot;
  one_crossing_unknot.append(1); one_crossing_unknot.append(1); one_crossing_unknot.append(2); one_crossing_unknot.append(2);
  
  for(unsigned i = 1; i <= n_components; i++)
  {
    if(component_pds[i].crossings.size()==0)
      component_pds[i].crossings.append(one_crossing_unknot);
  }
  
  for(unsigned i = 1; i <= n_components; i++)
  {
    component_pds[i].renumber();
  }
  
  basedvector<knot_diagram, 1> component_kds;
  
  for(unsigned i = 1; i <= n_components; i++)
  {
    knot_diagram blank_kd(component_pds[i]);
    component_kds.append(blank_kd);
  }
  
  for(unsigned i = 1; i <= n_components; i++)
  {
    cube<R> comp_cube(component_kds[i]);
    ptr<const module<R> > comp_kh;
    comp_kh = comp_cube.compute_kh();
    component_p_poly.append(comp_kh->free_poincare_polynomial());
  }
  
  
  component_p_poly_init = true;
}


multivariate_laurentpoly<Z> knot_detector::p_poly_from_kd(knot_diagram &my_kd)
{
    cube<R> my_cube(my_kd);
    ptr<const module<R> > my_kh = my_cube.compute_kh();
    return my_kh->free_poincare_polynomial();
}

std::string knot_detector::guess_knot(multivariate_laurentpoly<Z> p)
{
  /*check for unknot*/
	multivariate_laurentpoly<Z> U_p;
	U_p.muladdeq (1, multivariate_laurent_monomial (VARIABLE, 2, 1));
	U_p.muladdeq (1, multivariate_laurent_monomial (VARIABLE, 2, -1));

  if(p == U_p)
    return "U";
  /*let's start with all prime knots up to six crossings*/
  for(unsigned n = 3; n <= 6; n++)
  {
    for(unsigned k = 1; k<= rolfsen_crossing_knots(n); k++)
    {
      planar_diagram this_pd = rolfsen_knot(n,k);
      knot_diagram this_kd(this_pd);
      multivariate_laurentpoly<Z> candidate_p = p_poly_from_kd(this_kd);
      if(p == candidate_p)
        return this_pd.name;
      knot_diagram mirror_kd(MIRROR, this_kd);
      candidate_p = p_poly_from_kd(mirror_kd);
      if(p == candidate_p)
        return (this_pd.name + "M");
    }
  } 
  
  
  return "unknown";
}

std::string knot_detector::guess_components()
{
  std::string my_guess;
  compute_component_polys();
  
  for(unsigned i = 1; i <= component_p_poly.size(); i++)
  {
    my_guess += guess_knot(component_p_poly[i]);
    if(i < component_p_poly.size())
        my_guess += " and ";
  }
    
  if(is_product())
    my_guess += ", unlinked.\n";
  else
    my_guess += ", linked.\n";
    
  return my_guess;
}

bool knot_detector::is_product()
{
  if(!component_p_poly_init)
    compute_component_polys();
  
  multivariate_laurentpoly<Z> product_poly = component_p_poly[1];
  
  for(unsigned i =2; i<=component_p_poly.size(); i++)
    product_poly = product_poly * component_p_poly[i];
  
  if(product_poly == p_poly)
    return true;
  
  return false;
}

void knot_detector::show_self()
{
    printf("This link has poincare polynomial\n");
    p_poly.display_self();
    printf("It has %d components, "
           , n_components);
           
    if(component_p_poly_init)
    {
      for(unsigned i = 1; i <= component_p_poly.size(); i++)
      {
        printf("%s",guess_knot(component_p_poly[i]).c_str());
        if(i < component_p_poly.size())
          printf(" and ");
      }
    }
    
    if(is_product())
      printf(", which are probably unlinked from eachother.");
    else
      printf(", which are nontrivially linked together.");
    
    if(is_unlink())
      printf("\nIn fact, it's the unlink!");
    
    printf("\n\n");
    
}

void knot_detector::load_table()
{
	
}

void populate_p_poly_table(std::string file_name)
{
	map<multivariate_laurentpoly<Z>, std::string> poly_map;

	for(unsigned n = 3; n <= 10; n++)
	{
		for(unsigned k=1; k<=rolfsen_crossing_knots(n); k++)
		{
			for(unsigned m=0; m<= 1; m++)
				{
				planar_diagram pd = rolfsen_knot(n,k);
				knot_diagram kd(pd);
				if(m)
					kd = knot_diagram(MIRROR,kd);
				cube<Z2> c(kd,0);
				multivariate_laurentpoly<Z> p = c.compute_kh()->free_poincare_polynomial();
				if(poly_map % p)
				{
					printf("Coincidence found: %s and %s\n", poly_map.find(p).first.c_str(), kd.name.c_str());
					//poly_map[p] += ("or " kd.name);
				}
				else
				{
					poly_map.push(p,kd.name);
					printf("pushed %s\n",kd.name.c_str());
				}
			}
		}
	}
	
	writer w(file_name);
	write (w, poly_map);
}
