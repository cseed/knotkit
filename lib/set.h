
/* wrap for std::set */

template<class T> class set_iter;
template<class T> class set_const_iter;

template<class T>
class set
{
 private:
  friend class set_iter<T>;
  friend class set_const_iter<T>;
  
  class set_impl : public refcounted
  {
  public:
    std::set<T> t;
  };
  
  ptr<set_impl> impl;
  
 public:
  typedef set_iter<T> iter;
  typedef set_const_iter<T> const_iter;
  
 public:
  set () : impl(new set_impl) { }
  set (unsigned dummy_size) : impl(new set_impl) { }
  set (const set &s) : impl(s.impl) { }
  set (copy, const set &s) : impl(new set_impl) { impl->t = std::set<T> (s.impl->t); }
  set (reader &r);
  ~set () { }
  
  set &operator = (const set &s) { impl = s.impl; return *this; }
  
  T pop ()
  {
    typename std::set<T>::const_iterator i = impl->t.begin ();
    assert (i != impl->t.end ());
    T tmp = *i;
    impl->t.erase (i);
    return tmp;
  }
  
  bool is_empty () const { return impl->t.empty (); }
  unsigned card () const { return impl->t.size (); }
  const T &head () const;
  const T &tail () const;
  
  bool operator == (const set &s) const;
  bool operator < (const set &s) const;
  bool operator <= (const set &s) const;
  
  void clear () { impl->t.clear (); }
  void push (const T &v) { assert (impl->t.find (v) == impl->t.end ()); impl->t.insert (v); }
  bool toggle (const T &v);
  void operator += (const T &v) { impl->t.insert (v); }
  void operator -= (const T &v) { impl->t.erase (v); }
  void yank (const T &v) { assert (operator % (v)); impl->t.erase (v); }
  
  set &operator |= (const set<T> &s);
  set &operator &= (const set<T> &s);
  set &operator ^= (const set<T> &s);
  
  bool operator % (const T &v) const { return impl->t.find (v) != impl->t.end (); }
  bool operator () (const T &v) const { return operator % (v); }
  
  void write_self (writer &w) const;
  hash_t hash_self () const;
};

template<class T>
class set_iter
{
  set<T> &s;
  typename std::set<T>::const_iterator i, end;
  bool deleted;
  
 public:
  set_iter (set<T> &s_) : s(s_), i(s_.impl->t.begin ()), end(s_.impl->t.end ()), deleted(0) { }
  ~set_iter () { }
  
  void del ()
  {
    assert (!deleted);
    typename std::set<T>::const_iterator iprev = i ++;
    s.impl->t.erase (iprev);
    deleted = 1;
  }
  
  const T &val () const { assert (!deleted); return *i; }
  operator bool () const { assert (!deleted); return i != end; }
  set_iter &operator ++ () { if (deleted) deleted = 0; else i ++; return *this; }
  void operator ++ (int) { operator ++ (); }
};

template<class T>
class set_const_iter
{
  typename std::set<T>::const_iterator i, end;
  
 public:
  set_const_iter (const set<T> &s) : i(s.impl->t.begin ()), end(s.impl->t.end ()) { }
  ~set_const_iter () { }
  
  const T &val () const { return *i; }
  operator bool () const { return i != end; }
  set_const_iter &operator ++ () { i ++; return *this; }
  void operator ++ (int) { i ++; }
};

template<class T>
set<T>::set (reader &r)
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

template<class T> const T &
set<T>::head () const
{
  set_const_iter<T> i = *this;
  assert (i);
  return i.val ();
}

template<class T> const T &
set<T>::tail () const
{
  typename std::set<T>::const_reverse_iterator i = impl->t.crbegin ();
  assert (i != impl->t.crend ());
  return *i;
}  

template<class T> bool
set<T>::operator == (const set &s) const
{
  typename std::set<T>::const_iterator i = impl->t.begin (),
    j = s.impl->t.begin (),
    iend = impl->t.end (),
    jend = s.impl->t.end ();
  
  while (i != iend && j != jend && *i == *j)
    i ++, j ++;
  
  return i == iend && j == jend;
}

template<class T> bool
set<T>::operator < (const set &s) const
{
  typename std::set<T>::const_iterator i = impl->t.begin (),
    j = s.impl->t.begin (),
    iend = impl->t.end (),
    jend = s.impl->t.end ();
  
  while (i != iend && j != jend && *i == *j)
    i ++, j ++;
  
  return (i == iend && j != jend) || (i != iend && j != jend && *i < *j);
}

