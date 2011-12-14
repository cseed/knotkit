
template<class R> class linear_combination_const_iter;

template<class R>
class linear_combination
{
 private:
  // friend class linear_combination_iter<R>;
  friend class linear_combination_const_iter<R>;
  
  friend class module<R>;
  friend class free_submodule<R>;
  friend class quotient_module<R>;
  friend class mod_map<R>;
  
  typedef module<R> Rmod;
  
 public:
  // typedef linear_combination_iter<R> iter;
  typedef linear_combination_const_iter<R> const_iter;
  
 private:
  ptr<const Rmod> m;
  map<unsigned, R> v;
  
 public:
  linear_combination () { }
  explicit linear_combination (ptr<const Rmod> m_) : m(m_) { }
  linear_combination (ptr<const Rmod> m_, int x)
    : m(m_)
  {
    assert (x == 0);
  }
  
  linear_combination (const linear_combination &lc) : m(lc.m), v(lc.v) { }
  linear_combination (copy, const linear_combination &lc)
    : m(lc.m)
  {
    for (typename map<unsigned, R>::const_iter i = lc.v; i; i ++)
      v.push (i.key (), R (COPY, i.val ()));
  }
  ~linear_combination () { }

  linear_combination &operator = (const linear_combination &lc)
  {
    m = lc.m;
    v = lc.v;
    return *this;
  }
  
  bool operator == (const linear_combination &lc2) const
  {
    assert (m == lc2.m);
    linear_combination lc3 (COPY, *this);
    lc3 -= lc2;
    return lc3 == 0;
  }
  
  bool operator == (int x) const
  {
    assert (x == 0);
#ifndef NDEBUG
    check ();
#endif
    return v.is_empty ();
  }
  
  bool operator != (int x) const { return !operator == (x); }
  
  bool homogeneous () const
  {
    if (v.is_empty ())
      return 1;
    
    pair<unsigned, R> p = v.head ();
    grading hq = m->generator_grading (p.first);
    for (typename map<unsigned, R>::const_iter i = v; i; i ++)
      {
	if (hq != m->generator_grading (i.key ()))
	  return 0;
      }
    return 1;
  }
  
  pair<unsigned, R> head () const { return v.head (); }
  grading hq () const
  {
    // assert (homogeneous ());
    
    pair<unsigned, R> p = v.head ();
    return m->generator_grading (p.first);
  }
  
  R annihilator () const;
  
  void set_coeff (R c, unsigned i)
  {
    if (c == 0)
      v -= i;
    else
      v[i] = c;
  }
  
  linear_combination operator * (R c) const
  {
    linear_combination r (COPY, *this);
    r *= c;
    return r;
  }
  
  linear_combination operator + (const linear_combination &lc) const
  {
    linear_combination r (COPY, *this);
    r += lc;
    return r;
  }
  
  linear_combination operator - (const linear_combination &lc) const
  {
    linear_combination r (COPY, *this);
    r -= lc;
    return r;
  }
  
  linear_combination &operator += (unsigned i) { return muladd (1, i); }
  linear_combination &operator -= (unsigned i) { return mulsub (1, i); }
  
  linear_combination &operator *= (R c);
  linear_combination &operator /= (R c);
  
  linear_combination &operator += (const linear_combination &lc);
  linear_combination &operator -= (const linear_combination &lc);
  
  linear_combination &muladd (R c, unsigned i);
  linear_combination &muladd (R c, const linear_combination &lc);
  
  linear_combination &mulsub (R c, unsigned i);
  linear_combination &mulsub (R c, const linear_combination &lc);
  
  void yank (unsigned i) { v.yank (i); }
  void clear () { v.clear (); }
  
  bool operator % (unsigned i) const { return v % i; }
  R operator () (unsigned i) const { return v(i, R (0)); }
  
  unsigned card () const { return v.card (); }
  
#ifndef NDEBUG
  void check () const
  {
    for (typename map<unsigned, R>::const_iter i = v; i; i ++)
      assert (!m->is_zero (i.val (), i.key ()));
  }
#endif
  
