
template<unsigned p>
class Zp
{
 public:
  typedef ::linear_combination<Zp<p> > linear_combination;
  typedef ::linear_combination_const_iter<Zp<p> > linear_combination_const_iter;
  
 private:
  unsigned v;
  
 public:
  Zp () : v(0) { }
  Zp (unsigned init) : v(init % p) { }
  Zp (int init)
  {
    int i = init % (int)p;
    if (i < 0)
      i += p;
    assert (i >= 0 && i < (int)p);
    v = i;
  }
  
  Zp (const Zp &x) : v(x.v) { }
  Zp (copy, const Zp &x) : v(x.v) { }
  ~Zp () { }
  
  Zp &operator = (const Zp &x) { v = x.v; return *this; }
  Zp &operator = (int x)
  {
    return operator = (Zp (x));
  }
  
  bool operator == (const Zp &x) const { return v == x.v; }
  bool operator != (const Zp &x) const { return !operator == (x); }
  
  bool operator == (int x) const { return operator == (Zp (x)); }
  bool operator != (int x) const { return !operator == (x); }
  
  bool operator < (const Zp &x) const { return v < x.v; }
  
  bool is_unit () const
  {
    return v != 0;
  }
  
  Zp operator + (const Zp &x) const { return Zp (v + x.v); }
  Zp operator - (const Zp &x) const { return Zp ((int)v - (int)x.v); }
  Zp operator - () const { return Zp (- (int)v);  }
  
  Zp operator * (const Zp &x) const { return Zp (v * x.v); }
  
  Zp operator / (const Zp &x) const
  {
    return operator * (x.recip ());
  }
  
  Zp recip () const
  {
    tuple<unsigned, int, int> t = unsigned_extended_gcd (v, p);
    assert (get<0> (t) == 1);
    assert ((int)get<0> (t) == get<1> (t)*(int)v  + get<2> (t)*(int)p);
    
    return Zp (get<1> (t));
  }
  
  Zp &operator += (const Zp &x)
  {
    v = (v + x.v) % p;
    return *this;
  }
  
  Zp &operator -= (const Zp &x) { return operator = (Zp ((int)v - (int)x.v)); }
  
  Zp &operator *= (const Zp &x)
  {
    v = (v * x.v) % p;
    return *this;
  }
  
  Zp &operator /= (const Zp &x)
  {
    return operator *= (x.recip ());
  }
  
  // d | n, d.divides (n)
  bool divides (const Zp &n) const
  {
    return v || !n.v;
  }
  
  bool operator | (const Zp &n) const { return divides (n); }
  
  Zp div (const Zp &d) const { return operator / (d); }
  
  tuple<Zp, Zp, Zp> extended_gcd (const Zp &x) const
  {
    if (v)
      return make_tuple (v, Zp (1), Zp (0));
    else
      return make_tuple (x, Zp (0), Zp (1));
  }
  
  Zp gcd (const Zp &x) const
  {
    assert (v || x.v);
    return 1;
  }
  
  static void show_ring () { printf ("Z%d", p); }
  void show_self () const { printf ("%d (%d)", v, p); }
  void display_self () const { show_self (); newline (); }
};
