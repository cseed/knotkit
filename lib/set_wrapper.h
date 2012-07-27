
/* wrapper for STL maps */

template<class S, class T> class set_wrapper_iter;
template<class S, class T> class set_wrapper_const_iter;

template<class S, class T>
class set_wrapper
{
  friend class set_wrapper_iter<S, T>;
  friend class set_wrapper_const_iter<S, T>;
  
 protected:
  class set_wrapper_impl : public refcounted
  {
  public:
    S t;
  };
  
  ptr<set_wrapper_impl> impl;
  
 public:
  typedef set_wrapper_iter<S, T> iter;
  typedef set_wrapper_const_iter<S, T> const_iter;
  
 public:
  set_wrapper () : impl(new set_wrapper_impl) { }
  set_wrapper (unsigned dummy_size) : impl(new set_wrapper_impl) { }
  set_wrapper (const set_wrapper &s) : impl(s.impl) { }
  set_wrapper (copy, const set_wrapper &s) : impl(new set_wrapper_impl) { impl->t = S (s.impl->t); }
  set_wrapper (reader &r);
  ~set_wrapper () { }
  
  set_wrapper &operator = (const set_wrapper &s) { impl = s.impl; return *this; }
  
  T pop ()
  {
    typename S::const_iterator i = impl->t.begin ();
    assert (i != impl->t.end ());
    T tmp = *i;
    impl->t.erase (i);
    return tmp;
  }
  
  bool is_empty () const { return impl->t.empty (); }
  unsigned card () const { return impl->t.size (); }
  const T &head () const;
  const T &tail () const;
  
  void clear () { impl->t.clear (); }
  void push (const T &v) { assert (impl->t.find (v) == impl->t.end ()); impl->t.insert (v); }
  void operator += (const T &v) { impl->t.insert (v); }
  void operator -= (const T &v) { impl->t.erase (v); }
  void yank (const T &v) { assert (operator % (v)); impl->t.erase (v); }
  
  bool operator % (const T &v) const { return impl->t.find (v) != impl->t.end (); }
  bool operator () (const T &v) const { return operator % (v); }
  
  void write_self (writer &w) const;
  hash_t hash_self () const;
};

template<class S, class T>
class set_wrapper_iter
{
  ptr<typename set_wrapper<S, T>::set_wrapper_impl> impl;
  typename S::const_iterator i, end;
  bool deleted;
  
 public:
  set_wrapper_iter (set_wrapper<S, T> &s) : impl(s.impl), i(s.impl->t.begin ()), end(s.impl->t.end ()), deleted(0) { }
  ~set_wrapper_iter () { }
  
  void del ()
  {
    assert (!deleted);
    typename S::const_iterator iprev = i ++;
    impl->t.erase (iprev);
    deleted = 1;
  }
  
  const T &val () const { assert (!deleted); return *i; }
  operator bool () const { assert (!deleted); return i != end; }
  set_wrapper_iter &operator ++ () { if (deleted) deleted = 0; else i ++; return *this; }
  void operator ++ (int) { operator ++ (); }
};

template<class S, class T>
class set_wrapper_const_iter
{
  ptr<const typename set_wrapper<S, T>::set_wrapper_impl> impl;
  typename S::const_iterator i, end;
  
 public:
  set_wrapper_const_iter (const set_wrapper<S, T> &s) : impl(s.impl), i(s.impl->t.begin ()), end(s.impl->t.end ()) { }
  ~set_wrapper_const_iter () { }
  
  const T &val () const { return *i; }
  operator bool () const { return i != end; }
  set_wrapper_const_iter &operator ++ () { i ++; return *this; }
  void operator ++ (int) { i ++; }
};

template<class S, class T>
set_wrapper<S, T>::set_wrapper (reader &r)
{
  unsigned n;
  read (r, n);
  T x;
  for (unsigned i = 0; i < n; i ++)
    {
      read (r, x);
      push (x);
    }
}

template<class S, class T> const T &
set_wrapper<S, T>::head () const
{
  set_wrapper_const_iter<S, T> i = *this;
  assert (i);
  return i.val ();
}

template<class S, class T> const T &
set_wrapper<S, T>::tail () const
{
  typename S::const_reverse_iterator i = impl->t.rbegin ();
  assert (i != impl->t.rend ());
  return *i;
}  

template<class S, class T> void
set_wrapper<S, T>::write_self (writer &w) const
{
  write (w, card ());
  for (const_iter i = *this; i; i ++)
    write (w, i.val ());
}

template<class S, class T> hash_t
set_wrapper<S, T>::hash_self () const
{
  hash_t h = hash (card ());
  unsigned j = 0;
  for (const_iter i = *this; i && j < 10; i ++, j ++)
    h = hash_combine (h, hash (i.val ()));
  return h;
}
