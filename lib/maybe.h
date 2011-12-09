
template <class T>
class maybe
{
 private:
  bool present;
  T v;
  
 public:
  maybe () : present(0) { }
  maybe (const T &v_) : present(1), v(v_) { }
  maybe (const maybe &m) : present(m.present), v(m.v) { }
  maybe (reader &r);
  ~maybe () { }
  
  maybe &operator = (const maybe &m) { present = m.present; v = m.v; return *this; }
  
  bool is_some () const { return present; }
  bool is_none () const { return !present; }
  T &some () { assert (present); return v; }
  
  bool operator == (const maybe &m) const
  {
    if (present != m.present)
      return 0;
    if (present)
      return v == m.v;
  }
  bool operator != (const maybe &m) const { return !operator == (m); }
  
  void write_self (writer &w) const;
  hash_t hash_self () const;
};

template<class T>
maybe<T>::maybe (reader &r)
{
  read (r, present);
  if (present)
    read (r, v);
}

template<class T> void
maybe<T>::write_self (writer &w) const
{
  write (w, present);
  if (present)
    write (w, v);
}

template<class T> hash_t
maybe<T>::hash_self () const
{
  if (present)
    return hash_combine (hash (0xdeadbeef), hash (v));
  else
    return hash (0xcafebabe);
}
