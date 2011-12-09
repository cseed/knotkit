
template<unsigned B> class ullmanset_iter;
template<unsigned B> class ullmanset_const_iter;

template<unsigned B>
class ullmanset
{
 private:
  friend class ullmanset_iter<B>;
  friend class ullmanset_const_iter<B>;
  
  class keypos
  {
  public:
    unsigned key;
    unsigned pos;
  };
  
  class data
  {
  public:
    unsigned refcount;
    unsigned size;
    unsigned n;
    keypos kp[1];
  };
  
  data *d;
  
  void ref (data *newd) { assert (!d); d = newd; if (d) d->refcount ++; }
  void unref ();
  
  inline void delete_at_pos (unsigned p);
  
 public:
  typedef ullmanset_iter<B> iter;
  typedef ullmanset_const_iter<B> const_iter;
  
 public:
  ullmanset () : d(0) { }
  ullmanset (unsigned size);
  ullmanset (const ullmanset &s) : d(0) { ref (s.d); }
  ullmanset (copy, const ullmanset &s);
  // ullmanset (const unsignedset &t);
  ullmanset (const bitset &t);
  ullmanset (reader &r);
  ~ullmanset () { unref (); }
  
  ullmanset &operator = (const ullmanset &s) { unref (); ref (s.d); return *this; }
  // ullmanset &operator = (const unsignedset &t);
  ullmanset &operator = (const bitset &t);
  
  unsigned size () const { return d ? d->size : 0; }
  
  bool is_empty () const { assert (d); return d->n == 0; }
  unsigned card () const { assert (d); return d->n; }
  unsigned head () const { assert (d); assert (d->n > 0); return d->kp[0].key; }
  
  bool operator == (const ullmanset &s);
  bool operator != (const ullmanset &s) { return !operator == (s); }
  
  void restore (unsigned old_card)
  {
    assert (d);
    assert (d->n >= old_card);
    d->n = old_card;
  }
  
  void clear () { if (d) { d->n = 0; } }
  
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
    assert (d);
    assert ((k - B) >= 0);
    assert ((k - B) < d->size);
    unsigned p = d->kp[k - B].pos;
    return p < d->n && d->kp[p].key == k;
  }
  bool operator () (unsigned k) const { return operator % (k); }
  
  // always 0-based
  unsigned nth (unsigned p) const { assert (d); assert (p < d->n); return d->kp[p].key; }
  
  unsigned position (unsigned k) const { assert (operator % (k)); return d->kp[k - B].pos; }
  
  void write_self (writer &w) const;
};

template<unsigned B> void
ullmanset<B>::unref ()
{
  if (d && --d->refcount == 0)
    {
      delete [] (char *)d;
      d = 0;
    }
}

template<unsigned B> void
ullmanset<B>::delete_at_pos (unsigned p)
{
  assert (d);
  assert (p < d->n);
  unsigned n = --d->n;
  if (p != n)
    {
      unsigned ell = d->kp[n].key;
      d->kp[ell - B].pos = p;
      d->kp[p].key = ell;
    }
}

template<unsigned B>
ullmanset<B>::ullmanset (unsigned size)
  : d(0)
{
  data *newd = (data *)new char[sizeof (data)
				+ sizeof (keypos) * size
				- sizeof (keypos)];
  newd->refcount = 0;
  newd->size = size;
  newd->n = 0;
  ref (newd);
}

template<unsigned B>
ullmanset<B>::ullmanset (copy, const ullmanset &s)
  : d(0)
{
  if (s.d)
    {
      unsigned bytes = (sizeof (data)
			+ sizeof (keypos) * s.d->size
			- sizeof (keypos));
      data *newd = (data *)new char[bytes];
      memcpy (newd, s.d, bytes);
      newd->refcount = 0;
      ref (newd);
    }
}

template<unsigned B>
ullmanset<B>::ullmanset (reader &r)
  : d(0)
{
  unsigned size_ = r.read_unsigned ();
  if (size_ > 0)
    {
      data *newd = (data *)new char[sizeof (data) + sizeof (keypos) * (size_ - 1)];
      newd->refcount = 0;
      newd->size = size_;
      newd->n = 0;
      ref (newd);
    }
  
  unsigned card_ = r.read_unsigned ();
  for (unsigned i = 0; i < card_; i ++)
    push (r.read_unsigned ());
}

template<unsigned B> bool
ullmanset<B>::operator == (const ullmanset &s)
{
  assert (d);
  assert (s.d);
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
  assert (d);
  assert (!operator % (k));
  unsigned p = d->n ++;
  d->kp[p].key = k;
  d->kp[k - B].pos = p;
}

template<unsigned B> void
ullmanset<B>::operator += (unsigned k)
{
  assert (d);
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
  assert (d);
  assert ((k - B) >= 0);
  assert ((k - B) < d->size);
  if (operator % (k))
    {
      unsigned p = d->kp[k - B].pos;
      delete_at_pos (p);
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
class ullmanset_iter
{
 private:
  ullmanset<B> &s;
  unsigned i;
  bool deleted;
  
 public:
  ullmanset_iter (ullmanset<B> &s_) : s(s_), i(0), deleted(0) { assert (s_.d); }
  ~ullmanset_iter () { }
  
  void del () { assert (!deleted); assert (i < s.d->n); s.delete_at_pos (i); }
  unsigned val () const { assert (!deleted); assert (i < s.d->n); return s.d->kp[i].key; }
  unsigned pos () const { assert (!deleted); assert (i < s.d->n); return i; }
  operator bool () const { assert (!deleted); return i < s.d->n; }
  void operator ++ () { if (deleted) deleted = 0; else i ++; }
  void operator ++ (int) { operator ++ (); }
};

template<unsigned B>
class ullmanset_const_iter
{
 private:
  const ullmanset<B> &s;
  unsigned i;
  
 public:
  ullmanset_const_iter (const ullmanset<B> &s_) : s(s_), i(0) { assert (s_.d); }
  ~ullmanset_const_iter () { }
  
  unsigned val () const { assert (i < s.d->n); return s.d->kp[i].key; }
  unsigned pos () const { assert (i < s.d->n); return i; }
  operator bool () const { return i < s.d->n; }
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
