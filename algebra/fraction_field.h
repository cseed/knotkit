
template<class T> class fraction_field
{
 public:
  typedef ::linear_combination<fraction_field> linear_combination;
  typedef ::linear_combination_const_iter<fraction_field> linear_combination_const_iter;
  
 private:
  T num;
  T denom;
  
  enum reduced { REDUCED };
  
  void reduce ();
  void check ();
  
 public:
  fraction_field () : num(0), denom(1) { }
  fraction_field (int x) : num(x), denom(1) { }
  fraction_field (T num_) : num(num_), denom(1) { }
  fraction_field (T num_, T denom_) : num(num_), denom(denom_) { assert (denom != 0); reduce (); }
  fraction_field (reduced, T num_, T denom_)
    : num(num_), denom(denom_)
  {
    assert (denom != 0);
#ifndef NDEBUG
    check ();
#endif
  }
  
  fraction_field (const fraction_field &q) : num(q.num), denom(q.denom) { }
  fraction_field (copy, const fraction_field &q) : num(COPY, q.num), denom(COPY, q.denom) { }
  ~fraction_field () { }
  
  fraction_field &operator = (const fraction_field &q) { num = q.num; denom = q.denom; return *this; }
  fraction_field &operator = (int x) { num = x; denom = 1; return *this; }
  
  bool operator == (const fraction_field &q) const { return num * q.denom == q.num * denom; }
  
  bool operator == (int x) const { return num == denom * T (x); }
  bool operator != (int x) const { return !operator == (x); }
  
  bool is_unit () const { return num != 0; }
  
  fraction_field operator + (const fraction_field &x) const
  {
    T d = denom.gcd (x.denom);
    T e = denom.divide_exact (d),
      xe = x.denom.divide_exact (d);
    
    T new_num = num * xe;
    new_num += x.num * e;
    
    return fraction_field (new_num, denom*xe);
  }
  
  fraction_field operator - (const fraction_field &x) const
  {
    T d = denom.gcd (x.denom);
    T e = denom.divide_exact (d),
      xe = x.denom.divide_exact (d);
    
    T new_num = num * xe;
    new_num -= x.num * e;
    
    return fraction_field (new_num, denom*xe);
  }
  
  fraction_field operator * (const fraction_field &x) const
  {
    T d1 = num.gcd (x.denom);
    T d2 = denom.gcd (x.num);
    return fraction_field (REDUCED,
			   num.divide_exact (d1) * x.num.divide_exact (d2),
			   denom.divide_exact (d2) * x.denom.divide_exact (d1));
  }

  fraction_field operator / (const fraction_field &x) const
  {
    assert (x.num != 0);
    
    T d1 = num.gcd (x.num);
    T d2 = denom.gcd (x.denom);
    return fraction_field (REDUCED,
			   num.divide_exact (d1) * x.denom.divide_exact (d2),
			   denom.divide_exact (d2) * x.num.divide_exact (d1));
  }
  
  fraction_field &invert ()
  {
    assert (num != 0);
    T t = num;
    num = denom;
    denom = t;
    return *this;
  }
  
  fraction_field recip () const
  {
    assert (num != 0);
    return fraction_field (T (COPY, denom),
			   T (COPY, num));
  }
  
  fraction_field &operator += (const fraction_field &x)
  {
    T d = denom.gcd (x.denom);
    T e = denom.divide_exact (d),
      xe = x.denom.divide_exact (d);
    
    num *= xe;
    num += x.num * e;
    denom *= xe;
    
    reduce ();
    
    return *this;
  }
  
  fraction_field &operator -= (const fraction_field &x)
  {
    T d = denom.gcd (x.denom);
    T e = denom.divide_exact (d),
      xe = x.denom.divide_exact (d);
    
    num *= xe;
    num -= x.num * e;
    denom *= xe;
    
    reduce ();
    
    return *this;
  }
  
  fraction_field &operator *= (const fraction_field &x)
  {
    T d1 = num.gcd (x.denom);
    num = num.divide_exact (d1);
    
    T d2 = denom.gcd (x.num);
    denom = denom.divide_exact (d2);
    
    num *= x.num.divide_exact (d2);
    denom *= x.denom.divide_exact (d1);
    
#ifndef NDEBUG
    check ();
#endif
    
    return *this;
  }
  
  fraction_field &operator /= (const fraction_field &x)
  {
    assert (x.num != 0);
    
    T d1 = num.gcd (x.num);
    num = num.divide_exact (d1);
    
    T d2 = denom.gcd (x.denom);
    denom = denom.divide_exact (d2);
    
    num *= x.denom.divide_exact (d2);
    denom *= x.num.divide_exact (d1);
    
#ifndef NDEBUG
    check ();
#endif
    
    return *this;
  }
  
  // d | n, d.divides (n)
  bool divides (const fraction_field &n) const
  {
    // d = *this
    return (num != 0) || (n.num == 0);
  }
  
  bool operator | (const fraction_field &q) const { return divides (q); }
  
  fraction_field div (const fraction_field &d) const { return operator / (d); }
  
  fraction_field gcd (const fraction_field &q) const
  {
    assert (num != 0 && q.num != 0);
    return fraction_field (1);
  }
  
  triple<fraction_field, fraction_field, fraction_field> extended_gcd (const fraction_field &x) const
  {
    if (*this != 0)
      return triple<fraction_field, fraction_field, fraction_field> (*this, 1, 0);
    else
      return triple<fraction_field, fraction_field, fraction_field> (x, 0, 1);
  }
  
  static void show_ring () { printf ("fraction_field("); T::show_ring (); printf (")"); }
  void show_self () const;
  void display_self () const { show_self (); newline (); }
};

template<class T> void
fraction_field<T>::reduce ()
{
  if (num == 0)
    {
      denom = 1;
      return;
    }
  
  T d = num.gcd (denom);
  
  num = num.divide_exact (d);
  denom = denom.divide_exact (d);
}

template<class T> void
fraction_field<T>::check ()
{
  if (num == 0)
    return;
  // check denom == 1
  
  // assert (num.gcd (denom) == 1);
}

template<class T> void
fraction_field<T>::show_self () const
{
  if (denom == 1)
    show (num);
  else
    {
      printf ("(");
      show (num);
      printf (")/(");
      show (denom);
      printf (")");
    }
}
