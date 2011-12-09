
class Q
{
 public:
  typedef linear_combination<Q> linear_combination;
  // typedef linear_combination_iter<Q> linear_combination_iter;
  typedef linear_combination_const_iter<Q> linear_combination_const_iter;
  
 private:
  int n;
  unsigned d;
  
  void reduce ();
  
 public:
  Q () : n(0), d(1) { }
  Q (int x) : n(x), d(1) { }
  Q (int n_, unsigned d_) : n(n_), d(d_) { assert (d != 0); reduce (); }
  Q (const Q &q) : n(q.n), d(q.d) { }
  ~Q () { }
  
  Q &operator = (const Q &q) { n = q.n; d = q.d; return *this; }
  Q &operator = (int x) { n = x; d = 1; return *this; }
  
  bool operator == (const Q &q) const { return n == q.n && d == q.d; }
  
  bool operator == (int x) const { return n == x && d == 1; }
  bool operator != (int x) const { return !operator == (x); }
  
  bool is_unit () const { return n != 0; }
  
  Q operator + (const Q &x) const { return Q (n*x.d + x.n*d, d*x.d); }
  Q operator - (const Q &x) const { return Q (n*x.d - x.n*d, d*x.d); }
  Q operator * (const Q &x) const { return Q (n*x.n, d*x.d); }
  Q operator / (const Q &x) const { return operator * (x.recip ()); }
  
  Q recip () const
  {
    assert (is_unit ());
    if (n < 0)
      return Q (-d, -n);
    else
      return Q (d, n);
  }
  
  Q &operator += (const Q &x) { n = n*x.d + x.n*d; d *= x.d; reduce (); return *this; }
  Q &operator -= (const Q &x) { n = n*x.d - x.n*d; d *= x.d; reduce (); return *this; }
  Q &operator *= (const Q &x) { n *= x.n; d *= x.d; reduce (); return *this; }
  Q &operator /= (const Q &x) { return operator *= (x.recip ()); }
  
  bool divides (const Q &q) const { return (n != 0) || (q.n == 0); }
  bool operator | (const Q &q) const { return divides (q); }
  
  Q gcd (const Q &q) const
  {
    assert (n != 0 && q.n != 0);
    return Q (1);
  }
  
  static void show_ring () { printf ("Q"); }
  void show_self () const;
  void display_self () const { show_self (); newline (); }
};
