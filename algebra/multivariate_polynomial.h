
/* multivariate polynomial in a (vector) variable x with coefficients
   in T. */

template<unsigned n>
class multivariate_monomial
{
 private:
  unsigned v[n];
  
 public:
  multivariate_monomial ()
  {
    for (unsigned i = 0; i < n; i ++)
      v[i] = 0;
  }
  
  multivariate_monomial (variable, unsigned j)
  {
    for (unsigned i = 0; i < n; i ++)
      v[i] = 0;
    
    assert (j >= 1 && j <= n);
    v[j - 1] = 1;
  }
  
  multivariate_monomial (const multivariate_monomial &e)
  {
    for (unsigned i = 0; i < n; i ++)
      v[i] = e.v[i];
  }
  
  ~multivariate_monomial () { }
  
  multivariate_monomial &operator = (const multivariate_monomial &e)
  {
    for (unsigned i = 0; i < n; i ++)
      v[i] = e.v[i];
    return *this;
  }
  
  unsigned degree () const
  {
    unsigned d = 0;
    for (unsigned i = 0; i < n; i ++)
      d += v[i];
    return d;
  }
  
  bool operator == (const multivariate_monomial &e) const
  {
    for (unsigned i = 0; i < n; i ++)
      {
	if (v[i] != e.v[i])
	  return 0;
      }
    return 1;
  }
  bool operator != (const multivariate_monomial &e) const { return !operator == (e); }
  
  bool operator < (const multivariate_monomial &e) const
  {
    for (unsigned i = 0; i < n; i ++)
      {
	if (v[i] < e.v[i])
	  return 1;
	if (v[i] > e.v[i])
	  return 0;
      }
    return 0;
  }
    
  multivariate_monomial operator + (const multivariate_monomial &e) const
  {
    multivariate_monomial m;
    
    for (unsigned i = 0; i < n; i ++)
      m.v[i] = v[i] + e.v[i];
    
    return m;
  }
  
  bool divides (const multivariate_monomial &num) const
  {
    for (unsigned i = 0; i < n; i ++)
      {
	if (v[i] > num.v[i])
	  return 0;
      }
    return 1;
  }
  
  bool operator | (const multivariate_monomial &num) const { return divides  (num); }
  
  multivariate_monomial &mineq (const multivariate_monomial &e)
  {
    for (unsigned i = 0; i < n; i ++)
      {
	if (e.v[i] < v[i])
	  v[i] = e.v[i];
      }
    
    return *this;
  }
  
  multivariate_monomial operator - (const multivariate_monomial &denom) const
  {
    multivariate_monomial m;
    
    for (unsigned i = 0; i < n; i ++)
      {
	assert (v[i] >= denom.v[i]);
	m.v[i] = v[i] - denom.v[i];
      }
    
    return m;
  }
  
  void show_self () const
  {
    printf ("x^(");
    for (unsigned i = 0; i < n; i ++)
      {
	if (i > 0)
	  printf (",");
	printf ("%d", v[i]);
      }
    printf (")");
  }
};

template<class T, unsigned n>
class multivariate_polynomial
{
 public:
  typedef ::linear_combination<multivariate_polynomial<T, n> > linear_combination;
  typedef ::linear_combination_const_iter<multivariate_polynomial<T, n> >
    linear_combination_const_iter;
  
private:
  typedef multivariate_monomial<n> monomial;
  
  map<monomial, T> coeffs;
  
  explicit multivariate_polynomial (const map<monomial, T> &coeffs_) : coeffs(coeffs_) { }
  
public:
  multivariate_polynomial () { }
  multivariate_polynomial (int x)
  {
    T c (x);
    if (c != 0)
      coeffs.push (monomial (), c);
  }
  
  multivariate_polynomial (T c)
  {
    if (c != 0)
      coeffs.push (monomial (), c);
  }
  
  multivariate_polynomial (T c, variable, unsigned i)
  {
    if (c != 0)
      coeffs.push (monomial (VARIABLE, i), c);
  }
  
  multivariate_polynomial (T c, const monomial &m)
  {
    if (c != 0)
      coeffs.push (m, c);
  }
  
  multivariate_polynomial (const multivariate_polynomial &p) : coeffs(p.coeffs) { }
  multivariate_polynomial (copy, const multivariate_polynomial &p) : coeffs(COPY, p.coeffs) { }
  ~multivariate_polynomial () { }
  
