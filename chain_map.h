
template<class R> 
class chain_map
{
 public:
  const mod_map<R> d1, d2, f;
  
  void test_validity() const
  {
    assert (d1.compose (d1) == 0);
    assert (d2.compose (d2) == 0);
    assert ((f.compose(d1) == d2.compose(f))
	    || (f.compose(d1) + d2.compose(f) == 0));
  }
  
 public:
  chain_map (const mod_map<R> &d1_, const mod_map<R> &d2_, const mod_map<R> &f_)
    : d1(d1_), d2(d2_), f(f_)
  {
    test_validity();
  }
  ~chain_map() {}
  
  mod_map<R> induced_map_on_homology () const;
  mod_map<R> induced_map_on_homology_q () const;
};


template<class R> mod_map<R> 
chain_map<R>::induced_map_on_homology () const
{
  chain_complex_simplifier<R> s1 (d1.domain(), d1, 1, 1);
  assert (s1.new_d == 0);
  
  chain_complex_simplifier<R> s2 (d2.domain(), d2, 1, 1);
  assert (s2.new_d == 0);  
  
  mod_map<R> new_f = s2.pi.compose (f.compose(s1.iota));
  assert (new_f.domain() == s1.new_C);
  assert (new_f.codomain() == s2.new_C);
  
  return new_f;
}

template<class R> mod_map<R> 
chain_map<R>::induced_map_on_homology_q() const
{
  ptr<const free_submodule<R> > d1_ker = d1.kernel (),
    d1_im = d1.image (); //image as part of whole cube
  ptr<const free_submodule<R> > d1_im2 = d1_ker->restrict_submodule (d1_im); //image as part of kernel
  ptr<const quotient_module<R> > H1 = d1_ker->quotient (d1_im2); //quotient gives homology
  
  ptr<const free_submodule<R> > d2_ker = d2.kernel (),
    d2_im = d2.image (); //image as part of whole cube
  ptr<const free_submodule<R> > d2_im2 = d2_ker->restrict_submodule (d2_im); //image as part of kernel
  ptr<const quotient_module<R> > H2 = d2_ker->quotient (d2_im2); //quotient gives homology
  
  mod_map<R> chain_map_ker_to_ker = f.restrict (d1_ker,d2_ker);
  
  mod_map<R> h_map = chain_map_ker_to_ker.induced_map(H1, H2);
  
  return h_map;
}
