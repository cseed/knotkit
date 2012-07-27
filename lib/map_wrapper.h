
/* wrapper for STL maps */

template<class M, class K, class V> class map_wrapper_iter;
template<class M, class K, class V> class map_wrapper_const_iter;

template<class M, class K, class V>
class map_wrapper
{
  friend class map_wrapper_iter<M, K, V>;
  friend class map_wrapper_const_iter<M, K, V>;

 protected:
  class map_impl : public refcounted
  {
  public:
    M t;
  };
  
  ptr<map_impl> impl;
  
 public:
  typedef map_wrapper_iter<M, K, V> iter;
  typedef map_wrapper_const_iter<M, K, V> const_iter;
  
 public:
  map_wrapper () : impl(new map_impl) { }
  map_wrapper (unsigned dummy_size) : impl(new map_impl) { }
  map_wrapper (const map_wrapper &m) : impl(m.impl) { }
  map_wrapper (copy, const map_wrapper &m) : impl(new map_impl) { impl->t = M (m.impl->t); }
  map_wrapper (copy2, const map_wrapper &m);
  map_wrapper (reader &r);
  ~map_wrapper () { }
  
  map_wrapper &operator = (const map_wrapper &m) { impl = m.impl; return *this; }
  
  /* returns the pair associated to the smallest key */
  pair<K, V> head () const
  {
    typename M::const_iterator i = impl->t.begin ();
    assert (i != impl->t.end ());
    return pair<K, V> (i->first, i->second);
  }
  
  /* returns the pair associated to the largest key */
  pair<K, V> tail () const
  {
    typename M::const_reverse_iterator i = impl->t.rbegin ();
    assert (i != impl->t.rend ());
    return pair<K, V> (i->first, i->second);
  }
  
  maybe<V> remove_with_value (const K &k)
  {
    typename M::iterator i = impl->t.find (k);
    if (i == impl->t.end ())
      return maybe<V> ();
    else
      {
	V v = i->second;
	impl->t.erase (i);
	return maybe<V> (v);
      }
  }
  
  void clear () { impl->t.clear (); }
  bool is_empty () const { return impl->t.empty (); }
  unsigned card () const { return impl->t.size (); }
  void yank (const K &k) { assert (operator % (k)); impl->t.erase (k); }
  V yank_with_value (const K &k)
  {
    typename M::iterator i = impl->t.find (k);
    assert (i != impl->t.end ());
    V v = i->second;
    impl->t.erase (i);
    return v;
  }
  
  void operator -= (const K &k) { impl->t.erase (k); }
  
  void push (const K &k, const V &v) { assert (!operator % (k)); impl->t.insert (std::pair<K, V> (k, v)); }
  void set (const K &k, const V &v) { operator [] (k) = v; }
  
  pair<V &, bool> find (const K &k)
  {
    typename M::iterator i = impl->t.find (k);
    bool present;
    if (i == impl->t.end ())
      {
	i = impl->t.insert (std::pair<K, V> (k, V ())).first;
	present = 0;
      }
    else
      present = 1;
    return pair<V &, bool> (i->second, present);
  }

  V *operator ^ (const K &k)
  {
    typename M::iterator i = impl->t.find (k);
    return i == impl->t.end () ? 0 : &i->second;
  }
  const V *operator ^ (const K &k) const
  {
    typename M::const_iterator i = impl->t.find (k);
    return i == impl->t.end () ? 0 : &i->second;
  }
  
  V &operator [] (const K &k) { return impl->t[k]; }
  const V &operator [] (const K &k) const
  {
    typename M::const_iterator i = impl->t.find (k);
    assert (i != impl->t.end ());
    return i->second;
  }
  const V &operator () (const K &k) const { return operator [] (k); }
  const V &operator () (const K &k, const V &d) const
  {
    typename M::const_iterator i = impl->t.find (k);
    if (i == impl->t.end ())
      return d;
    else
      return i->second;
  }
  
  bool operator % (const K &k) const
  {
    typename M::const_iterator i = impl->t.find (k);
    return i != impl->t.end ();
  }
  
  bool operator == (const map_wrapper &m) const;
  bool operator != (const map_wrapper &m) const { return !operator == (m); }
  bool operator < (const map_wrapper &m) const;
  
  void write_self (writer &w) const;
};

template<class M, class K, class V>
map_wrapper<M, K, V>::map_wrapper (copy2, const map_wrapper &m)
  : impl(new map_impl)
{
  /* Keys are immutable.  Just copy the values. */
  for (const_iter i = m; i; i ++)
    push (i.key (), V (COPY, i.val ()));
}

template<class M, class K, class V>
map_wrapper<M, K, V>::map_wrapper (reader &r)
  : impl(new map_impl)
{
  unsigned card_ = r.read_unsigned ();
  K k;
  V v;
  for (unsigned i = 0; i < card_; i ++)
    {
      read (r, k);
      read (r, v);
      push (k, v);
    }
}

template<class M, class K, class V> bool
map_wrapper<M, K, V>::operator == (const map_wrapper &m) const
{
  if (card () != m.card ())
    return 0;
  
  const_iter i = *this,
    j = m;
  while (i && j)
    {
      if (i.key () != j.key ()
	  || i.val () != j.val ())
	return 0;
      i ++;
      j ++;
    }
  if (i || j)
    return 0;
  return 1;
}

template<class M, class K, class V> bool
map_wrapper<M, K, V>::operator < (const map_wrapper &m) const
{
  const_iter i = *this,
    j = m;
  while (i && j)
    {
      if (i.key () < j.key ())
	return 1;
      else if (i.key () > j.key ())
	return 0;
      else
	{
	  assert (i.key () == j.key ());
	  if (i.val () < j.val ())
	    return 1;
	  else if (i.val () > j.val ())
	    return 0;
	  else
	    assert (i.val () == j.val ());
	}
      i ++;
      j ++;
    }
  return !i && j;
}

template<class M, class K, class V> void
map_wrapper<M, K, V>::write_self (writer &w) const
{
  write (w, card ());
  for (const_iter i = *this; i; i ++)
    {
      write (w, i.key ());
      write (w, i.val ());
    }
}

template<class M, class K, class V>
class map_wrapper_iter
{
 private:
  ptr<typename map_wrapper<M, K, V>::map_impl> impl;
  typename M::iterator i, end;
  bool deleted;
  
 public:
  map_wrapper_iter (map_wrapper<M, K, V> &m) : impl(m.impl), i(m.impl->t.begin ()), end(m.impl->t.end ()), deleted(0) { }
  ~map_wrapper_iter () { }
  
  void del ()
  {
    assert (!deleted);
    typename M::iterator iprev = i ++;
    impl->t.erase (iprev);
    deleted = 1;
  }
  
  const K &key () const { assert (!deleted); return i->first; }
  V &val () { assert (!deleted); return i->second; }
  operator bool () const { assert (!deleted); return i != end; }
  void operator ++ () { if (deleted) deleted = 0; else i ++; }
  void operator ++ (int) { operator ++ (); }
};

template<class M, class K, class V>
class map_wrapper_const_iter
{
 private:
  ptr<const typename map_wrapper<M, K, V>::map_impl> impl;
  typename M::const_iterator i, end;
  
 public:
  map_wrapper_const_iter (const map_wrapper<M, K, V> &m) : impl(m.impl), i(m.impl->t.begin ()), end(m.impl->t.end ()) { }
  ~map_wrapper_const_iter () { }
  
  const K &key () const { return i->first; }
  const V &val () { return i->second; }
  operator bool () const { return i != end; }
  void operator ++ () { i ++; }
  void operator ++ (int) { i ++; }
};
