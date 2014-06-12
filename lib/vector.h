
template<class T>
class vector
{
protected:
  class data
  {
  public:
    unsigned refcount;
    unsigned n;
    unsigned c;
    T p[1];
  };
  
  data *d;
  
  static data *alloc (unsigned n_, unsigned c_, unsigned initn, T *init);
  
  void unref ();
  void ref (data *newd) { assert (d == 0); d = newd; if (d) d->refcount ++; }
  
  T &at (unsigned i) { return operator [] (i); }
  const T &at (unsigned i) const { return operator [] (i); }
  
public:
  vector () : d(0) { }
  explicit vector (unsigned n_);
  vector (const vector &v) : d(0) { ref (v.d); }
  vector (const vector &v, const vector &u);
  vector (copy, const vector &v);
  vector (copy2, const vector &v);
  vector (unsigned n_, T *p_);
  vector (unsigned n_, const vector &v_);
  explicit vector (reader &r);
  ~vector () { unref (); }
  
  vector &operator = (const vector &v) { unref (); ref (v.d); return *this; }
  
  bool operator < (const vector &v) const;
  bool operator == (const vector &v) const;
  bool operator != (const vector &v) const { return !operator == (v); }
  
  bool operator % (unsigned i) { return d && i < d->n; }
  
  T &operator [] (unsigned i) { assert (d); assert (i < d->n); return d->p[i]; }
  const T &operator [] (unsigned i) const { assert (d); assert (i < d->n); return d->p[i]; }
  
  const T &operator () (unsigned i) const { return operator [] (i); }
  
  /* ??? break resize into grow_capacity/resize, and specialize this
     to in-place ctor from v */
  void append (const T &v)
  {
    if (!d)
      reserve (4);
    else if (d->n == d->c)
      reserve (d->c * 2);
    
    assert (d && d->n < d->c);
    
    new (&d->p[d->n ++]) T (v);
  }
  
  const T &top () const
  {
    unsigned n = size ();
    assert (n > 0);
    return at (n - 1);
  }
  
  T pop ();
  
  unsigned capacity () const { return d ? d->c : 0; }
  unsigned size () const { return d ? d->n : 0; }
  void resize (unsigned new_size);
  void reserve (unsigned c);
  
  void sort () { assert (d); sort (0, d->n); }
  void sort (unsigned from, unsigned len);
  template<typename C> void sort (unsigned from, unsigned len, C comp);
  
  unsigned lower_bound (const T &v) const;
  unsigned upper_bound (const T &v) const;
  
  void write_self (writer &w) const;
  hash_t hash_self () const;
};

template<class T> typename vector<T>::data *
vector<T>::alloc (unsigned n_, unsigned c_, unsigned initn, T *init)
{
  if (c_ > 0)
    {
      data *d = (data *)new char[sizeof (data) + sizeof (T) * (c_ - 1)];
      
      d->refcount = 1;
      d->c = c_;
      d->n = n_;
      
      if (initn > n_)
	initn = n_;
      
      for (unsigned i = 0; i < initn; i ++)
	new (&d->p[i]) T (init[i]);
      for (unsigned i = initn; i < n_; i ++)
	new (&d->p[i]) T ();
      
      return d;
    }
  else
    return 0;
}

template<class T> void
vector<T>::unref ()
{
  if (d && -- d->refcount == 0)
    {
      for (unsigned i = 0; i < d->n; i ++)
	d->p[i].~T ();
      delete [] (char *)d;
    }
  d = 0;
}

template<class T>
vector<T>::vector (const vector &v, const vector &u)
  : d(0)
{
  if (v.d)
    {
      unsigned vn = v.d->n,
	un = u.size ();
      d = alloc (vn, vn + un, v.d->n, v.d->p);
      
      d->n = vn + un;
      for (unsigned i = 0; i < un; i ++)
	new (&d->p[vn + i]) T (u[i]);
    }
  else
    {
      if (u.d)
	d = alloc (u.d->n, u.d->n, u.d->n, u.d->p);
    }
}

template<class T>
vector<T>::vector (copy, const vector &v)
  : d(0)
{
  if (v.d)
    d = alloc (v.d->n, v.d->n, v.d->n, v.d->p);
}

template<class T>
vector<T>::vector (copy2, const vector &v)
  : d(0)
{
  if (v.d && v.d->n > 0)
    {
      unsigned n = v.d->n;
      d = (data *)new char[sizeof (data) + sizeof (T) * (n - 1)];
      
      d->refcount = 1;
      d->c = n;
      d->n = n;
      
      for (unsigned i = 0; i < n; i ++)
	new (&d->p[i]) T (COPY1, v.d->p[i]);
    }
}

template<class T>
vector<T>::vector (unsigned n_)
  : d(0)
{
  d = alloc (n_, n_, 0, 0);
}

template<class T>
vector<T>::vector (reader &r)
  : d(0)
{
  unsigned n_;
  read (r, n_);
  if (n_ > 0)
    {
      d = (data *)new char[sizeof (data) + sizeof (T) * (n_ - 1)];
      d->refcount = 1;
      d->c = n_;
      d->n = n_;
      
      for (unsigned i = 0; i < n_; i ++)
	ctor_read (r, &d->p[i]);
    }
}

