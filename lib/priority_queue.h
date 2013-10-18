
template<class V>
class priority_queue
{
 private:
  class weighted
  {
  public:
    V v;
    int pri;
    
  public:
    weighted () { }
    weighted (const weighted &w) : v(w.v), pri(w.pri) { }
    weighted (const V &v_, int pri_) : v(v_), pri(pri_) { }
    ~weighted () { }
    
    weighted &operator = (const weighted &w) { v = w.v; pri = w.pri; }
    
    bool operator == (const weighted &w) const { return v == w.v && pri == w.pri; }
    bool operator != (const weighted &w) const { return !operator == (w); }
    
    bool operator < (const weighted &w) const { return pri > w.pri || (pri == w.pri && v < w.v); }
  };
  
  map<V, int> pri;
  set<weighted> s;
  
 public:
  priority_queue () { }
  priority_queue (const priority_queue &q) : pri(q.pri), s(q.s) { }
  priority_queue (copy, const priority_queue &q) : pri(COPY, q.pri), s(COPY, q.s) { }
  ~priority_queue () { }
  
  priority_queue &operator = (const priority_queue &q) { pri = q.pri; s = q.s; }
  
  bool is_empty () const { return s.is_empty (); }
  bool operator % (const V &v) const { return pri % v; }
  void push (const V &v, int p)
  {
    assert (! (pri % v));
    pri.push (v, p);
    s.push (weighted (v, p));
  }
  priority_queue &operator -= (const V &v)
  {
    maybe<int> old_p = pri.remove_with_value (v);
    if (old_p.is_some ())
      s.yank (weighted (v, old_p.some ()));
    return *this;
  }
  void yank (const V &v) { assert (pri % v); operator -= (v); }
  void insert (const V &v, int p)
  {
    int *old_p = pri ^ v;
    if (old_p)
      {
	if (*old_p != p)
	  {
	    s.yank (weighted (v, *old_p));
	    s.push (weighted (v, p));
	    *old_p = p;
	  }
      }
    else
      {
	pri.push (v, p);
	s.push (weighted (v, p));
      }
  }
  V pop ()
  {
    weighted w = s.pop ();
    pri.yank (w.v);
    return w.v;
  }
  const V &top () const { return s.head ().v; }
  
  unsigned card () const { return s.card (); }
};
