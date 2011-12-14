
template<class C, class E> class laurentpoly;

class exponent1
{
 public:
  int first;
  
 public:
  exponent1 () : first(0) { }
  explicit exponent1 (unsigned first_) : first(first_) { }
  exponent1 (const exponent1 &e) : first(e.first) { }
  ~exponent1 () { }
  
  exponent1 &operator = (const exponent1 &e) { first = e.first; return *this; }
  
  inline laurentpoly<int, exponent1> operator + (const exponent1 &e) const;
  
  exponent1 &operator *= (const exponent1 &e) { first += e.first; return *this; }
  exponent1 operator * (const exponent1 &e) const { return exponent1 (first + e.first); }
  
  template<class T> laurentpoly<T, exponent1> operator * (const T &c) const
  {
    laurentpoly<T, exponent1> r;
    r.addeq (c, *this);
    return r;
  }
  
  bool operator == (int x) const { assert (x == 0); return !first; }
  bool operator != (int x) const { return !operator == (x); }
  
  bool operator == (const exponent1 &e) const { return first == e.first; }
  bool operator != (const exponent1 &e) const { return !operator == (e); }
  bool operator < (const exponent1 &e) const { return first < e.first; }
  
  exponent1 operator ^ (int p) const { return exponent1 (first * p); }
  
  void show_self () const { printf ("(%d)", first); }
  void display_self () const { show_self (); newline (); }
};

class exponent2
{
 public:
  int first,
    second;
  
 public:
  exponent2 () : first(0), second(0) { }
  explicit exponent2 (int i) : first(0), second(0) { assert (i == 0); }
  exponent2 (int first_, int second_) : first(first_), second(second_) { }
  exponent2 (const exponent2 &e) : first(e.first), second(e.second) { }
  ~exponent2 () { }
  
  exponent2 &operator = (const exponent2 &e) { first = e.first; second = e.second; return *this; }
  
  inline laurentpoly<int, exponent2> operator + (const exponent2 &e) const;
  
  exponent2 &operator *= (const exponent2 &e) { first += e.first; second += e.second; return *this; }
  exponent2 operator * (const exponent2 &e) const { return exponent2 (first + e.first, second + e.second); }
  
  template<class T> laurentpoly<T, exponent2> operator * (const T &c) const
  {
    laurentpoly<T, exponent2> r;
    r.addeq (c, *this);
    return r;
  }
  
  bool operator == (int x) const { assert (x == 0); return !first && !second; }
  bool operator != (int x) const { return !operator == (x); }
  
  bool operator == (const exponent2 &e) const { return first == e.first && second == e.second; }
  bool operator != (const exponent2 &e) const { return !operator == (e); }
  bool operator < (const exponent2 &e) const { return first < e.first
      || (first == e.first && second < e.second); }
  
  exponent2 operator ^ (int p) const { return exponent2 (first * p, second * p); }
  
  void show_self () const { printf ("(%d,%d)", first, second); }
  void display_self () const { show_self (); newline (); }
  
  unsigned texwidth () const
  {
    if (!first && !second)
      return 1;
    
    unsigned w = 0;
    if (first == 1)
      w ++;
    else if (first < 0)
      w += 3;
    else if (first)
      w += 2;
    
    if (second == 1)
      w ++;
    else if (second < 0)
      w += 3;
    else if (second)
      w += 2;
    
    return w;
  }
  void texshow (FILE *fp) const
  {
    if (!first && !second)
      printf ("1");
    else
      {
	if (first == 1)
	  printf ("t");
	else if (first)
	  printf ("t^{%d}", first);
	
	if (second == 1)
	  printf ("q");
	else if (second)
	  printf ("q^{%d}", second);
      }
  }
};

#if 0
template<class T, class E> laurentpoly<T, E> operator * (const T &c, const E &e)
{
  return e*c;
}
#endif

template<class C, class E>
class laurentpoly
{
public:
  typedef typename map<E, C>::iter map_iter;
  typedef typename map<E, C>::const_iter map_const_iter;
  
  map<E, C> m;
  
public:
  laurentpoly () { }
  explicit laurentpoly (const C &c) { m.push (E (0), c); }
  explicit laurentpoly (const E &e) { m.push (e, 1); }
  laurentpoly (const laurentpoly &p) : m(p.m) { }
  laurentpoly (copy, const laurentpoly &p) : m(COPY, p.m) { }
  ~laurentpoly () { }
  
  laurentpoly &operator = (const laurentpoly &p) { m = p.m; return *this; }
  laurentpoly &operator = (const C &c) { m.clear (); m.push (E (0), c); }
  
  bool operator == (const laurentpoly &p) { return m == p.m; }
  bool operator != (const laurentpoly &p) { return m != p.m; }
  
  laurentpoly &operator += (const laurentpoly &p);
  laurentpoly &operator -= (const laurentpoly &p);
  laurentpoly &operator *= (const laurentpoly &p) { operator = (*this * p); }
  laurentpoly &operator *= (const C &s);

  laurentpoly &subeq (const C &s, const E &e);
  laurentpoly &addeq (const C &s, const E &e);
  
  laurentpoly &muleq (const C &s, const E &e);
  laurentpoly &muladdeq (const C &s, const E &e, const laurentpoly &p);
  
  laurentpoly operator - () const;

  laurentpoly operator + (const E &e) const { return *this + laurentpoly (e); }
  laurentpoly operator + (const C &c) const { return *this + laurentpoly (c); }
  
  laurentpoly operator + (const laurentpoly &p) const;
  laurentpoly operator - (const laurentpoly &p) const;
  laurentpoly operator * (const laurentpoly &p) const;
  laurentpoly operator * (const E &e) const;
  laurentpoly operator * (const C &s) const;
  