  multivariate_polynomial &operator = (const multivariate_polynomial &p)
  {
    coeffs = p.coeffs;
    return *this;
  }
  
  multivariate_polynomial &operator = (int x)
  {
    coeffs = map<monomial, T> ();
    T c (x);
    if (c != 0)
      coeffs.push (monomial (), c);
    return *this;
  }
  
  multivariate_polynomial &operator = (T c)
  {
    coeffs = map<monomial, T> ();
    if (c != 0)
      coeffs.push (monomial (), c);
    return *this;
  }
  
  bool is_unit () const;
  
  bool operator == (const multivariate_polynomial &p) const
  {
#ifndef NDEBUG
    check ();
    p.check ();
#endif
    return coeffs == p.coeffs;
  }

  bool operator != (const multivariate_polynomial &p) const { return !operator == (p); }
  
  bool operator == (int x) const
  {
#ifndef NDEBUG
    check ();
#endif
    T c (x);
    if (c == 0)
      return coeffs.is_empty ();
    else
      {
	if (coeffs.card () != 1)
	  return 0;
	
	pair<monomial, T> p = coeffs.head ();
	return p.first.degree () == 0
	  && p.second == c;
      }
  }
  
  bool operator != (int x) const { return !operator == (x); }
  
  multivariate_polynomial &operator += (multivariate_polynomial p);
  multivariate_polynomial &operator -= (multivariate_polynomial p);
  multivariate_polynomial &operator *= (multivariate_polynomial p) { return operator = (*this * p); }
  multivariate_polynomial &operator *= (T s);
  
  multivariate_polynomial &add_term (T c, variable, unsigned i)
  {
    monomial m (VARIABLE, i);
    T &c2 = coeffs[m];
    c2 += c;
    if (c2 == 0)
      coeffs -= m;
    return *this;
  }
  
  multivariate_polynomial operator - () const { return multivariate_polynomial () - *this; }
  multivariate_polynomial operator + (multivariate_polynomial p) const
  {
    multivariate_polynomial r (COPY, *this);
    r += p;
    return r;
  }
  
  multivariate_polynomial operator - (multivariate_polynomial p) const
  {
    multivariate_polynomial r (COPY, *this);
    r -= p;
    return r;
  }
  
  multivariate_polynomial operator * (const multivariate_polynomial &p) const;
  
  monomial common_monomial () const;
  
  pair<multivariate_polynomial, multivariate_polynomial>
    uncommon_factors (multivariate_polynomial b, basedvector<multivariate_polynomial, 1> ds);
  maybe<multivariate_polynomial>
    divides_exactly (const multivariate_polynomial &num) const;
  multivariate_polynomial divide_exact (const multivariate_polynomial &d) const;
  
  bool operator | (const multivariate_polynomial &num) const { abort (); }
  
#ifndef NDEBUG
  void check () const;
#endif
  
  static void show_ring ()
  {
    T::show_ring ();
    switch (n)
      {
      case 0: break;
      case 1: printf ("[x_1]"); break;
      case 2: printf ("[x_1, x_2]"); break;
      default:
	printf ("[x_1, ..., x_%d]", n);
	break;
      }
  }
  
  void display_self () const { show_self (); newline (); }
  void show_self () const;
};

template<class T, unsigned n> bool
multivariate_polynomial<T, n>::is_unit () const
{
  // ??? is_singleton
  if (coeffs.card () != 1)
    return 0;
  
  pair<monomial, T> p = coeffs.head ();
  return p.first.degree () == 0
    && p.second.is_unit ();
}

template<class T, unsigned n> multivariate_polynomial<T, n> &
  multivariate_polynomial<T, n>::operator += (multivariate_polynomial p)
{
  for (typename map<monomial, T>::const_iter i = p.coeffs; i; i ++)
    {
      monomial m = i.key ();
      T &c = coeffs[m];
      c += i.val ();
      if (c == 0)
	coeffs -= m;
    }
  return *this;
}

template<class T, unsigned n> multivariate_polynomial<T, n> &
multivariate_polynomial<T, n>::operator -= (multivariate_polynomial p)
{
  for (typename map<monomial, T>::const_iter i = p.coeffs; i; i ++)
    {
      monomial m = i.key ();
      T &c = coeffs[m];
      c -= i.val ();
      if (c == 0)
	coeffs -= m;
    }
  return *this;
}

