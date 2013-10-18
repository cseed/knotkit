
/* univariate polynomial in a single variable `x' with coefficients in
   T. */

template<class T>
class polynomial
{
private:
  map<unsigned, T> coeffs;
  
  polynomial (const map<unsigned, T> &coeffs_) : coeffs(coeffs_) { }
  
public:
  polynomial () { }
  
  polynomial (int x)
  {
    T c (x);
    if (c != 0)
      coeffs.push (0, c);
  }
  
  polynomial (T c)
  {
    if (c != 0)
      coeffs.push (0, c);
  }
  
  polynomial (int x, unsigned e)
  {
    T c (x);
    if (c != 0)
      coeffs.push (e, c);
  }
  
  polynomial (T c, unsigned e)
  {
    if (c != 0)
      coeffs.push (e, c);
  }
  
  polynomial (const polynomial &p) : coeffs(p.coeffs) { }
  polynomial (copy, const polynomial &p) : coeffs(COPY, p.coeffs) { }
  polynomial (reader &r) : coeffs(r) { }
  ~polynomial () { }
  
  polynomial &operator = (const polynomial &p) { coeffs = p.coeffs; return *this; }
  polynomial &operator = (int x)
  {
    coeffs = map<unsigned, T> ();
    T c (x);
    if (c != 0)
      coeffs.push (0, c);
    return *this;
  }
  
  polynomial &operator = (T c)
  {
    coeffs = map<unsigned, T> ();
    if (c != 0)
      coeffs.push (0, c);
    return *this;
  }
  
  unsigned degree () const;
  bool is_unit () const;
  
  polynomial recip () const
  {
    assert (coeffs.card () == 1);
    
    pair<unsigned, T> p = coeffs.head ();
    assert (p.first == 0);
    return polynomial (p.second.recip ());
  }
  
  bool operator == (const polynomial &p) const
  {
#ifndef NDEBUG
    check ();
    p.check ();
#endif
    return coeffs == p.coeffs;
  }
  bool operator != (const polynomial &p) const { return !operator == (p); }
  
  bool operator == (int x) const
  {
    T c (x);
    if (c == 0)
      return coeffs.is_empty ();
    else if (coeffs.card () != 1)
      return 0;
    else
      {
	pair<unsigned, T> p = coeffs.head ();
	return p.first == 0
	  && p.second == c;
      }
  }
  
  bool operator != (int x) const { return !operator == (x); }
  
  polynomial &operator += (polynomial p);
  polynomial &operator -= (polynomial p);
  polynomial &operator *= (polynomial p) { return operator = (*this * p); }
  polynomial &operator *= (T s);
  
  // ??? rename
  polynomial &add_term (T c, unsigned e)
  {
    T &c2 = coeffs[e];
    c2 += c;
    if (c2 == 0)
      coeffs -= e;
    return *this;
  }
  
  polynomial operator - () const { return polynomial () - *this; }
  polynomial operator + (polynomial p) const
  {
    polynomial r (COPY, *this);
    r += p;
    return r;
  }
  
  polynomial operator - (const polynomial &p) const
  {
    polynomial r (COPY, *this);
    r -= p;
    return r;
  }
  
  polynomial operator * (const polynomial &p) const;
  
  bool divides (const polynomial &num) const;
  
  // *this | num
  bool operator | (const polynomial &num) const { return divides (num); }
  
  tuple<polynomial, polynomial> divide_with_remainder (const polynomial &denom) const;
  
  polynomial mod (const polynomial &denom) const;

  polynomial divide_exact (const polynomial &denom) const;
  polynomial div (const polynomial &denom) const { return divide_exact (denom); }
  
  polynomial gcd (const polynomial &b) const;
  polynomial lcm (const polynomial &b) const;
  tuple<polynomial, polynomial, polynomial> extended_gcd (const polynomial &b) const;
  
#ifndef NDEBUG
  void check () const;
#endif
  
  void write_self (writer &w) const { write (w, coeffs); }
  static void show_ring () { T::show_ring (); printf ("[x]"); }
  void display_self () const { show_self (); newline (); }
  void show_self () const;
};

template<class T> unsigned
polynomial<T>::degree () const
{
  return coeffs.tail ().first;
}