template<class T> bool
set<T>::operator <= (const set &s) const
{
  typename std::set<T>::const_iterator i = impl->t.begin (),
    j = s.impl->t.begin (),
    iend = impl->t.end (),
    jend = s.impl->t.end ();
  
  while (i != iend && j != jend && *i == *j)
    i ++, j ++;
  
  return i == iend
    || (i != iend && j != jend && *i <= *j);
}

template<class T> bool
set<T>::toggle (const T &v)
{
  typename std::set<T>::const_iterator i = impl->t.lower_bound (v),
    end = impl->t.end ();
  if (i != end && *i == v)
    {
      impl->t.erase (i);
      return 1;
    }
  else
    {
      if (i == impl->t.begin ())
	impl->t.insert (v);
      else
	{
	  typename std::set<T>::const_iterator j = --i;
	  assert (*j < v);
	  impl->t.insert (j, v);
	}
      return 0;
    }
}

template<class T> set<T> &
set<T>::operator |= (const set<T> &s)
{
  std::set<T> news;
  std::set_union (impl->t.begin (), impl->t.end (),
		  s.impl->t.begin (), s.impl->t.end (),
		  inserter (news, news.begin ()));
  impl->t = news;
  return *this;
}

template<class T> set<T> &
set<T>::operator &= (const set<T> &s)
{
  std::set<T> news;
  std::set_intersection (impl->t.begin (), impl->t.end (),
			 s.impl->t.begin (), s.impl->t.end (),
			 inserter (news, news.begin ()));
  impl->t = news;
  return *this;
}

template<class T> set<T> &
set<T>::operator ^= (const set<T> &s)
{
#if 0
  printf ("before:\n");
  printf ("this:");
  for (typename std::set<T>::const_iterator i = impl->t.begin (); i != impl->t.end (); i ++)
    printf (" %d", *i);
  printf ("\n");
  printf ("s:");
  for (typename std::set<T>::const_iterator i = s.impl->t.begin (); i != s.impl->t.end (); i ++)
    printf (" %d", *i);
  printf ("\n");
#endif
  
#if 1
  typename std::set<T>::const_iterator i = impl->t.begin (),
    iend = impl->t.end (),
    j = s.impl->t.begin (),
    jend = s.impl->t.end ();
  
  while (i != iend && j != jend && *i == *j)
    {
      impl->t.erase (i);
      i = impl->t.begin ();
      j ++;
    }
  
  if (i == iend)
    impl->t.insert (j, jend);
  else
    {
      if (j != jend && *j < *i)
	{
	  i = impl->t.insert (*j).first;
	  j ++;
	}
      
      while (j != jend)
	{
	  assert (*i < *j);
	  
	  typename std::set<T>::const_iterator iprev = i ++;
	  if (i == iend)
	    {
	      while (j != jend)
		{
		  iprev = impl->t.insert (iprev, *j);
		  j ++;
		}
	      break;
	    }
	  else if (*i == *j)
	    {
	      impl->t.erase (i);
	      i = iprev;
	      j ++;
	    }
	  else
	    {
	      while (j != jend && *j < *i)
		{
		  iprev = impl->t.insert (iprev, *j);
		  j ++;
		}
	      if (j != jend && *j == *i)
		{
		  impl->t.erase (i);
		  i = iprev;
		  j ++;
		}
	    }
	}
    }
#endif
  
#if 0
  std::set<T> news;
  std::set_symmetric_difference (impl->t.begin (), impl->t.end (),
				 s.impl->t.begin (), s.impl->t.end (),
				 inserter (news, news.begin ()));
  impl->t = news;
#endif
  
#if 0
  printf ("after:\n");
  printf ("this:");
  for (typename std::set<T>::const_iterator i = impl->t.begin (); i != impl->t.end (); i ++)
    printf (" %d", *i);
  printf ("\n");
#endif
  
  return *this;
}

template<class T> void
set<T>::write_self (writer &w) const
{
  write (w, card ());
  for (const_iter i = *this; i; i ++)
    write (w, i.val ());
}

template<class T> hash_t
set<T>::hash_self () const
{
  hash_t h = hash (card ());
  unsigned j = 0;
  for (const_iter i = *this; i && j < 10; i ++, j ++)
    h = hash_combine (h, hash (i.val ()));
  return h;
}