  void show_self () const;
  void display_self () const { show_self (); newline (); }
};

template<class R> linear_combination<R>
operator * (R c, const linear_combination<R> &lc)
{
  return lc * c;
}

template<class R>
class linear_combination_const_iter
{
  typename map<unsigned, R>::const_iter i;
  
 public:
  linear_combination_const_iter (const linear_combination<R> &lc) : i(lc.v) { }
  ~linear_combination_const_iter () { }
  
  operator bool () const { return i; }
  linear_combination_const_iter &operator ++ () { i ++; return *this; }
  void operator ++ (int) { i ++; }
  unsigned key () const { return i.key (); }
  R val () { return i.val (); }
};

template<class R> R
linear_combination<R>::annihilator () const
{
  R r (1);
  for (typename map<unsigned, R>::const_iter i = v; i; i ++)
    r = r.lcm (m->annihilator (i.val (), i.key ()));
  return r;
}

template<class R> linear_combination<R> &
linear_combination<R>::operator *= (R c)
{
  if (c == 0)
    v.clear ();
  else if (c != 1)
    {
      for (typename map<unsigned, R>::iter i = v; i; i ++)
	i.val () *= c;
    }
  return *this;
}

template<class R> linear_combination<R> &
linear_combination<R>::operator += (const linear_combination &lc)
{
  assert (m == lc.m);
  for (typename map<unsigned, R>::const_iter i = lc.v; i; i ++)
    {
      unsigned k = i.key ();
      R &c = v[k];
      c += i.val ();
      if (m->is_zero (c, k))
	v -= k;
    }
  return *this;
}

template<class R> linear_combination<R> &
linear_combination<R>::operator -= (const linear_combination &lc)
{
  assert (m == lc.m);
  for (typename map<unsigned, R>::const_iter i = lc.v; i; i ++)
    {
      unsigned k = i.key ();
      R &c = v[k];
      c -= i.val ();
      if (m->is_zero (c, k))
	v -= k;
    }
  return *this;
}

template<class R> linear_combination<R> &
linear_combination<R>::muladd (R c, unsigned i)
{
  R &ic = v[i];
  ic += c;
  if (m->is_zero (ic, i))
    v -= i;
  return *this;
}

template<class R> linear_combination<R> &
linear_combination<R>::muladd (R c, const linear_combination &lc)
{
  assert (m == lc.m);
  for (typename map<unsigned, R>::const_iter i = lc.v; i; i ++)
    {
      unsigned k = i.key ();
      R &vc = v[k];
      vc += (c * i.val ());
      if (vc == 0)
	v -= k;
    }
  return *this;
}

template<class R> linear_combination<R> &
linear_combination<R>::mulsub (R c, unsigned i)
{
  R &ic = v[i];
  ic -= c;
  if (m->is_zero (ic, i))
    v -= i;
  return *this;
}

template<class R> linear_combination<R> &
linear_combination<R>::mulsub (R c, const linear_combination &lc)
{
  assert (m == lc.m);
  for (typename map<unsigned, R>::const_iter i = lc.v; i; i ++)
    {
      unsigned k = i.key ();
      R &vc = v[k];
      vc -= (c * i.val ());
      if (vc == 0)
	v -= k;
    }
  return *this;
}

template<class R> void
linear_combination<R>::show_self () const
{
  bool first = 1;
  for (typename map<unsigned, R>::const_iter i = v; i; i ++)
    {
      if (first)
	first = 0;
      else
	printf (" + ");
      show (i.val ());
      printf ("*");
      m->show_generator (i.key ());
    }
}

template<>
class linear_combination<Z2>
{
 private:
  // friend class linear_combination_iter<Z2>;
  friend class linear_combination_const_iter<Z2>;
  
  friend class module<Z2>;
  friend class free_submodule<Z2>;
  friend class quotient_module<Z2>;
  friend class mod_map<Z2>;
  
  typedef module<Z2> Z2mod;
  
 public:
  typedef linear_combination_const_iter<Z2> const_iter;
  
 private:
  ptr<const Z2mod> m;
  set<unsigned> v;
  
