
/* wrapper for std::set */

template<class T>
class set : public set_wrapper<std::set<T>, T>
{
  typedef set_wrapper<std::set<T>, T> base;
  
 public:
  set () { }
  set (unsigned dummy_size) : base(dummy_size) { }
  set (const set &m) : base(m) { }
  set (copy, const set &m) : base(COPY, m) { }
  set (reader &r) : base(r) { }
  ~set () { }
  
  set &operator = (const set &m) { base::operator = (m); return *this; }
  
  bool operator == (const set &s) const;
  bool operator != (const set &s) const { return !operator == (s); }
  bool operator < (const set &s) const;
  bool operator <= (const set &s) const;
  
  bool toggle (const T &v);
  
  set &operator |= (const set &s);
  set &operator &= (const set &s);
  set &operator ^= (const set &s);
};

template<class T>
  using set_iter = set_wrapper_iter<std::set<T>, T>;

template<class T>
  using set_const_iter = set_wrapper_const_iter<std::set<T>, T>;

template<class T> bool
set<T>::operator == (const set &s) const
{
  typename std::set<T>::const_iterator i = this->impl->t.begin (),
    j = s.impl->t.begin (),
    iend = this->impl->t.end (),
    jend = s.impl->t.end ();
  
  while (i != iend && j != jend && *i == *j)
    i ++, j ++;
  
  return i == iend && j == jend;
}

template<class T> bool
set<T>::operator < (const set &s) const
{
  typename std::set<T>::const_iterator i = this->impl->t.begin (),
    j = s.impl->t.begin (),
    iend = this->impl->t.end (),
    jend = s.impl->t.end ();
  
  while (i != iend && j != jend && *i == *j)
    i ++, j ++;
  
  return (i == iend && j != jend) || (i != iend && j != jend && *i < *j);
}

template<class T> bool
set<T>::operator <= (const set &s) const
{
  typename std::set<T>::const_iterator i = this->impl->t.begin (),
    j = s.impl->t.begin (),
    iend = this->impl->t.end (),
    jend = s.impl->t.end ();
  
  while (i != iend && j != jend && *i == *j)
    i ++, j ++;
  
  return i == iend
    || (i != iend && j != jend && *i <= *j);
}

template<class T> bool
set<T>::toggle (const T &v)
{
  typename std::set<T>::const_iterator i = this->impl->t.lower_bound (v),
    end = this->impl->t.end ();
  if (i != end && *i == v)
    {
      this->impl->t.erase (i);
      return 1;
    }
  else
    {
      if (i == this->impl->t.begin ())
	this->impl->t.insert (v);
      else
	{
	  typename std::set<T>::const_iterator j = --i;
	  assert (*j < v);
	  this->impl->t.insert (j, v);
	}
      return 0;
    }
}

template<class T> set<T> &
set<T>::operator |= (const set &s)
{
  std::set<T> news;
  std::set_union (this->impl->t.begin (), this->impl->t.end (),
		  s.impl->t.begin (), s.impl->t.end (),
		  inserter (news, news.begin ()));
  this->impl->t = news;
  return *this;
}

template<class T> set<T> &
set<T>::operator &= (const set &s)
{
  std::set<T> news;
  std::set_intersection (this->impl->t.begin (), this->impl->t.end (),
			 s.impl->t.begin (), s.impl->t.end (),
			 inserter (news, news.begin ()));
  this->impl->t = news;
  return *this;
}

template<class T> set<T> &
set<T>::operator ^= (const set &s)
{
#if 0
  printf ("before:\n");
  printf ("this:");
  for (typename std::set<T>::const_iterator i = this->impl->t.begin (); i != this->impl->t.end (); i ++)
    printf (" %d", *i);
  printf ("\n");
  printf ("s:");
  for (typename std::set<T>::const_iterator i = s.impl->t.begin (); i != s.impl->t.end (); i ++)
    printf (" %d", *i);
  printf ("\n");
#endif
  
#if 1
  typename std::set<T>::const_iterator i = this->impl->t.begin (),
    iend = this->impl->t.end (),
    j = s.impl->t.begin (),
    jend = s.impl->t.end ();
  
  while (i != iend && j != jend && *i == *j)
    {
      this->impl->t.erase (i);
      i = this->impl->t.begin ();
      j ++;
    }
  
  if (i == iend)
    this->impl->t.insert (j, jend);
  else
    {
      if (j != jend && *j < *i)
	{
	  i = this->impl->t.insert (*j).first;
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
		  iprev = this->impl->t.insert (iprev, *j);
		  j ++;
		}
	      break;
	    }
	  else if (*i == *j)
	    {
	      this->impl->t.erase (i);
	      i = iprev;
	      j ++;
	    }
	  else
	    {
	      while (j != jend && *j < *i)
		{
		  iprev = this->impl->t.insert (iprev, *j);
		  j ++;
		}
	      if (j != jend && *j == *i)
		{
		  this->impl->t.erase (i);
		  i = iprev;
		  j ++;
		}
	    }
	}
    }
#endif
  
#if 0
  std::set<T> news;
  std::set_symmetric_difference (this->impl->t.begin (), this->impl->t.end (),
				 s.impl->t.begin (), s.impl->t.end (),
				 inserter (news, news.begin ()));
  this->impl->t = news;
#endif
  
#if 0
  printf ("after:\n");
  printf ("this:");
  for (typename std::set<T>::const_iterator i = this->impl->t.begin (); i != this->impl->t.end (); i ++)
    printf (" %d", *i);
  printf ("\n");
#endif
  
  return *this;
}