template<class T>
vector<T>::vector (unsigned n_, T *p_)
  : d(0)
{
  d = alloc (n_, n_, n_, p_);
}

template<class T>
vector<T>::vector (unsigned n_, const vector &v)
  : d(0)
{
  if (v.d)
    d = alloc (n_, n_, v.d->n, v.d->p);
  else
    d = alloc (n_, n_, 0, 0);
}

template<class T> bool
vector<T>::operator < (const vector &v) const
{
  unsigned n = size (),
    vn =  v.size ();
  if (n < vn)
    return 1;
  else if (n > vn)
    return 0;
  
  for (unsigned i = 0; i < n; i ++)
    {
      if (at (i) < v[i])
	return 1;
      else if (at (i) > v[i])
	return 0;
    }
  
  return 0;
}

template<class T> bool
vector<T>::operator == (const vector &v) const
{
  unsigned n = size ();
  if (n != v.size ())
    return 0;
  for (unsigned i = 0; i < n; i ++)
    {
      if (at (i) != v[i])
	return 0;
    }
  
  return 1;
}

template<class T> T
vector<T>::pop ()
{
  unsigned n = size ();
  assert (n > 0);
  T tmp = at (n - 1);
  resize (n - 1);
  return tmp;
}

template<class T> void 
vector<T>::reserve (unsigned newc)
{
  if (newc <= capacity ())
    return;
  
  data *newd;
  if (d)
    newd = alloc (d->n, newc, d->n, d->p);
  else
    newd = alloc (0, newc, 0, 0);
  unref ();
  d = newd;
}
 
template<class T> void 
vector<T>::resize (unsigned new_size)
{
  unsigned n = size ();
  if (new_size == n)
    return;
  
  reserve (new_size);
  
  assert (d);
  assert (new_size <= capacity ());
  
  if (new_size < n)
    {
      for (unsigned i = new_size; i < n; i ++)
	d->p[i].~T ();
    }
  else
    {
      for (unsigned i = n; i < new_size; i ++)
	new (&d->p[i]) T ();
    }
  d->n = new_size;
}

template<class T> void
vector<T>::sort (unsigned from, unsigned len)
{
  assert (d);
  assert (from < d->n);
  assert (from + len <= d->n);
  std::sort (&d->p[from], &d->p[from + len]);
}

template<class T> template<typename C> void
vector<T>::sort (unsigned from, unsigned len, C comp)
{
  assert (d);
  assert (from < d->n);
  assert (from + len <= d->n);
  std::sort (&d->p[from], &d->p[from + len], comp);
}

template<class T> void
vector<T>::write_self (writer &w) const
{
  if (d)
    {
      write (w, d->n);
      for (unsigned i = 0; i < d->n; i ++)
	write (w, d->p[i]);
    }
  else
    {
      unsigned n = 0;
      write (w, n);
    }
}

template<class T> hash_t
vector<T>::hash_self () const
{
  unsigned n = size ();
  hash_t h = hash (n);
  for (unsigned i = 0; i < n; i ++)
    h = hash_combine (h, hash (d->p[i]));
  return h;
}

template<class T> unsigned
vector<T>::lower_bound (const T &v) const
{
  T *r = std::lower_bound (&d->p[0], &d->p[d->n], v);
  return r - &d->p[0];
}

template<class T> unsigned
vector<T>::upper_bound (const T &v) const
{
  T *r = std::upper_bound (&d->p[0], &d->p[d->n], v);
  return r - &d->p[0];
}

template<class T, unsigned B> 
class basedvector : public vector<T>
{
public:
  basedvector () { }
  explicit basedvector (unsigned n_) : vector<T>(n_) { }
  explicit basedvector (vector<T> &v_) : vector<T>(v_) { }
  basedvector (unsigned n_, T *p_) : vector<T>(n_, p_) { }
  basedvector (unsigned n_, const vector<T> &v_) : vector<T>(n_, v_) { }
  basedvector (copy, const vector<T> &v_) : vector<T>(COPY, v_) { }
  basedvector (copy2, const vector<T> &v_) : vector<T>(COPY2, v_) { }
  basedvector (const vector<T> &v_, const vector<T> &u_) : vector<T>(v_, u_) { }
  explicit basedvector (reader &r) : vector<T>(r) { }
  ~basedvector () { }
  
  vector<T> &operator = (const vector<T> &v) { vector<T>::operator = (v); }
  
  bool operator % (unsigned i) { return vector<T>::operator % (i - B); }
  T &operator [] (unsigned i) { return vector<T>::operator [] (i - B); }
  const T &operator [] (unsigned i) const { return vector<T>::operator [] (i - B); }
  const T &operator () (unsigned i) const { return vector<T>::operator () (i - B); }
  
  void sort () { vector<T>::sort (); }
  void sort (unsigned from, unsigned len) { vector<T>::sort (from - B, len); }
  template<typename C> void sort (unsigned from, unsigned len, C comp) { vector<T>::sort (from - B, len, comp); }
  
  unsigned lower_bound (const T &v) const { return vector<T>::lower_bound (v) + B; }
  unsigned upper_bound (const T &v) const { return vector<T>::upper_bound (v) + B; }
};
