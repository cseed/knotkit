
#include <surfacekit.h>

void
test_knot_detector()
{  
  int crossing_ar1[3][4] = { {4,2,1,1},{3,5,4,6},{2,5,3,6} };
  planar_diagram pd1("two_comp_unlink", 3, crossing_ar1);
  knot_diagram kd1(pd1);
  knot_detector detector1(kd1);
  detector1.compute_component_polys();
  detector1.show_self();

  int crossing_ar2[5][4] = { {1,6,5,4},{4,5,3,2},{2,3,7,1},{6,10,9,8},{8,9,10,7}};
  planar_diagram pd2("tref_plus_hopf", 5, crossing_ar2);
  knot_diagram kd2(pd2);
  knot_detector detector2(kd2);
  detector2.compute_component_polys();
  detector2.show_self();

  int crossing_ar3[5][4] = { {1,6,5,4},{4,5,3,2},{2,3,7,1},{10,9,8,6},{8,9,10,7}};
  planar_diagram pd3("tref_plus_u_notwist", 5, crossing_ar3);
  knot_diagram kd3(pd3);
  knot_detector detector3(kd3);
  detector3.compute_component_polys();
  detector3.show_self();

  int crossing_ar4[6][4] = {  {1,6,5,4},{4,5,3,2},{2,3,7,1},{6,10,9,8},{8,9,12,7},{11,11,12,10} };
  planar_diagram pd4("tref_plus_u_twist", 6, crossing_ar4);
  knot_diagram kd4(pd4);
  knot_detector detector4(kd4);
  detector4.compute_component_polys();
  detector4.show_self();
}

void
test_alg_action()
{
  knot_diagram kd (rolfsen_knot (8, 19));
  kd.marked_edge = 1;
  
  typedef Z2 R;
  
  cube<R> c (kd, 0); //the second argument indicates reduced or no
  mod_map<R> d1 = c.compute_d (1, 0, 0, 0, 0);
  mod_map<R> x = c.compute_alg_action(kd.marked_edge);
  
  assert (d1.compose (d1) == 0);
 
  ptr<const free_submodule<R> > d1_ker = d1.kernel (),
  d1_im = d1.image (); //image as part of whole cube
  
  ptr<const free_submodule<R> > d1_im2 = d1_ker->restrict_submodule (d1_im); //image as part of kernel
  
  ptr<const quotient_module<R> > H = d1_ker->quotient (d1_im2); //quotient gives homology
    
  printf ("The rank of the homology group is = %d\n",
          H->dim ());
  
  printf ("The rank of the chain cx is %d\n", c.khC->dim());

  printf ("The rank of the image of x on the chain cx is %d\n", x.image()->dim());

  assert (x.compose (d1) == d1.compose (x));
  
  mod_map<R> x_ker = x.restrict (d1_ker); //the action of x on the kernel
  mod_map<R> x_H = x_ker.induced_map (H); //the action of x on homology
  
  ptr<const module<R> > x_H_im = x_H.image ();

  printf ("The rank of the image of the x action is = %d\n",
          x_H_im->dim ());
}

void
test_projector()
{
  knot_diagram kd (rolfsen_knot (8, 19));
  typedef Z2 R;
  
  cube<R> c (kd, 0); //the second argument indicates reduced or no
  mod_map<R> d1 = c.compute_d (1, 0, 0, 0, 0);
  
  unsigned which_proj_ar[] = {0,0,0,0,0,0,0,0};
  basedvector< unsigned, 1 > which_proj(8);
  for(unsigned i = 1; i<=8;i++)
    which_proj[i]=which_proj_ar[i-1];
  
  mod_map<R> p = c.compute_projector(which_proj);
  ptr<const free_submodule<R> > dom = p.image();
  mod_map<R> big_d = p.compose(d1);
  mod_map<R> little_d = big_d.restrict(dom, dom);
  
  printf("dim of cube: %d\nrank of p: %d\n rank of little_d: %d\nrank of homology: %d\n",c.khC->dim(),big_d.image()->dim(), little_d.image()->dim(), little_d.homology()->dim());
}

