
/* multivariate polynomial in a (vector) variable x with coefficients
   in T. */

class multivariate_laurent_monomial
{
 public:
  map<unsigned, int> m;
  
  explicit multivariate_laurent_monomial (const map<unsigned, int> &m_) : m(m_) { }
  
 public:
  multivariate_laurent_monomial ()  { }
  
  multivariate_laurent_monomial (variable, unsigned j)
  {
    m.push (j, 1);
  }
  
  multivariate_laurent_monomial (variable, unsigned j, int e)
  {
    if (e != 0)
      m.push (j, e);
  }
  
  multivariate_laurent_monomial (const multivariate_laurent_monomial &e) : m(e.m) { }
  multivariate_laurent_monomial (copy, const multivariate_laurent_monomial &e)
    : m(COPY, e.m)
  {
  }
  
  multivariate_laurent_monomial (reader &r) : m(r) { }
  
  ~multivariate_laurent_monomial () { }
  
  multivariate_laurent_monomial &operator = (const multivariate_laurent_monomial &e)
  {
    m = e.m;
    return *this;
  }
  
  unsigned degree () const
  {
    int d = 0;
    for (map<unsigned, int>::const_iter i = m; i; i ++)
      d += i.val ();
    return d;
  }
  
  bool operator == (const multivariate_laurent_monomial &e) const
  {
#ifndef NDEBUG
    check ();
    e.check ();
#endif
    return m == e.m;
  }
  bool operator != (const multivariate_laurent_monomial &e) const { return !operator == (e); }
  
  bool operator == (int x) const
  {
    assert (x == 1);
#ifndef NDEBUG
    check ();
#endif
    return m.is_empty ();
  }
  
  bool operator != (int x) const { return !operator == (x); }
  
  bool operator < (const multivariate_laurent_monomial &e) const
  {
#ifndef NDEBUG
    check ();
    e.check ();
#endif
    return m < e.m;
  }
  
  bool operator <= (const multivariate_laurent_monomial &e) const
  {
#ifndef NDEBUG
    check ();
    e.check ();
#endif
    return m <= e.m;
  }
  
  multivariate_laurent_monomial &operator *= (const multivariate_laurent_monomial &e)
  {
    for (map<unsigned, int>::const_iter i = e.m; i; i ++)
      {
	pair<int &, bool> p = m.find (i.key ());
	if (p.second)
	  {
	    p.first += i.val ();
	    if (p.first == 0)
	      m -= i.key ();
	  }
	else
	  p.first = i.val ();
      }
#ifndef NDEBUG
    check ();
#endif
    return *this;
  }
  
  multivariate_laurent_monomial operator * (const multivariate_laurent_monomial &e) const
  {
    multivariate_laurent_monomial r;
    r *= *this;
    r *= e;
    return r;
  }
  
  int operator () (unsigned j) const
  {
    return m(j, 0);
  }
  
  void set_exponent (unsigned j, int e)
  {
    if (e)
      m[j] = e;
    else
      m -= j;
  }
  
  void push_exponent (unsigned j, int e)
  {
    assert (! (m % j));
    if (e != 0)
      m.push (j, e);
  }
  
  void show_self () const
  {
    for (map<unsigned, int>::const_iter i = m; i; i ++)
      {
	assert (i.val () != 0);
	if (i.val () == 1)
	  printf ("x%d", i.key ());
	else
	  printf ("x%d^%d", i.key (), i.val ());
      }
  }
  
  void write_self (writer &w) const { write (w, m); }
  
#ifndef NDEBUG
  void check () const
  {
    for (map<unsigned, int>::const_iter i = m; i; i ++)
      assert (i.val () != 0);
  }
#endif
};

template<class T>
class multivariate_laurentpoly
{
 public:
  typedef ::linear_combination<multivariate_laurentpoly<T> > linear_combination;
  typedef ::linear_combination_const_iter<multivariate_laurentpoly<T> >
    linear_combination_const_iter;
  
 public:
  typedef multivariate_laurent_monomial monomial;
  
  map<monomial, T> coeffs;
  
  explicit multivariate_laurentpoly (const map<monomial, T> &coeffs_) : coeffs(coeffs_) { }
  
  void normalize ()
  {
    for (typename map<monomial, T>::iter i = coeffs; i; i ++)
      {
	if (i.val () == 0)
	  i.del ();
      }
#ifndef NDEBUG
    check ();
#endif
  }
  
 public:
  multivariate_laurentpoly () { }
  multivariate_laurentpoly (int x)
  {
    T c (x);
    if (c != 0)
      coeffs.push (monomial (), c);
  }
  
  multivariate_laurentpoly (T c)
  {
    if (c != 0)
      coeffs.push (monomial (), c);
  }
  
  multivariate_laurentpoly (T c, variable, unsigned i)
  {
    if (c != 0)
      coeffs.push (monomial (VARIABLE, i), c);
  }
  
  multivariate_laurentpoly (T c, variable, unsigned i, int e)
  {
    if (c != 0)
      coeffs.push (monomial (VARIABLE, i, e), c);
  }
  
  multivariate_laurentpoly (T c, const monomial &m)
  {
    if (c != 0)
      coeffs.push (m, c);
  }
  
  multivariate_laurentpoly (const multivariate_laurentpoly &p) : coeffs(p.coeffs) { }
  multivariate_laurentpoly (copy, const multivariate_laurentpoly &p)
    : coeffs(COPY2, p.coeffs)
  {
  }

  multivariate_laurentpoly (reader &r) : coeffs(r) { }
  
