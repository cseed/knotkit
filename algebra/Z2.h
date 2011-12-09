class Z2
{
 public:
  typedef linear_combination<Z2> linear_combination;
  // typedef linear_combination_iter<Z2> linear_combination_iter;
  typedef linear_combination_const_iter<Z2> linear_combination_const_iter;
  
 private:
  bool v;
  
 public:
  Z2 () : v(0) { }
  Z2 (int x) : v((bool)(x & 1)) { }
  Z2 (bool v_) : v(v_) { }
  Z2 (const Z2 &x) : v(x.v) { }
  ~Z2 () { }
  
  Z2 &operator = (const Z2 &x) { v = x.v; return *this; }
  Z2 &operator = (int x) { v = (bool)(x & 1); return *this; }
  
  bool operator == (const Z2 &x) const { return v == x.v; }
  
  bool operator == (int x) const { return v == (bool)(x & 1); }
  bool operator != (int x) const { return !operator == (x); }
  
  bool is_unit () const { return v; }
  
  Z2 operator + (const Z2 &x) const { return Z2 (v ^ x.v); }
  Z2 operator - (const Z2 &x) const { return Z2 (v ^ x.v); }
  Z2 operator * (const Z2 &x) const { return Z2 (v & x.v); }
  Z2 operator / (const Z2 &x) const { assert (x.v); return *this; }
  
  Z2 recip () const { assert (v); return *this; }
  
  Z2 &operator += (const Z2 &x) { v ^= x.v; return *this; }
  Z2 &operator -= (const Z2 &x) { v ^= x.v; return *this; }
  Z2 &operator *= (const Z2 &x) { v &= x.v; return *this; }
  Z2 &operator /= (const Z2 &x) { assert (x.v); return *this; }
  
  bool divides (Z2 x) const { return v || !x.v; }
  bool operator | (const Z2 x) const { return divides (x); }
  
  Z2 div (Z2 x) const
  {
    assert (x.divides (*this));
    return *this;
  }
  
  Z2 gcd (Z2 x) const
  {
    assert (v && x.v);
    return Z2 (1);
  }
  
  triple<Z2, Z2, Z2> extended_gcd (Z2 x) const
  {
    if (v)
      return triple<Z2, Z2, Z2> (Z2 (1), Z2 (1), Z2 (0));
    else
      return triple<Z2, Z2, Z2> (Z2 (1), Z2 (0), Z2 (1));
  }
  
  static void show_ring () { printf ("Z2"); }
  void show_self () const { printf ("%d", (int)v); }
  void display_self () const { printf ("%d\n", (int)v); }
};
