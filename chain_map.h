
template<class R> 
class chain_map
{
 public:
  ptr<const module<R> > C1, C2;
  const mod_map<R> &d1, &d2, &f;

  void test_validity(){
      assert (d1.compose (d1) == 0);
      assert (d2.compose (d2) == 0);
      assert((f.compose(d1) == d2.compose(f)) ||( f.compose(d1) +  d2.compose(f) == 0) );
  }

 public:
  chain_map ( ptr<const module<R> > C1_, ptr<const module<R> > C2_, const mod_map<R> &d1_, const mod_map<R> &d2_, const mod_map<R> &f_): C1(C1_), C2(C2_), d1(d1_), d2(d2_), f(f_) {}
  ~chain_map(){}
};


template<class R> 
class chain_map_helper
{
 public:
  chain_map_helper(){}
  ~chain_map_helper(){}
  
  typedef typename R::linear_combination linear_combination;

  
  void map_info_from_bigq (chain_map<R> &cm);
  void map_info_from_simplifier(chain_map<R> &cm);
  mod_map<R> induced_map_on_homology_s (chain_map<R> &cm);
  int compare_images_bigq (chain_map<R> &cm1, chain_map<R> &cm2);

};


template<class R> 
mod_map<R> chain_map_helper<R>::induced_map_on_homology_s(chain_map<R> &cm)
{
  cm.test_validity();
  
#if 0
  // ??? I think this was for gss.
  chain_complex_simplifier<R> s1a (cm.C1, cm.d1, 0, 1);
  chain_complex_simplifier<R> s1b (s1a.new_C, s1a.new_d, 2, 1);
  assert (s1b.new_d == 0);
  
  chain_complex_simplifier<R> s2a (cm.C2, cm.d2, 0, 1);
  chain_complex_simplifier<R> s2b (s2a.new_C, s2a.new_d, 2, 1);
  assert (s2b.new_d == 0);
  
  mod_map<R> new_f = (s2b.pi.compose
		      (s2a.pi.compose
		       (cm.f.compose
			(s1a.iota.compose
			 (s1b.iota)))));
#endif
  
#if 1
  chain_complex_simplifier<R> s1 (cm.C1, cm.d1, 1, 1);
  assert (s1.new_d == 0);
  
  chain_complex_simplifier<R> s2 (cm.C2, cm.d2, 1, 1);
  assert (s2.new_d == 0);  
  
  mod_map<R> new_f = s2.pi.compose (cm.f.compose(s1.iota));
#endif
  
  return new_f;
}


template<class R>
int chain_map_helper<R>::compare_images_bigq (chain_map<R> &cm1, chain_map<R> &cm2)
{
  cm1.test_validity();
  cm2.test_validity();
  assert(cm1.C2 == cm2.C2); //same target
  assert(cm1.d2 == cm2.d2); //with same differential
  
  ptr<const free_submodule<R> > d1_ker = cm1.d1.kernel ();
  ptr<const free_submodule<R> > d2_ker = cm2.d1.kernel ();
  ptr<const free_submodule<R> > d_ker = cm1.d2.kernel ();
  
  mod_map<R> f1 = cm1.f.restrict(d1_ker, d_ker);
  mod_map<R> f2 = cm2.f.restrict(d2_ker, d_ker);
  
  //now take homology of final complex
  ptr<const free_submodule<R> > d_im = d_ker->restrict_submodule(cm1.d2.image()); //image of d inside ker d
  ptr<const quotient_module<R> > H = d_ker->quotient (d_im); //quotient gives homology

  //where do f1 and f2 land in it?
  mod_map<R> f1_bar = f1.induced_map_to(H);
  mod_map<R> f2_bar = f2.induced_map_to(H);
  
  //their images
  ptr<const free_submodule<R> > f1_im = f1_bar.image();
  ptr<const free_submodule<R> > f2_im = f2_bar.image();
  //ptr<const free_submodule<R> > intersection = f1_im->restrict_submodule(f2_im);

  
    basedvector<linear_combination, 1> union_vec;
    for(unsigned i = 1; i<= f1_im->dim(); i++)
      union_vec.append(f1_im->inject_generator(i));
    for(unsigned i = 1; i<= f2_im->dim(); i++)
      union_vec.append(f2_im->inject_generator(i));
    mod_span<R> im_span(H, union_vec);
    
    ptr<const free_submodule<R> > im_submod = H->submodule(im_span);
  

  printf("dim of f_1: %d\ndim of f_2: %d\ndim of union: %d\n"
        ,f1_im->dim()
        ,f2_im->dim()
        ,im_submod->dim());

  if(im_submod->dim() > f1_im->dim())
		printf("KH says these are different!\n");

  return im_submod->dim() - f1_im->dim();
}



