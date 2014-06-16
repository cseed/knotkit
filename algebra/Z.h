
class Z
{
 public:
  typedef ::linear_combination<Z> linear_combination;
  typedef ::linear_combination_const_iter<Z> linear_combination_const_iter;
  
 private:
  enum steal { STEAL };
  
  class Z_impl : public refcounted
  {
  public:
    mpz_t x;
    
  public:
    Z_impl () { mpz_init (x); }
    Z_impl (int init) { mpz_init_set_si (x, init); }
    Z_impl (copy, mpz_srcptr init) { mpz_init_set (x, init); }
    Z_impl (steal, mpz_srcptr init) { x[0] = *init; }
    Z_impl (reader &r)
    {
      mpz_init (x);
      r.read_mpz (x);
    }
    
    ~Z_impl () { mpz_clear (x); }
    
    void write_self (writer &w) const
    {
      w.write_mpz (x);
    }
  };
  
  ptr<Z_impl> impl;
  
  Z (steal, mpz_srcptr init) : impl(new Z_impl (STEAL, init)) { }
  
 public:
  Z () : impl(new Z_impl) { }
  Z (int init) : impl(new Z_impl (init)) { }
  Z (const Z &z) : impl(z.impl) { }
  Z (copy, const Z &z) : impl(new Z_impl (COPY, z.impl->x)) { }
  Z (reader &r) : impl(new Z_impl (r)) { }
  ~Z () { }
  
  Z &operator = (const Z &z) { impl = z.impl; return *this; }
  Z &operator = (int x) { impl = new Z_impl (x); return *this; }
  
  bool operator == (const Z &z) const { return mpz_cmp (impl->x, z.impl->x) == 0; }
  bool operator != (const Z &z) const { return !operator == (z); }
  
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
  
  Z operator - () const
  {
    mpz_t x;
    mpz_init (x);
    mpz_neg (x, impl->x);
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
  
  Z operator / (const Z &denom) const
  {
    if (mpz_cmp_si (denom.impl->x, 1) == 0)
      return *this;
    else
      {
	assert (mpz_cmp_si (denom.impl->x, -1) == 0);
	
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
  
  // *this += z1*z2
  Z &muladdeq (const Z &z1, const Z &z2)
  {
    mpz_addmul (impl->x, z1.impl->x, z2.impl->x);
    return *this;
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
  
  bool divides (const Z &num) const
  {
    return mpz_divisible_p (num.impl->x, impl->x);
  }
  bool operator | (const Z &num) const { return divides (num); }
  
  Z divide_exact (const Z &denom) const
  {
    // num = *this
    mpz_t q;
    mpz_init (q);
    mpz_divexact (q, impl->x, denom.impl->x);
    return Z (STEAL, q);
  }
  Z div (const Z &denom) const { return divide_exact (denom); }
  
  tuple<Z, Z> divide_with_remainder (const Z &denom) const
  {
    // *this = num
    mpz_t q, r;
    mpz_init (q);
    mpz_init (r);
    mpz_tdiv_qr (q, r, impl->x, denom.impl->x);
    return make_tuple (Z (STEAL, q),
		       Z (STEAL, r));
  }
  
  Z gcd (const Z &z) const
  {
    assert (*this != 0 || z != 0);
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
  
  tuple<Z, Z, Z> extended_gcd (const Z &z) const
  {
    mpz_t d, s, t;
    mpz_init (d);
    mpz_init (s);
    mpz_init (t);
    mpz_gcdext (d, s, t, impl->x, z.impl->x);
    return make_tuple (Z (STEAL, d),
		       Z (STEAL, s),
		       Z (STEAL, t));
  }
  
  static void show_ring () { printf ("Z"); }
  void show_self () const { mpz_out_str (stdout, 10, impl->x); }
  void display_self () const { show_self (); newline (); }
  void write_self (writer &w) const { write (w, *impl); }
};
