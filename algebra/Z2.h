class Z2
{
 public:
  typedef ::linear_combination<Z2> linear_combination;
  // typedef linear_combination_iter<Z2> linear_combination_iter;
  typedef ::linear_combination_const_iter<Z2> linear_combination_const_iter;
  
 private:
  bool v;
  
 public:
  Z2 () : v(0) { }
  Z2 (int x) : v((bool)(x & 1)) { }
  Z2 (unsigned x) : v((bool)(x & 1)) { }
  Z2 (bool v_) : v(v_) { }
  Z2 (const Z2 &x) : v(x.v) { }
  Z2 (copy, const Z2 &x) : v(x.v) { }
  Z2 (reader &r) { v = r.read_bool (); }
  ~Z2 () { }
  
  Z2 &operator = (const Z2 &x) { v = x.v; return *this; }
  Z2 &operator = (int x) { v = (bool)(x & 1); return *this; }
  
  bool operator == (const Z2 &x) const { return v == x.v; }
  bool operator != (const Z2 &x) const { return v != x.v; }
  
  bool operator == (int x) const { return v == (bool)(x & 1); }
  bool operator != (int x) const { return !operator == (x); }
  
  bool is_unit () const { return v; }
  
  Z2 operator + (const Z2 &x) const { return Z2 (v ^ x.v); }
  Z2 operator - (const Z2 &x) const { return Z2 (v ^ x.v); }
  Z2 operator - () const { return *this; }
  Z2 operator * (const Z2 &x) const { return Z2 (v & x.v); }
  Z2 operator / (const Z2 &x) const { assert (x.v); return *this; }
  
  Z2 recip () const { assert (v); return *this; }
  
  Z2 &operator += (const Z2 &x) { v ^= x.v; return *this; }
  Z2 &operator -= (const Z2 &x) { v ^= x.v; return *this; }
  Z2 &operator *= (const Z2 &x) { v &= x.v; return *this; }
  Z2 &operator /= (const Z2 &x) { assert (x.v); return *this; }
  
  // *this += z1*z2
  Z2 &muladdeq (const Z2 &z1, const Z2 &z2)
  {
    return operator += (z1 * z2);
  }
  
  bool divides (const Z2 &num) const { return v || !num.v; }
  bool operator | (const Z2 &num) const { return divides (num); }
  
  Z2 divide_exact (const Z2 &denom) const
  {
    // num = *this
    assert (denom.divides (*this));
    assert (denom == 1);
    return *this;
  }
  Z2 div (const Z2 &denom) const { return divide_exact(denom); }
  
  Z2 gcd (const Z2 &x) const
  {
    assert (v && x.v);
    return Z2 (1);
  }
  
  tuple<Z2, Z2, Z2> extended_gcd (const Z2 &x) const
  {
    assert (v || x.v);
    if (v)
      return make_tuple (Z2 (1), Z2 (1), Z2 (0));
    else
      return make_tuple (Z2 (1), Z2 (0), Z2 (1));
  }
  
  static void show_ring () { printf ("Z2"); }
  void write_self (writer &w) const { w.write_bool (v); }
  void show_self () const { printf ("%d", (int)v); }
  void display_self () const { printf ("%d\n", (int)v); }
};