template<class T> bool
polynomial<T>::is_unit () const
{
  // ??? is_singleton
  if (coeffs.card () != 1)
    return 0;
  
  pair<unsigned, T> p = coeffs.head ();
  return p.first == 0
    && p.second.is_unit ();
}

template<class T> polynomial<T> &
polynomial<T>::operator += (polynomial p)
{
  for (typename map<unsigned, T>::const_iter i = p.coeffs; i; i ++)
    {
      unsigned e = i.key ();
      T &c = coeffs[e];
      c += i.val ();
      if (c == 0)
	coeffs -= e;
    }
  return *this;
}

template<class T> polynomial<T> &
polynomial<T>::operator -= (polynomial p)
{
  for (typename map<unsigned, T>::const_iter i = p.coeffs; i; i ++)
    {
      unsigned e = i.key ();
      T &c = coeffs[e];
      c -= i.val ();
      if (c == 0)
	coeffs -= e;
    }
  return *this;
}

template<class T> polynomial<T> &
polynomial<T>::operator *= (T s)
{
  if (s == 0)
    coeffs.clear ();
  else
    {
      for (typename map<unsigned, T>::iter i = coeffs; i; i ++)
	i.val () *= s;
    }
}

template<class T> polynomial<T>
polynomial<T>::operator * (const polynomial &p) const
{
  polynomial r;
  
  for (typename map<unsigned, T>::const_iter i = coeffs; i; i ++)
    for (typename map<unsigned, T>::const_iter j = p.coeffs; j; j ++)
      {
	unsigned e = i.key () + j.key ();
	T &c = r.coeffs[e];
	c += i.val () * j.val ();
	if (c == 0)
	  r.coeffs -= e;
      }
  return r;
}

template<class T> tuple<polynomial<T>, polynomial<T> > 
polynomial<T>::divide_with_remainder (const polynomial &d) const
{
  // num = *this
  assert (d != 0);
  
  polynomial r (COPY, *this);
  polynomial q;
  
  pair<unsigned, T> d_leading_term = d.coeffs.tail ();
  for (;;)
    {
      if (r == 0)
	break;
      
      pair<unsigned, T> r_leading_term = r.coeffs.tail ();
      if (r_leading_term.first < d_leading_term.first)
	break;
      
      polynomial m (r_leading_term.second / d_leading_term.second,
		    r_leading_term.first - d_leading_term.first);
      r -= m * d;
      q += m;
    }
  
  assert (r == 0 || r.degree () < d.degree ());
  // assert (*this == q*d + r);
  
  return make_tuple (q, r);
}

template<class T> polynomial<T>
polynomial<T>::mod (const polynomial &denom) const
{
  polynomial q, r;
  tie (q, r) = divide_with_remainder (denom);
  return r;
}

template<class T> bool
polynomial<T>::divides (const polynomial &num) const
{
  // denom = *this
  polynomial q, r;
  tie (q, r) = num.divide_with_remainder (*this);
  return r == 0;
}

template<class T> polynomial<T>
polynomial<T>::divide_exact (const polynomial &denom) const
{
  polynomial q, r;
  tie (q, r) = divide_with_remainder (denom);
  assert (r == 0);
  return q;
}

template<class T> polynomial<T>
polynomial<T>::gcd (const polynomial &b) const
{
  polynomial a = *this;
  while (b != 0)
    {
      polynomial r;
      tie (ignore, r) = a.divide_with_remainder (b);
      a = b;
      b = r;
    }
  return a;
}

template<class T> polynomial<T>
polynomial<T>::lcm (const polynomial &b) const
{
  // a = *this
  return divide_exact (gcd (b)) * b;
}

template<class T> tuple<polynomial<T>, polynomial<T>, polynomial<T> >
polynomial<T>::extended_gcd (const polynomial &b) const
{
  // a = *this
  if (b == 0)
    return make_tuple (*this, polynomial (1), polynomial (0));
  
  polynomial q, r;
  tie (q, r) = divide_with_remainder (b);
  if (r == 0)
    return make_tuple (b, polynomial (0), polynomial (1));
  else
    {
      polynomial d, s, t;
      
      tie (d, s, t) = b.extended_gcd (r);
      
      polynomial s2 = t,
	t2 = s - t*q;
      
      return make_tuple (d, s2, t2);
    }
}

