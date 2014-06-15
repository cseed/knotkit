
#include <surfacekit.h>

typedef fraction_field<polynomial<Z2> > R;

int
main ()
{
  knot_diagram kd (rolfsen_knot (8, 19));
  kd.marked_edge = 1;
  cube<R> c (kd, 1);
  mod_map<R> d = twisted_differential (c);
  assert (d.compose (d) == 0);
  
  ptr<const module<R> > C = c.khC;
  unsigned dh = 0;
  while (d != 0)
    {
      chain_complex_simplifier<R> s (C, d, dh);
      C = s.new_C;
      d = s.new_d;
      dh ++;
    }
  
  printf ("|C| = %d\n", C->dim ());
}

#if 0
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

  chain_map<R> cm(d1, d2, f);
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

void
test_p_poly_table()
{		
	knot_table kt;
	
	planar_diagram pd(rolfsen_knot(4,1));
	knot_diagram kd(pd);
	cube<Z2> c(kd,0);
	multivariate_laurentpoly<Z> p1 = c.compute_kh()->free_poincare_polynomial();
	printf("4_1 is %s\n", kt.lookup(p1).c_str());
	
	planar_diagram pd2(rolfsen_knot(6,2));
	knot_diagram kd2M(pd2);
	knot_diagram kd2(MIRROR, kd2M);
	cube<Z2> c2(kd2,0);
	multivariate_laurentpoly<Z> p2 = c2.compute_kh()->free_poincare_polynomial();
	printf("6_2M is %s\n", kt.lookup(p2).c_str());
	
	multivariate_laurentpoly<Z> p = -11;
  p.muladdeq (5, VARIABLE, 1);
  p.muladdeq (7, VARIABLE, 2);
  p.muladdeq (-3, VARIABLE, 3);

  multivariate_laurentpoly<Z> q = p*p + p + 23;
	printf("fictional ppoly is %s", kt.lookup(q).c_str());
}

int
main ()
{
	//populate_p_poly_table(8);
	//test_p_poly_table();
	
#if 0
	mvd_helper h;
	planar_diagram the_pd = rolfsen_knot(8,19);
	marked_vertex_diagram mvd = h.pinch_xing(the_pd, 1, 0);
	// 	mvd.display_bohua();
	// 	mvd.print_knot_sequence();
	// 	mvd_cube cc1(mvd);
	
	marked_vertex_diagram the_mvd(rolfsen_knot(3,1));
	the_mvd.display_bohua();
	mvd = h.swap_xing(the_mvd, 1);
	mvd.display_bohua();
	mvd.print_knot_sequence();
	mvd_cube cc(mvd);
	
	marked_vertex_diagram mvd_mirror(MIRROR, mvd);
	mvd_mirror.display_bohua();
	mvd_mirror.print_knot_sequence();
	mvd_cube cc2(mvd_mirror);
	
	the_mvd = marked_vertex_diagram(rolfsen_knot(6,1));
	the_mvd = marked_vertex_diagram(MIRROR,the_mvd);
	for(unsigned i = 1; i <= the_mvd.crossings.size(); i++)
	{
			mvd = h.swap_xing(the_mvd,i);
			mvd.display_bohua();
			mvd.print_knot_sequence();
			mvd_cube cc_it(mvd);			
	}


	for(unsigned i = 1; i <= the_mvd.crossings.size(); i++)
	{
			mvd = h.pinch_xing(the_mvd,i,0);
			mvd.display_bohua();
			mvd.print_knot_sequence();
			mvd_cube cc_it(mvd);			
	}
#endif

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


#if 0
  knot_diagram kd (rolfsen_knot (8, 19));
  cube<Z2> c (kd);
  sseq ss = compute_szabo_sseq (c);
  multivariate_laurentpoly<Z> ssp = ss.pages[1].poincare_polynomial (ss.bounds);
  
  display ("ssp: ", ssp);
  
  multivariate_laurentpoly<Z> p;
  p.muladdeq (5, multivariate_laurent_monomial (VARIABLE, 1, -2));
  p.muladdeq (-6, multivariate_laurent_monomial (VARIABLE, 2, 13));
  p.muladdeq (14, (multivariate_laurent_monomial (VARIABLE, 1, 5)
		   * multivariate_laurent_monomial (VARIABLE, 2, -6)));
  display ("p: ", p);
  
  display ("p*p: ", p*p);

  {
    writer w ("dump");
    write (w, p*p);
  }

  {
    reader r ("dump");
    multivariate_laurentpoly<Z> q (r);
    
    display ("q: ", q);
    
    assert (q == p*p);
  }
#endif
  
 
}
#endif