  ~multivariate_laurentpoly () { }
  
  multivariate_laurentpoly &operator = (const multivariate_laurentpoly &p)
  {
    coeffs = p.coeffs;
    return *this;
  }
  
  multivariate_laurentpoly &operator = (int x) { return operator = (T (x)); }
  multivariate_laurentpoly &operator = (T c)
  {
    coeffs = map<monomial, T> ();
    if (c != 0)
      coeffs.push (monomial (), c);
    return *this;
  }
  
  bool operator == (const multivariate_laurentpoly &p) const
  {
#ifndef NDEBUG
    check ();
    p.check ();
#endif
    return coeffs == p.coeffs;
  }
  bool operator != (const multivariate_laurentpoly &p) const { return !operator == (p); }
  
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
	return p.first == 1
	  && p.second == c;
      }
  }
  
  bool operator != (int x) const { return !operator == (x); }
  
  bool operator < (const multivariate_laurentpoly &p) const
  {
#ifndef NDEBUG
    check ();
    p.check ();
#endif
    return coeffs < p.coeffs;
  }
  
  bool operator <= (const multivariate_laurentpoly &p) const
  {
#ifndef NDEBUG
    check ();
    p.check ();
#endif
    return coeffs <= p.coeffs;
  }
  
  unsigned card () const { return coeffs.card (); }
  pair<monomial, T> head () const { return coeffs.head (); }
  
  multivariate_laurentpoly &operator += (const multivariate_laurentpoly &p);
  multivariate_laurentpoly &operator -= (const multivariate_laurentpoly &p);
  multivariate_laurentpoly &operator *= (const multivariate_laurentpoly &p)
  {
    return operator = (*this * p);
  }
  
  multivariate_laurentpoly &operator *= (T s);
  
  multivariate_laurentpoly &muladdeq (T c, variable, unsigned i)
  {
    monomial m (VARIABLE, i);
    T &c2 = coeffs[m];
    c2 += c;
    if (c2 == 0)
      coeffs -= m;
    return *this;
  }
  
  multivariate_laurentpoly &muladdeq (T c, const monomial &m)
  {
    T &c2 = coeffs[m];
    c2 += c;
    if (c2 == 0)
      coeffs -= m;
    return *this;
  }
  
  multivariate_laurentpoly &muladdeq (multivariate_laurentpoly a, multivariate_laurentpoly b);
  
  multivariate_laurentpoly operator - () const { return multivariate_laurentpoly () - *this; }
  multivariate_laurentpoly operator + (const multivariate_laurentpoly &p) const
  {
    multivariate_laurentpoly r (COPY, *this);
    r += p;
    return r;
  }
  
  multivariate_laurentpoly operator - (const multivariate_laurentpoly &p) const
  {
    multivariate_laurentpoly r (COPY, *this);
    r -= p;
    return r;
  }
  
  multivariate_laurentpoly operator * (const multivariate_laurentpoly &p) const;
  
#ifndef NDEBUG
  void check () const;
#endif
  
  static void show_ring ()
  {
    T::show_ring ();
    printf ("[x_1^+/-1, ..., x_n^+/-1]");
  }
  
  void display_self () const { show_self (); newline (); }
  void show_self () const;
  void write_self (writer &w) const { write (w, coeffs); }
};

template<class T> multivariate_laurentpoly<T>
operator * (const T &s, const multivariate_laurentpoly<T> &p)
{
  multivariate_laurentpoly<T> r (COPY, p);
  r *= s;
  return r;
}

template<class T> multivariate_laurentpoly<T> &
multivariate_laurentpoly<T>::operator += (const multivariate_laurentpoly &p)
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

template<class T> multivariate_laurentpoly<T> &
multivariate_laurentpoly<T>::operator -= (const multivariate_laurentpoly &p)
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

template<class T> multivariate_laurentpoly<T> &
multivariate_laurentpoly<T>::operator *= (T s)
{
  if (s == 0)
    coeffs.clear ();
  else
    {
      for (typename map<monomial, T>::iter i = coeffs; i; i ++)
	i.val () *= s;
      normalize ();
    }
  return *this;
}

template<class T> multivariate_laurentpoly<T>
multivariate_laurentpoly<T>::operator * (const multivariate_laurentpoly &p) const
{
  multivariate_laurentpoly r;
  
  for (typename map<monomial, T>::const_iter i = coeffs; i; i ++)
    for (typename map<monomial, T>::const_iter j = p.coeffs; j; j ++)
      {
	monomial m = i.key () * j.key ();
	r.coeffs[m].muladdeq (i.val (), j.val ());
      }
  r.normalize ();
  return r;
}

template<class T> multivariate_laurentpoly<T> &
multivariate_laurentpoly<T>::muladdeq (multivariate_laurentpoly a, multivariate_laurentpoly b)
{
  for (typename map<monomial, T>::const_iter i = a.coeffs; i; i ++)
    for (typename map<monomial, T>::const_iter j = b.coeffs; j; j ++)
      {
	monomial m = i.key () * j.key ();
	coeffs[m].muladdeq (i.val (), j.val ());
      }
  normalize ();
  return *this;
}

#ifndef NDEBUG
template<class T> void
multivariate_laurentpoly<T>::check () const
{
  for (typename map<monomial, T>::const_iter i = coeffs; i; i ++)
    assert (i.val () != 0);
}
#endif

template<class T> void
multivariate_laurentpoly<T>::show_self () const
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
      
      if (m == 1)
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