#ifndef NDEBUG
template<class T> void
polynomial<T>::check () const
{
  for (typename map<unsigned, T>::const_iter i = coeffs; i; i ++)
    assert (i.val () != 0);
}
#endif

template<class T> void
polynomial<T>::show_self () const
{
  unsigned first = 1;
  for (typename map<unsigned, T>::const_iter i = coeffs; i; i ++)
    {
      unsigned e = i.key ();
      T c = i.val ();
      
      assert (c != 0);
      
      if (first)
	first = 0;
      else
	printf (" + ");
      
      if (e == 0 && c == 1)
	printf ("1");
      else if (e == 0)
	show (c);
      else if (c == 1)
	{
	  if (e == 1)
	    printf ("x");
	  else
	    printf ("x^%d", e);
	}
      else
	{
	  show (c);
	  if (e == 1)
	    printf ("*x");
	  else
	    printf ("*x^%d", e);
	}
    }
  if (first)
    printf ("0");
}

template<>
class polynomial<Z2>
{
 private:
  set<unsigned> coeffs;
  
  explicit polynomial (const set<unsigned> &coeffs_) : coeffs(coeffs_) { }
  
 public:
  polynomial () { }
  polynomial (int x)
  {
    Z2 c (x);
    if (c != 0)
      coeffs.push (0);
  }
  
  polynomial (Z2 c)
  {
    if (c != 0)
      coeffs.push (0);
  }
  
  polynomial (Z2 c, unsigned e)
  {
    if (c != 0)
      coeffs.push (e);
  }
  
  polynomial (const polynomial &p) : coeffs(p.coeffs) { }
  polynomial (copy, const polynomial &p) : coeffs(COPY, p.coeffs) { }
  ~polynomial () { }
  
  polynomial &operator = (const polynomial &p) { coeffs = p.coeffs; return *this; }
  
  polynomial &operator = (int x)
  {
    coeffs = set<unsigned> ();
    Z2 c (x);
    if (c != 0)
      coeffs.push (0);
    return *this;
  }
  
  polynomial &operator = (Z2 c)
  {
    coeffs = set<unsigned> ();
    if (c == 1)
      coeffs.push (0);
    return *this;
  }
  
  bool operator == (const polynomial &p) const { return coeffs == p.coeffs; }
  bool operator != (const polynomial &p) const { return !operator == (p); }
  
  bool operator == (int x) const
  {
    Z2 c (x);
    if (c == 0)
      return coeffs.is_empty ();
    else
      return coeffs.card () == 1
	&& coeffs.head () == 0;
  }
  
  bool operator != (int x) const { return !operator == (x); }
  
  unsigned degree () const { return coeffs.tail (); }
  bool is_unit () const
  {
    return coeffs.card () == 1
      && coeffs.head () == 0;
  }
  
  polynomial recip () const
  {
    assert (is_unit ());
    return 1;
  }
  
  polynomial &operator += (polynomial p) { coeffs ^= p.coeffs; return *this; }
  polynomial &operator -= (polynomial p) { coeffs ^= p.coeffs; return *this; }
  polynomial &operator *= (polynomial p) { return operator = (*this * p); }
  polynomial &operator *= (Z2 s)
  {
    if (s == 0)
      coeffs.clear ();
    return *this;
  }
  
  polynomial &add_term (Z2 c, unsigned e)
  {
    if (c == 1)
      coeffs.toggle (e);
    return *this;
  }
  
  polynomial operator - () const { return polynomial (COPY, *this); };
  polynomial operator + (polynomial p) const
  {
    polynomial r (COPY, *this);
    r += p;
    return r;
  }
  
  polynomial operator - (polynomial p) const
  {
    polynomial r (COPY, *this);
    r -= p;
    return r;
  }
  
  polynomial operator * (polynomial p) const;
  
  pair<polynomial, polynomial> divide_with_remainder (polynomial d) const;
  
  polynomial mod (polynomial d) const;
  
  bool divides (polynomial d) const;
  polynomial divide_exact (polynomial d) const;
  
  polynomial gcd (polynomial b) const;
  
  static void show_ring () { printf ("Z2[x]"); }
  void display_self () const { show_self (); newline (); }
  void show_self () const;
};
