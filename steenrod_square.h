
class steenrod_square
{
  // cube of resolutions (in steenrod_square, c will denote a cycle)
  const cube<Z2> &cor;
  
  mod_map<Z2> d;
  const chain_complex_simplifier<Z2> &s;
  
  map<grading, set<unsigned> > KGij;
  
  // sC, fC take generators as input, not hypercube vertices
  Z2 sC (unsigned x, unsigned y) const;
  Z2 fC (unsigned x, unsigned y) const;
  
  pair<set<unsigned>, // Gxy
      map<unsigned, unsigned>  // lxy
    > ladybug_matching (unsigned x, unsigned y) const;
  
  triple<set<unsigned>, // Gcx
      map<unsigned, unsigned>, // bx
      map<unsigned, Z2> // sx
    > boundary_matching (grading cgr,
			 linear_combination<Z2> c,
			 unsigned x) const;
  
  set<pair<unsigned, unsigned> > make_G2cx (grading cgr,
					    linear_combination<Z2> c,
					    unsigned x) const;
  Z2 sq2_coeff (grading cgr,
		linear_combination<Z2> c,
		unsigned x) const;
  
 public:
  steenrod_square (const cube<Z2> &cor_,
		   mod_map<Z2> &d_,
		   const chain_complex_simplifier<Z2> &s_);
  ~steenrod_square () { }
  
  mod_map<Z2> sq1 () const;
  mod_map<Z2> sq2 () const;
};