void
test_projector_in_series()
{
  //the seifert surface for the hopf link
  int crossings_ar[][4] = {{2,3,4,1},{4,6,5,1},{6,3,2,5}};
  
  //the other seifert surface for the hopf link
  //int crossings_ar[][4] = {{1,2,5,6},{3,4,6,5},{2,1,4,3}};
  
  planar_diagram pd("hopf standard ss", 3, crossings_ar);
  knot_diagram kd(pd);
  
  typedef Z2 R;
  
  cube<R> c (kd, 0); //the second argument indicates reduced or no
  mod_map<R> d = c.compute_d (1, 0, 0, 0, 0);
  
  //highest bit number refers to the first crossing entered.
  unsigned first_proj_ar[] = {0,4,4};
  basedvector< unsigned, 1 > first_proj(3);
  for(unsigned i = 1; i<=3;i++)
    first_proj[i]=first_proj_ar[i-1];
  

  unsigned second_proj_ar[] = {1,4,4};
  basedvector< unsigned, 1 > second_proj(3);
  for(unsigned i = 1; i<=3;i++)
    second_proj[i]=second_proj_ar[i-1];

  kd.display_self();
  
  printf("computing first projection \n");
  mod_map<R> p1 = c.compute_projector(first_proj);
  
  printf("computing second projection \n");
  mod_map<R> p2 = c.compute_projector(second_proj);

  //zero resolution is unknot, and we can compute its homology by restricting d
  ptr<const free_submodule<R> > dom1 = p1.image();
  mod_map<R> big_d1 = p1.compose(d);
  mod_map<R> d1 = big_d1.restrict(dom1, dom1);
  printf("homology of zero resolution has dimension %d and ppoly\n", d1.homology()->dim());
  d1.homology()->free_poincare_polynomial().display_self();
  
  //one resolution is hopf, and we can compute its homology by restricting d
  ptr<const free_submodule<R> > dom2 = p2.image();
  mod_map<R> big_d2 = p2.compose(d);
  mod_map<R> d2 = big_d2.restrict(dom2, dom2);
  printf("homology of one resolution has dimension %d and ppoly\n", d2.homology()->dim());
  d2.homology()->free_poincare_polynomial().display_self();
  
  //go between them
  ptr<const free_submodule<R> > dom = dom1;
  ptr<const free_submodule<R> > range = dom2;
  mod_map<R> big_d = p2.compose(d);
  mod_map<R> f = big_d2.restrict(dom, range);

  chain_map<R> cm(dom,range, d1, d2, f);
  chain_map_helper<R> helper;
  helper.map_info_from_bigq(cm);
  
  /*
  assert (d1.compose (d1) == 0);
  ptr<const free_submodule<R> > d1_ker = d1.kernel (),
  d1_im = d1.image (); //image as part of whole cube
  ptr<const free_submodule<R> > d1_im2 = d1_ker->restrict_submodule (d1_im); //image as part of kernel
  ptr<const quotient_module<R> > H1 = d1_ker->quotient (d1_im2); //quotient gives homology

  assert (d2.compose (d2) == 0);
  ptr<const free_submodule<R> > d2_ker = d2.kernel (),
  d2_im = d2.image (); //image as part of whole cube
  ptr<const free_submodule<R> > d2_im2 = d2_ker->restrict_submodule (d2_im); //image as part of kernel
  ptr<const quotient_module<R> > H2 = d2_ker->quotient (d2_im2); //quotient gives homology

  assert(chain_map.compose(d1) == d2.compose(chain_map));
  mod_map<R> chain_map_ker_to_ker = chain_map.restrict (d1_ker,d2_ker); //the action of x on the kernel
  mod_map<R> h_map = chain_map_ker_to_ker.induced_map(H1, H2); //the action of x on homology
  
  printf("dim of H1: %d\ndim of H2: %d\ndim of map: %d\nppoly of image of map\n"
        ,H1->dim()
        ,H2->dim()
        ,h_map.image()->dim());
  h_map.image()->free_poincare_polynomial().display_self();
  */
}

// test for ring

