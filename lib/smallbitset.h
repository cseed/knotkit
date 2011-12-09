
class smallbitset_iter;
class smallbitset_const_iter;

class smallbitset
{
 private:
  friend class smallbitset_iter;
  friend class smallbitset_const_iter;
  
  unsigned n : 6;
  uint64 x : 58;
  
 public:
  static const unsigned max_size  = 58;
  
  typedef smallbitset_iter iter;
  typedef smallbitset_const_iter const_iter;
  
 public:
  smallbitset () : n(0), x(0) { }
  explicit smallbitset (unsigned n_) : n(n_), x(0) { assert (n_ <= max_size); }
  explicit smallbitset (reader &r);
  smallbitset (unsigned n_, uint64 x_)
    : n(n_), x(x_ & uint64_fill (n_))
  {
    assert (n <= max_size);
    assert ((x >> n) == 0);
  }
  
  smallbitset (const smallbitset &bs) : n(bs.n), x(bs.x) { }
  explicit smallbitset (const ullmanset<1> &t);
  // explicit smallbitset (const unsignedset &t);
  explicit inline smallbitset (const bitset &t);
  
  ~smallbitset () { }
  
  smallbitset &operator = (const smallbitset &bs) { n = bs.n; x = bs.x; return *this; }
  smallbitset &operator = (const bitset &t) { *this = smallbitset (t); return *this; }
  // smallbitset &operator = (const unsignedset &t) { *this = smallbitset (t); return *this; }
  smallbitset &operator = (const ullmanset<1> &t) { *this = smallbitset (t); return *this; }
  
  bool is_empty () const { assert ((x >> n) == 0); return x == 0; }
  unsigned card () const { assert ((x >> n) == 0); return uint64_bitcount (x); }
  unsigned size () const { return n; }
  unsigned head () const { assert (x); return uint64_ffs (x); }
  unsigned pop ()
  {
    assert (x);
    
    unsigned i = uint64_ffs (x);
    x = uint64_bitclear (x, i);
    return i;
  }
  
  bool subset (const smallbitset &bs) const
  {
    assert (n == bs.n);
    assert ((x >> n) == 0);
    assert ((bs.x >> n) == 0);
    return (x & bs.x) == x;
  }
  
  void push (unsigned i) { assert (! (*this % i)); operator += (i); }
  void yank (unsigned i) { assert (*this % i); operator -= (i); }
  void operator += (unsigned i) { assert (i <= n); x = uint64_bitset (x, i); }
  void operator -= (unsigned i) { assert (i <= n); x = uint64_bitclear (x, i); }
  
  void toggle (unsigned i) { assert (i <= n); x = uint64_bittoggle (x, i); }
  void toggle () { x ^= (((uint64)1) << n) - 1; }
  
  smallbitset &operator |= (const smallbitset &bs) { assert (n == bs.n); x |= bs.x; return *this; }
  smallbitset &operator &= (const smallbitset &bs) { assert (n == bs.n); x &= bs.x; return *this; }
  smallbitset &operator ^= (const smallbitset &bs) { assert (n == bs.n); x ^= bs.x; return *this; }
  
  bool operator == (const smallbitset &bs) const { return n == bs.n && x == bs.x; }
  bool operator != (const smallbitset &bs) const { return !operator == (bs); }
  
  inline bool operator % (unsigned i) const { return uint64_bittest (x, i); }
  inline bool operator () (unsigned i) const { return operator % (i); }
  
  void show_self () const;
  void display_self () const;

  void write_self (writer &w) const;
  hash_t hash_self () const { assert ((x >> n) == 0); return hash_combine (hash (n), hash (x)); }
};

class smallbitset_const_iter
{
  uint64 x;
  unsigned b;
  
 public:
  smallbitset_const_iter (const smallbitset &bs) : x(bs.x), b(0) { if (x) b = uint64_ffs (x); }
  smallbitset_const_iter (const smallbitset_const_iter &i) : x(i.x), b(i.b) { }
  ~smallbitset_const_iter () { }
  
  smallbitset_const_iter &operator = (const smallbitset_const_iter &i) { x = i.x; b = i.b; return *this; }
  
  unsigned val () const { return b; }
  operator bool () const { return (bool)x; }
  void operator ++ () { assert (x); x = uint64_bitclear (x, b); if (x) b = uint64_ffs (x); }
  void operator ++ (int) { operator ++ (); }
};

class smallbitset_iter
{
  smallbitset *bs;
  uint64 x;
  unsigned b;
  bool deleted;
  
 public:
  smallbitset_iter (smallbitset &bs_) 
    : bs(&bs_), x(bs_.x), b(0), deleted(0)
  {
    if (x) 
      b = uint64_ffs (x);
  }
  smallbitset_iter (const smallbitset_iter &i) 
    : bs(i.bs), x(i.x), b(i.b), deleted(i.deleted) { }
  ~smallbitset_iter () { }
  
  smallbitset_iter &operator = (const smallbitset_iter &i)
  {
    bs = i.bs;
    x = i.x;
    b = i.b;
    deleted = i.deleted;
    return *this;
  }
  
  void del () { assert (!deleted); (*bs) -= b; deleted = 1; }
  unsigned val () const { assert (!deleted); return b; }
  operator bool () const { assert (!deleted); return (bool)x; }
  void operator ++ ()
  {
    assert (x);
    
    x = uint64_bitclear (x, b);
    if (x)
      b = uint64_ffs (x);
    deleted = 0;
  }
  void operator ++ (int) { operator ++ (); }
};
