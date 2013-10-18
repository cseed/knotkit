
template<unsigned B> class ullmanset_iter;
template<unsigned B> class ullmanset_const_iter;

template<unsigned B> class ullmanset_const_stl_iter;

template<unsigned B>
class ullmanset
{
 private:
  friend class ullmanset_iter<B>;
  friend class ullmanset_const_iter<B>;
  friend class ullmanset_const_stl_iter<B>;
  
  class keypos
  {
  public:
    unsigned key;
    unsigned pos;
  };
  
  class data : public refcounted
  {
  public:
    unsigned size;
    unsigned n;
    keypos kp[1];
    
  public:
    inline void delete_at_pos (unsigned p);
  };
  
  ptr<data> d;
  
 public:
  typedef ullmanset_iter<B> iter;
  typedef ullmanset_const_iter<B> const_iter;
  
 public:
  ullmanset (unsigned size);
  ullmanset () : ullmanset(0) { }
  ullmanset (const ullmanset &s) : d(s.d) { }
  ullmanset (copy, const ullmanset &s);
  ullmanset (const bitset &t);
  ullmanset (unsigned size, initializer_list<unsigned> il);
  ullmanset (reader &r);
  ~ullmanset () { }
  
  ullmanset &operator = (const ullmanset &s) { d = s.d; return *this; }
  ullmanset &operator = (const bitset &t);
  
  // range-based for
  inline ullmanset_const_stl_iter<B> begin () const;
  inline ullmanset_const_stl_iter<B> end () const;
  
  unsigned size () const { return d->size; }
  
  bool is_empty () const { return d->n == 0; }
  unsigned card () const { return d->n; }
  unsigned head () const { assert (d->n > 0); return d->kp[0].key; }
  
  bool operator == (const ullmanset &s);
  bool operator != (const ullmanset &s) { return !operator == (s); }
  
  void restore (unsigned old_card)
  {
    assert (d->n >= old_card);
    d->n = old_card;
  }
  
  void clear () { d->n = 0; }
  
  inline void push (unsigned k);
  inline void operator += (unsigned k); // ??? should these be inline?
  inline void operator -= (unsigned k);
  
  void yank (unsigned k) { assert (operator % (k)); operator -= (k); }
  
  void toggle (unsigned k)
  {
    if (operator % (k))
      operator -= (k);
    else
      operator += (k);
  }
  
  ullmanset &operator |= (const ullmanset &s);
  ullmanset &operator &= (const ullmanset &s);
  ullmanset &operator ^= (const ullmanset &s);
  
  bool operator % (unsigned k) const
  {
    assert ((k - B) >= 0);
    assert ((k - B) < d->size);
    unsigned p = d->kp[k - B].pos;
    return p < d->n && d->kp[p].key == k;
  }
  
  bool operator () (unsigned k) const { return operator % (k); }
  
  // always 0-based
  unsigned nth (unsigned p) const
  {
    assert (p < d->n);
    return d->kp[p].key;
  }
  
  unsigned position (unsigned k) const
  {
    assert (operator % (k));
    return d->kp[k - B].pos;
  }
  
  void write_self (writer &w) const;
};

template<unsigned B> void
ullmanset<B>::data::delete_at_pos (unsigned p)
{
  assert (p < n);
  --n;
  if (p != n)
    {
      unsigned ell = kp[n].key;
      kp[ell - B].pos = p;
      kp[p].key = ell;
    }
}

template<unsigned B>
ullmanset<B>::ullmanset (unsigned size)
{
  data *newd = (data *)new char[sizeof (data)
				+ sizeof (keypos) * size
				- sizeof (keypos)];
  new (newd) data;
  newd->size = size;
  newd->n = 0;
  d = newd;
}

template<unsigned B>
ullmanset<B>::ullmanset (unsigned size, initializer_list<unsigned> il)
  : ullmanset(size)
{
  for (unsigned i : il)
    operator += (i);
}

template<unsigned B>
ullmanset<B>::ullmanset (copy, const ullmanset &s)
  : ullmanset(s.size ())
{
  d->n = s.d->n;
  memcpy (&d->kp[0], &s.d->kp[0], sizeof (keypos) * d->n);
}

template<unsigned B>
ullmanset<B>::ullmanset (reader &r)
  : ullmanset(r.read_unsigned ())
{
  unsigned card_ = r.read_unsigned ();
  for (unsigned i = 0; i < card_; i ++)
    push (r.read_unsigned ());
}

template<unsigned B> bool
ullmanset<B>::operator == (const ullmanset &s)
{
  if (d->n != s.d->n)
    return 0;
  for (iter i = *this; i; i ++)
    {
      if (!s(i.val ()))
	return 0;
    }
  return 1;
}

template<unsigned B> void
ullmanset<B>::push (unsigned k)
{
  assert (!operator % (k));
  unsigned p = d->n ++;
  d->kp[p].key = k;
  d->kp[k - B].pos = p;
}