template<class R> 
void chain_map_helper<R>::map_info_from_bigq(chain_map<R> &cm)
{
  cm.test_validity();
  
  ptr<const free_submodule<R> > d1_ker = cm.d1.kernel (),
  d1_im = cm.d1.image (); //image as part of whole cube
  ptr<const free_submodule<R> > d1_im2 = d1_ker->restrict_submodule (d1_im); //image as part of kernel
  ptr<const quotient_module<R> > H1 = d1_ker->quotient (d1_im2); //quotient gives homology

  ptr<const free_submodule<R> > d2_ker = cm.d2.kernel (),
  d2_im = cm.d2.image (); //image as part of whole cube
  ptr<const free_submodule<R> > d2_im2 = d2_ker->restrict_submodule (d2_im); //image as part of kernel
  ptr<const quotient_module<R> > H2 = d2_ker->quotient (d2_im2); //quotient gives homology

  mod_map<R> chain_map_ker_to_ker = cm.f.restrict (d1_ker,d2_ker); //the action of x on the kernel
  mod_map<R> h_map = chain_map_ker_to_ker.induced_map(H1, H2); //the action of x on homology
  
  if(H2->free_rank()==H2->dim())
  {
  printf("dim of H1: %d\ndim of H2: %d\ndim of map: %d\nppoly of image of map\n"
        ,H1->dim()
        ,H2->dim()
        ,h_map.image()->dim());
  h_map.image()->free_poincare_polynomial().display_self();
  h_map.display_self();
  }
  else
  {
  printf("dim of H1: %d\nrank of H1: %d\ndim of H2: %d\nrank of H2: %d\n"
        ,H1->dim()
        ,H1->free_rank()
        ,H2->dim()
        ,H2->free_rank()
        );
  if(0)
    printf("the map is sadly zero");
  else
    {
      h_map.display_self();
     /* ptr<const free_submodule<R> > h_im = h_map.image();
      for(unsigned i = 1; i <= H1->dim()); i++)
      {
        h_im->generator_grading(i).display_self();
      }
      */
    }
  }

}


template<class R> 
void chain_map_helper<R>::map_info_from_simplifier(chain_map<R> &cm)
{
  cm.test_validity();

 unsigned dh = 1;
 ptr<const module<R> > C1 = cm.C1,
   C2 = cm.C2;
 mod_map<R> d1 = cm.d1,
   d2 = cm.d2,
   f = cm.f;
 for (;;)
   {
     chain_complex_simplifier<R> s1 (C1, d1, dh, 1),
       s2 (C2, d2, dh, 1);

     C1 = s1.new_C;
     C2 = s2.new_C;
     d1 = s1.new_d;
     d2 = s2.new_d;

     f = s2.pi.compose (f).compose (s1.iota);

     dh ++;

     printf ("rank E^%d_1 = %d\n", dh, C1->dim ());
     printf ("rank E^%d_2 = %d\n", dh, C2->dim ());
     printf ("f:\n");  display (f);

     if (d1 == 0 && d2 == 0)
       break;
   }

}

