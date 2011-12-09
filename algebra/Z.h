
class Z
{
 public:
  typedef linear_combination<Z> linear_combination;
  typedef linear_combination_const_iter<Z> linear_combination_const_iter;
  
  enum steal { STEAL };
  
 private:
  class Z_impl : public refcounted
  {
  public:
    mpz_t x;
    
  public:
    Z_impl () { mpz_init (x); }
    Z_impl (int init) { mpz_init_set_si (x, init); }
    Z_impl (copy, mpz_srcptr init) { mpz_init_set (x, init); }
    Z_impl (steal, mpz_srcptr init) { x[0] = *init; }
    ~Z_impl () { mpz_clear (x); }
  };
  
  ptr<Z_impl> impl;
  
  Z (steal, mpz_srcptr init) : impl(new Z_impl (STEAL, init)) { }
  
 public:
  Z () : impl(new Z_impl) { }
  Z (int init) : impl(new Z_impl (init)) { }
  Z (const Z &z) : impl(z.impl) { }
  Z (copy, const Z &z) : impl(new Z_impl (COPY, z.impl->x)) { }
  ~Z () { }
  
  Z &operator = (const Z &z) { impl = z.impl; return *this; }
  Z &operator = (int x) { impl = new Z_impl (x); return *this; }
  
  bool operator == (const Z &z) const { return mpz_cmp (impl->x, z.impl->x) == 0; }
  
  bool operator == (int y) const { return mpz_cmp_si (impl->x, y) == 0; }
  bool operator != (int y) const { return !operator == (y); }
  
  bool operator < (const Z &z) const { return mpz_cmp (impl->x, z.impl->x) < 0; }
  
  bool is_unit () const
  {
    return *this == 1 || *this == -1;
  }
  
  Z operator + (const Z &z) const
  {
    mpz_t x;
    mpz_init (x);
    mpz_add (x, impl->x, z.impl->x);
    return Z (STEAL, x);
  }
  
  Z operator - (const Z &z) const
  {
    mpz_t x;
    mpz_init (x);
    mpz_sub (x, impl->x, z.impl->x);
    return Z (STEAL, x);
  }
  
  Z operator * (const Z &z) const
  {
    mpz_t x;
    mpz_init (x);
    mpz_mul (x, impl->x, z.impl->x);
    return Z (STEAL, x);
  }
  
  Z operator / (const Z &z) const
  {
    if (mpz_cmp_si (z.impl->x, 1) == 0)
      return *this;
    else
      {
	assert (mpz_cmp_si (z.impl->x, -1) == 0);
	
	mpz_t x;
	mpz_init (x);
	mpz_neg (x, impl->x);
	return Z (STEAL, x);
      }
  }
  
  Z recip () const
  {
    assert (is_unit ());
    return Z (COPY, *this);
  }
  
  Z &operator += (const Z &z)
  {
    mpz_add (impl->x, impl->x, z.impl->x);
    return *this;
  }
  
  Z &operator -= (const Z &z)
  {
    mpz_sub (impl->x, impl->x, z.impl->x);
    return *this;
  }
  
  Z &operator *= (const Z &z)
  {
    mpz_mul (impl->x, impl->x, z.impl->x);
    return *this;
  }
  
  Z &operator /= (const Z &z)
  {
    if (mpz_cmp_si (z.impl->x, 1) == 0)
      ;
    else
      {
	assert (mpz_cmp_si (z.impl->x, -1) == 0);
	mpz_neg (impl->x, impl->x);
      }
    return *this;
  }
  
  bool divides (const Z &n) const
  {
    // d = *this
    return mpz_divisible_p (n.impl->x, impl->x);
  }
  
  bool operator | (const Z &z) const { return divides (z); }
  
  Z div (const Z &d) const
  {
    // n = *this
    mpz_t q;
    mpz_init (q);
    mpz_divexact (q, impl->x, d.impl->x);
    return Z (STEAL, q);
  }
  
  triple<Z, Z, Z> extended_gcd (const Z &z) const
  {
    mpz_t d, s, t;
    mpz_inits (d, s, t, 0);
    mpz_gcdext (d, s, t, impl->x, z.impl->x);
    return triple<Z, Z, Z> (Z (STEAL, d),
			    Z (STEAL, s),
			    Z (STEAL, t));
  }
  
  Z gcd (const Z &z) const
  {
    mpz_t d;
    mpz_gcd (d, impl->x, z.impl->x);
    return Z (STEAL, d);
  }
  
  Z lcm (const Z &z) const
  {
    mpz_t m;
    mpz_lcm (m, impl->x, z.impl->x);
    return Z (STEAL, m);
  }
  
  static void show_ring () { printf ("Z"); }
  void show_self () const { mpz_out_str (stdout, 10, impl->x); }
  void display_self () const { show_self (); newline (); }
};
