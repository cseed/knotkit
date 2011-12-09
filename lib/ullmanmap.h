
template<class V, unsigned B> class ullmanmap_iter;
template<class V, unsigned B> class ullmanmap_const_iter;

template<class V, unsigned B>
class ullmanmap
{
 private:
  friend class ullmanmap_iter<V, B>;
  friend class ullmanmap_const_iter<V, B>;
  
  ullmanset<B> present;
  basedvector<V, B> values;
  
 public:
  typedef ullmanmap_iter<V, B> iter;
  typedef ullmanmap_const_iter<V, B> const_iter;
  
 public:
  ullmanmap () { }
  ullmanmap (unsigned size) : present(size), values(size) { }
  ullmanmap (unsigned size, const map<unsigned, V> &m); // common
  ullmanmap (const ullmanmap &m) : present(m.present), values(m.values) { }
  ullmanmap (copy, const ullmanmap &m) : present(COPY, m.present), values(COPY, m.values) { }
  ullmanmap (reader &r);
  ~ullmanmap () { }
  
  ullmanmap &operator = (const ullmanmap &m) { present = m.present; values = m.values; return *this; }
  ullmanmap &operator = (const map<unsigned, V> &m); // common
  
  unsigned size () const { return present.size (); }
  
  pair<unsigned, V> head ();
  void clear () { present.clear (); }
  unsigned card () { return present.card (); }
  void operator -= (unsigned k) { present -= k; }
  
  void push (unsigned k, const V &v) { present.push (k); values[k] = v; }
  void set (unsigned k, const V &v) { present += k; values[k] = v; }
  
  V &operator [] (unsigned k) { if (! (present % k)) { present.push (k); values[k] = V (); } return values[k]; }
  const V &operator [] (unsigned k) const { assert (present % k); return values[k]; }
  const V &operator () (unsigned k) const { assert (present % k); return operator [] (k); }
  const V &operator () (unsigned k, const V &d) const { if (present % k) return values[k]; else return d; }
  
  pair<V &, bool> find (unsigned k)
  {
    bool p = present % k;
    return pair<V &, bool> (operator [] (k), p);
  }
  
  bool operator % (unsigned k) const { return present % k; }

  unsigned nth (unsigned p) const { return present.nth (p); }
  unsigned position (unsigned k) const { return present.position (k); }
  
  void write_self (writer &w) const;
};

template<class V, unsigned B>
ullmanmap<V, B>::ullmanmap (unsigned size, const map<unsigned, V> &m)
  : present(size), values(size)
{
  for (typename map<unsigned, V>::const_iter i = m; i; i ++)
    push (i.key (), i.val ());
}

template<class V, unsigned B>
ullmanmap<V, B>::ullmanmap (reader &r)
  : present(r),
    values(r)
{
}

template<class V, unsigned B> ullmanmap<V, B> &
ullmanmap<V, B>::operator = (const map<unsigned, V> &m)
{
  /* assume the ullman map is already allocated */
  clear ();
  for (typename map<unsigned, V>::const_iter i = m; i; i ++)
    push (i.key (), i.val ());
  return *this;
}

template<class V, unsigned B> void
ullmanmap<V, B>::write_self (writer &w) const
{
  write (w, present);
  write (w, values);
}

template<class V, unsigned B>
class ullmanmap_iter
{
 private:
  ullmanset_iter<B> i;
  basedvector<V, B> &values;
  
 public:
  ullmanmap_iter (ullmanmap<V, B> &m) : i(m.present), values(m.values) { }
  ~ullmanmap_iter () { }
  
  void del () { i.del (); }
  unsigned key () const { return i.val (); }
  V &val () { return values[i.val ()]; }
  const V &val () const { return values[i.val ()]; }
  operator bool () const { return (bool)i; }
  void operator ++ () { i ++; }
  void operator ++ (int) { i ++; }
};

template<class V, unsigned B>
class ullmanmap_const_iter
{
 private:
  ullmanset_const_iter<B> i;
  const basedvector<V, B> &values;
  
 public:
  ullmanmap_const_iter (const ullmanmap<V, B> &m) : i(m.present), values(m.values) { }
  ~ullmanmap_const_iter () { }
  
  unsigned key () const { return i.val (); }
  const V &val () const { return values[i.val ()]; }
  operator bool () const { return (bool)i; }
  void operator ++ () { i ++; }
  void operator ++ (int) { i ++; }
};