template<unsigned B> void
ullmanset<B>::operator += (unsigned k)
{
  assert ((k - B) >= 0);
  assert ((k - B) < d->size);
  if (!operator % (k))
    {
      unsigned p = d->n ++;
      d->kp[p].key = k;
      d->kp[k - B].pos = p;
    }
}

template<unsigned B> void
ullmanset<B>::operator -= (unsigned k)
{
  assert ((k - B) >= 0);
  assert ((k - B) < d->size);
  if (operator % (k))
    {
      unsigned p = d->kp[k - B].pos;
      d->delete_at_pos (p);
    }
}
  
template<unsigned B> ullmanset<B> &
ullmanset<B>::operator |= (const ullmanset &s)
{
  for (iter i = s; i; i ++)
    operator += (i.val ());
}

template<unsigned B> ullmanset<B> &
ullmanset<B>::operator &= (const ullmanset<B> &s)
{
  for (iter i = *this; i; i ++)
    {
      unsigned k = i.val ();
      if (!s(k))
	i.del ();
    }
}

template<unsigned B> ullmanset<B> &
ullmanset<B>::operator ^= (const ullmanset<B> &s)
{
  for (const_iter i = s; i; i ++)
    {
      int k = i.val ();
      if (operator % (k))
	operator -= (k);
      else
	push (k);
    }
  return *this;
}


template<unsigned B> void
ullmanset<B>::write_self (writer &w) const
{
  write (w, size ());
  write (w, card ());
  for (const_iter i = *this; i; i ++)
    write (w, i.val ());
}

template<unsigned B>
class ullmanset_const_stl_iter
{
 private:
  ptr<typename ullmanset<B>::data> d;
  unsigned p;
  
 public:
  ullmanset_const_stl_iter (const ullmanset<B> &s, unsigned p_) : d(s.d), p(p_) { }
  
  bool operator != (const ullmanset_const_stl_iter &end) const { return p != end.p; }
  unsigned operator * () const { assert (p < d->n); return d->kp[p].key; }
  ullmanset_const_stl_iter &operator ++ () { p ++; return *this; }
};

template<unsigned B> ullmanset_const_stl_iter<B>
ullmanset<B>::begin () const
{
  return ullmanset_const_stl_iter<B> (*this, 0);
}

template<unsigned B> ullmanset_const_stl_iter<B>
ullmanset<B>::end () const
{
  return ullmanset_const_stl_iter<B> (*this, d->n);
}

template<unsigned B>
class ullmanset_iter
{
 private:
  ptr<typename ullmanset<B>::data> d;
  unsigned i;
  bool deleted;
  
 public:
  ullmanset_iter (ullmanset<B> &s) : d(s.d), i(0), deleted(0) { }
  ~ullmanset_iter () { }
  
  void del () { assert (!deleted); assert (i < d->n); d->delete_at_pos (i); }
  unsigned val () const { assert (!deleted); assert (i < d->n); return d->kp[i].key; }
  unsigned pos () const { assert (!deleted); assert (i < d->n); return i; }
  operator bool () const { assert (!deleted); return i < d->n; }
  void operator ++ () { if (deleted) deleted = 0; else i ++; }
  void operator ++ (int) { operator ++ (); }
};

template<unsigned B>
class ullmanset_const_iter
{
 private:
  ptr<typename ullmanset<B>::data> d;
  unsigned i;
  
 public:
  ullmanset_const_iter (const ullmanset<B> &s) : d(s.d), i(0) { }
  ~ullmanset_const_iter () { }
  
  unsigned val () const { assert (i < d->n); return d->kp[i].key; }
  unsigned pos () const { assert (i < d->n); return i; }
  operator bool () const { return i < d->n; }
  void operator ++ () { i ++; }
  void operator ++ (int) { i ++; }
};

/* interoperating with other set types */
template<unsigned B>
set<unsigned> &operator |= (set<unsigned> &s, const ullmanset<B> &t)
{
  for (ullmanset_const_iter<B> i = t; i; i ++)
    s += i.val ();
  return s;
}

template<unsigned B>
ullmanset<B> &operator |= (ullmanset<B> &s, const set<unsigned> &t)
{
  for (set_const_iter<unsigned> i = t; i; i ++)
    s += i.val ();
  return s;
}

template<unsigned B>
ullmanset<B> &operator &= (ullmanset<B> &s, const set<unsigned> &t)
{
  for (ullmanset_const_iter<B> i = s; i; i ++)
    {
      int k = i.val ();
      if (!t(k))
	i.del ();
    }
  return s;
}

template<unsigned B>
ullmanset<B> &operator ^= (ullmanset<B> &s, const set<unsigned> &t)
{
  for (set_const_iter<unsigned> i = t; i; i ++)
    {
      int k = i.val ();
      if (s % k)
	s -= (k);
      else
	s.push (k);
    }
  return s;
}