template<class T, unsigned n> multivariate_polynomial<T, n> &
multivariate_polynomial<T, n>::operator *= (T s)
{
  if (s == 0)
    coeffs.clear ();
  else
    {
      for (typename map<monomial, T>::iter i = coeffs; i; i ++)
	i.val () *= s;
    }
}

template<class T, unsigned n> multivariate_polynomial<T, n>
multivariate_polynomial<T, n>::operator * (const multivariate_polynomial &p) const
{
  multivariate_polynomial r;
  
  for (typename map<monomial, T>::const_iter i = coeffs; i; i ++)
    for (typename map<monomial, T>::const_iter j = p.coeffs; j; j ++)
      {
	monomial m = i.key () + j.key ();
	T &c = r.coeffs[m];
	c += i.val () * j.val ();
	if (c == 0)
	  r.coeffs -= m;
      }
  return r;
}

template<class T, unsigned n> multivariate_monomial<n>
multivariate_polynomial<T, n>::common_monomial () const
{
  monomial m = coeffs.head ().first;
  for (typename map<monomial, T>::const_iter i = coeffs; i; i ++)
    m.mineq (i.key ());
  return m;
}

template<class T, unsigned n> pair<multivariate_polynomial<T, n>,
  multivariate_polynomial<T, n> >
multivariate_polynomial<T, n>::uncommon_factors (multivariate_polynomial b,
						 basedvector<multivariate_polynomial, 1> ds)
{
  multivariate_polynomial a = *this;
  
  monomial m = a.common_monomial ();
  m.mineq (b.common_monomial ());
  
  multivariate_polynomial mp (1, m);
  a = a.divide_exact (mp);
  b = b.divide_exact (mp);
  
 L:
  for (unsigned i = 1; i <= ds.size (); i ++)
    {
      const multivariate_polynomial &d = ds[i];
      
      maybe<multivariate_polynomial> aq = d.divides_exactly (a);
      if (aq.is_some ())
	{
	  maybe<multivariate_polynomial> bq = d.divides_exactly (b);
	  if (bq.is_some ())
	    {
	      a = aq.some ();
	      b = bq.some ();
	      
	      goto L;
	    }
	}
    }
  
  return pair<multivariate_polynomial, multivariate_polynomial> (a, b);
}

template<class T, unsigned n> maybe<multivariate_polynomial<T, n> >
multivariate_polynomial<T, n>::divides_exactly (const multivariate_polynomial &num) const
{
  const multivariate_polynomial &d = *this;
  
  multivariate_polynomial r (COPY, num);
  multivariate_polynomial q;
  
  pair<monomial, T> d_leading_term = d.coeffs.tail ();
  for (;;)
    {
      if (r == 0)
	{
	  // ??
	  assert (q*d == num);
	  
	  return maybe<multivariate_polynomial> (q);
	}
      
      pair<monomial, T> r_leading_term = r.coeffs.tail ();
      if (d_leading_term.first | r_leading_term.first)
	{      
	  multivariate_polynomial m (r_leading_term.second / d_leading_term.second,
				     r_leading_term.first - d_leading_term.first);
	  r -= m * d;
	  q += m;
	}
      else
	return maybe<multivariate_polynomial> ();
    }
}

template<class T, unsigned n> multivariate_polynomial<T, n>
multivariate_polynomial<T, n>::divide_exact (const multivariate_polynomial &d) const
{
  maybe<multivariate_polynomial> r = d.divides_exactly (*this);
  return r.some ();
}

#ifndef NDEBUG
template<class T, unsigned n> void
multivariate_polynomial<T, n>::check () const
{
  for (typename map<monomial, T>::const_iter i = coeffs; i; i ++)
    assert (i.val () != 0);
}
#endif

template<class T, unsigned n> void
multivariate_polynomial<T, n>::show_self () const
{
  unsigned first = 1;
  for (typename map<monomial, T>::const_iter i = coeffs; i; i ++)
    {
      monomial m = i.key ();
      T c = i.val ();
      
      assert (c != 0);
      
      if (first)
	first = 0;
      else
	printf (" + ");
      
      if (m.degree () == 0)
	{
	  if (c == 1)
	    printf ("1");
	  else
	    show (c);
	}
      else
	{
	  if (c != 1)
	    {
	      show (c);
	      printf ("*");
	    }
	  show (m);
	}
    }
  if (first)
    printf ("0");
}