  C aug () const
  {
    C r = 0;
    for (map_const_iter i = m; i; i ++)
      r += i.val ();
    return r;
  }
  void show_self () const;
  void display_self () const { show_self (); newline (); }
  void texshow (FILE *fp);
};

template<class C, class E> inline laurentpoly<C, E> operator + (const E &e, const laurentpoly<C, E> &p) { return p + e; }
template<class C, class E> inline laurentpoly<C, E> operator + (const C &c, const laurentpoly<C, E> &p) { return p + c; }

template<class C, class E> inline laurentpoly<C, E> operator * (const C &s, const laurentpoly<C, E> &p) { return p * s; }

template<class C, class E> laurentpoly<C, E> &
laurentpoly<C, E>::operator += (const laurentpoly &p)
{
  for (map_const_iter i = p.m; i; i ++)
    addeq (i.val (), i.key ());
  return *this;
}

template<class C, class E> laurentpoly<C, E> &
laurentpoly<C, E>::operator -= (const laurentpoly &p)
{
  for (map_const_iter i = p.m; i; i ++)
    subeq (i.val (), i.key ());
  return *this;
}

template<class C, class E> laurentpoly<C, E> &
laurentpoly<C, E>::operator *= (const C &s)
{
  for (map_iter i = m; i; i ++)
    i.val () *= s;
  return *this;
}

template<class C, class E> laurentpoly<C, E> &
laurentpoly<C, E>::addeq (const C &s, const E &e)
{
  pair<C &, bool> x = m.find (e);
  if (!x.second)
    x.first = s;
  else
    x.first += s;
  return *this;
}

template<class C, class E> laurentpoly<C, E> &
laurentpoly<C, E>::subeq (const C &s, const E &e)
{
  pair<C &, bool> x = m.find (e);
  if (!x.second)
    x.first = -s;
  else
    x.first -= s;
  return *this;
}

template<class C, class E> laurentpoly<C, E> &
laurentpoly<C, E>::muleq (const C &s, const E &e)
{
  map<C, E> new_m;
  for (map_const_iter i = m; i; i ++)
    m.push (e * m.key (), s * m.val ());
  m = new_m;
  return *this;
}

template<class C, class E> laurentpoly<C, E> &
laurentpoly<C, E>::muladdeq (const C &s, const E &e, const laurentpoly &p)
{
  for (map_const_iter i = p.m; i; i ++)
    addeq (s * i.val (), e * i.key ());
  return *this;
}

template<class C, class E> laurentpoly<C, E>
laurentpoly<C, E>::operator - () const
{
  map<C, E> new_m;
  for (map_const_iter i = m; i; i ++)
    new_m.push (m.key (), -m.val ());
  return laurentpoly (new_m);
}

template<class C, class E> laurentpoly<C, E>
laurentpoly<C, E>::operator + (const laurentpoly &p) const
{
  laurentpoly r (COPY, *this);
  r += p;
  return r;
}

template<class C, class E> laurentpoly<C, E>
laurentpoly<C, E>::operator - (const laurentpoly &p) const
{
  laurentpoly r (COPY, *this);
  r -= p;
  return r;
}

template<class C, class E> laurentpoly<C, E>
laurentpoly<C, E>::operator * (const laurentpoly &p) const
{
  laurentpoly r;
  for (map_const_iter i = m; i; i ++)
    r.muladdeq (i.val (), i.key (), p);
  return r;
}

template<class C, class E> laurentpoly<C, E>
laurentpoly<C, E>::operator * (const C &s) const
{
  laurentpoly r (COPY, *this);
  for (map_iter i = r.m; i; i ++)
    i.val () *= s;
  return r;
}

template<class C, class E> laurentpoly<C, E>
laurentpoly<C, E>::operator * (const E &e) const
{
  laurentpoly r;
  for (map_const_iter i = m; i; i ++)
    r.addeq (i.val (), i.key () * e);
  return r;
}

template<class C, class E> void
laurentpoly<C, E>::show_self () const
{
  if (m.card () == 0)
    {
      printf ("0");
      return;
    }
  
  bool first = 1;
  for (map_const_iter i = m; i; i ++)
    {
      if (i.val () != 0)
	{
	  if (!first)
	    printf (" + ");
	  
	  if (i.val () == 1)
	    show (i.key ());
	  else
	    {
	      show (i.val ());
	      printf ("*");
	      show (i.key ());
	    }
	  first = 0;
	}
    }
}

template<class C, class E> void
laurentpoly<C, E>::texshow (FILE *fp)
{
  bool first = 1;
  unsigned w = 0;
  for (map_const_iter i = m; i; i ++)
    {
      if (i.val () == 0)
	continue;
      
      if (w > 47)
	{
	  fprintf (fp, " \\\\\n");
	  fprintf (fp, " & & & ");
	  w = 0;
	}
      
      if (first)
	first = 0;
      else
	{
	  printf ("+");
	  w += 3;
	}
      
      if (i.val () == 1)
	{
	  i.key ().texshow (fp);
	  w += i.key ().texwidth ();
	}
      else
	{
	  fprintf (fp, "%d", i.val ());
	  w ++;
	  if (i.key () != 0)
	    {
	      i.key ().texshow (fp);
	      w += i.key ().texwidth ();
	    }
	}
    }
}

typedef laurentpoly<int, exponent1> intpoly1;
typedef laurentpoly<int, exponent2> intpoly2;

inline laurentpoly<int, exponent1> 
exponent1::operator + (const exponent1 &e) const
{
  intpoly1 r;
  r.addeq (1, *this);
  r.addeq (1, e);
  return r;
}

inline laurentpoly<int, exponent2> 
exponent2::operator + (const exponent2 &e) const
{
  intpoly2 r;
  r.addeq (1, *this);
  r.addeq (1, e);
  return r;
}