template<class R> void
test_ring (int p)
{
  R zero (0);
  R one (1);
  R minus_one (-1);
  
  assert (zero == 0);
  assert (zero | zero);
  assert (one | zero);
  assert (minus_one | zero);
  assert (! (zero | one));
  assert (! (zero | minus_one));
  
  assert (one.is_unit ());
  assert (minus_one.is_unit ());
  assert (one.recip () == one);
  assert (minus_one.recip () == minus_one);
  
  if (p)
    assert (R (p) == 0);
  
  if (p != 2)
    assert (one != minus_one);
  
  int n = (p
	   ? std::min (p, 20)
	   : 20);
  for (int i = -n; i <= n; i ++)
    {
      R x (i);
      if (x.is_unit ())
	{
	  assert (x * x.recip () == one);
	  assert (x.recip () * x == one);
	  assert (x.recip ().recip () == x);
	}
      
      assert (one | x);
      assert (minus_one | x);
      
      if (x != 0)
	{
	  assert (x | zero);
	  assert (! (zero | x));
	}
      
      for (int j = -n; j <= n; j ++)
	{
	  R y (j);
	  
	  assert (- (-x) == x);
	  assert (x + y == y + x);
	  assert (x * y == y * x);
	  
	  if (x != 0 && x | y)
	    {
	      R q = y.div (x);
	      assert (y == q * x);
	    }
	  
	  if (x != 0 || y != 0)
	    {
	      triple<R, R, R> t = x.extended_gcd (y);
	      assert (t.first == t.second*x + t.third*y);
	    }
	  
	  for (int k = -n; k <= n; k ++)
	    {
	      R z (k);
	      
	      assert ((x + y) + z == x + (y + z));
	      assert ((x * y) * z == x * (y * z));
	      assert (x*(y + z) == x*y + x*z);
	      assert ((x + y)*z == x*z + y*z);
	    }
	}
    }
}

template<class F> void
test_field ()
{
  for (unsigned i = 1; i <= 8; i ++)
    for (unsigned j = 1; j <= rolfsen_crossing_knots (i); j ++)
      {
	knot_diagram kd (rolfsen_knot (i, j));
	
	show (kd); newline ();
	
	cube<F> c (kd);
	mod_map<F> d = c.compute_d (1, 0, 0, 0, 0);
	
	assert (d.compose (d) == 0);
	
	ptr<const quotient_module<F> > H = d.homology ();
	display ("H:\n", *H);
	
	chain_complex_simplifier<F> s (c.khC, d, 1);
	display ("s.new_C:\n", *s.new_C);
	
	assert (H->dim () == s.new_C->dim ());
      }
}

marked_vertex_diagram pinch_xing(planar_diagram pd, unsigned crossing_to_pinch, unsigned position)
{
	assert(position < 2);
	int edges = 2*pd.crossings.size();
  	basedvector<basedvector<int, 1>, 1> crossings(COPY, pd.crossings);
	basedvector<basedvector<int, 1>, 1> saddles(1);
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
		saddles[1] = new_saddle;
		marked_vertex_diagram mvd(pd.name, crossings, saddles);
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
		saddles[1] = new_saddle;
		marked_vertex_diagram mvd(pd.name, crossings, saddles);
		return mvd;
	}
}


marked_vertex_diagram pinch_xing(marked_vertex_diagram mvd_in, unsigned crossing_to_pinch, unsigned position)
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

marked_vertex_diagram swap_xing(marked_vertex_diagram mvd_in, unsigned crossing_to_swap)
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

int
main ()
{

	planar_diagram the_pd = rolfsen_knot(8,19);
	marked_vertex_diagram mvd = pinch_xing(the_pd, 1, 0);
	//mvd.display_bohua();
	//mvd.print_knot_sequence();
	//mvd_cube cc(mvd);

	
	the_pd = rolfsen_knot(3,1);
	basedvector<basedvector<int, 1> , 1> no_saddles;
	marked_vertex_diagram the_mvd(the_pd.name, the_pd.crossings, no_saddles);
	the_mvd.display_bohua();
	mvd = swap_xing(the_mvd, 1);
	mvd.display_bohua();
	mvd.print_knot_sequence();
	mvd_cube cc(mvd);
	
	marked_vertex_diagram mvd_mirror(MIRROR, mvd);
	mvd_mirror.display_bohua();
	mvd_mirror.print_knot_sequence();
	mvd_cube cc2(mvd_mirror);
	
		// 
		// the_pd = rolfsen_knot(5,2);
		// for(unsigned i = 1; i < the_pd.crossings.size()-2; i++)
		// {
		// 	for(unsigned j = 0; j <= 1; j++)
		// 	{
		// 		mvd = pinch_xing(the_pd,i,j);
		// 		mvd = pinch_xing(mvd, i+1, j);
		// 		mvd = pinch_xing(mvd, i+2, j);
		// 		mvd.display_bohua();
		// 		mvd.print_knot_sequence();
		// 		mvd_cube cc_it(mvd);			
		// 	}
		// }
		//  

}