 public:
  linear_combination () { }
  linear_combination (ptr<const Z2mod> m_) : m(m_) { }
  linear_combination (const linear_combination &lc) : m(lc.m), v(lc.v) { }
  linear_combination (copy, const linear_combination &lc) : m(lc.m), v(COPY, lc.v) { }
  ~linear_combination () { }
  
  linear_combination &operator = (const linear_combination &lc)
  { 
    m = lc.m; 
    v = lc.v; 
    return *this;
  }
  
  bool operator == (const linear_combination &lc) const { assert (m == lc.m); return v == lc.v; }
  
  bool operator == (int x) const { assert (x == 0); return v.is_empty (); }
  bool operator != (int x) const { return !operator == (x); }
  
  pair<unsigned, Z2> head () const { return pair<unsigned, Z2> (v.head (), Z2 (1)); }
  grading hq () const
  {
    // assert (homogeneous ());
    return m->generator_grading (v.head ());
  }
  
  Z2 annihilator () const
  {
    return Z2 (operator == (0) ? 1 : 0);
  }
  
  void set_coeff (Z2 c, unsigned i)
  {
    if (c == 0)
      v -= i;
    else
      v += i;
  }
  
  linear_combination operator * (Z2 c) const
  {
    if (c == 1)
      return *this;
    else
      return linear_combination (m);
  }
  
  linear_combination operator + (const linear_combination &lc) const
  {
    linear_combination r (COPY, *this);
    r += lc;
    return r;
  }
  
  linear_combination operator - (const linear_combination &lc) const
  {
    linear_combination r (COPY, *this);
    r -= lc;
    return r;
  }
  
  linear_combination &operator *= (Z2 c)
  {
    if (c == 0)
      v.clear ();
    return *this;
  }
  linear_combination &operator /= (Z2 c) { assert (c == 1); return *this; }
  
  linear_combination &operator += (unsigned i) { v.toggle (i); return *this; }
  linear_combination &operator -= (unsigned i) { v.toggle (i); return *this; }
  
  linear_combination &muladd (Z2 c, unsigned i)
  {
    if (c == 1)
      operator += (i);
    return *this;
  }
  
  linear_combination &mulsub (Z2 c, unsigned i)
  {
    if (c == 1)
      operator -= (i);
    return *this;
  }
  
  linear_combination &operator += (const linear_combination &lc) { v ^= lc.v; return *this; }
  linear_combination &operator -= (const linear_combination &lc) { v ^= lc.v; return *this; }
  
  linear_combination &muladd (Z2 c, const linear_combination &lc)
  {
    if (c == 1)
      operator += (lc);
    return *this;
  }
  linear_combination &mulsub (Z2 c, const linear_combination &lc)
  {
    if (c == 1)
      operator -= (lc);
    return *this;
  }
  
  void yank (unsigned i) { v.yank (i); }
  void clear () { v.clear (); }
  
  bool operator % (unsigned i) const { return v % i; }
  Z2 operator () (unsigned i) const { return Z2 (v % i); }
  
  unsigned card () const { return v.card (); }
  
#ifndef NDEBUG
  void check () const;
#endif
  
  void show_self () const;
  void display_self () const { show_self (); newline (); }
};

inline void
linear_combination<Z2>::show_self () const
{
  bool first = 1;
  for (set_const_iter<unsigned> i = v; i; i ++)
    {
      if (first)
	first = 0;
      else
	printf ("+");
      m->show_generator(i.val ());
      //printf ("%d", );
    }
}

inline linear_combination<Z2>
operator * (Z2 c, const linear_combination<Z2> &lc)
{
  return lc * c;
}

template<>
class linear_combination_const_iter<Z2>
{
  set<unsigned>::const_iter i;
  
 public:
  linear_combination_const_iter (const linear_combination<Z2> &lc) : i(lc.v) { }
  ~linear_combination_const_iter () { }
  
  operator bool () const { return i; }
  linear_combination_const_iter &operator ++ () { i ++; return *this; }
  void operator ++ (int) { i ++; }
  unsigned key () const { return i.val (); }
  Z2 val () { return Z2 (1); }
};
