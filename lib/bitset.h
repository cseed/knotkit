
typedef uint64 word_t;
  
static const unsigned word_bits = uint64_bits;
static const unsigned word_bit_mask = uint64_bit_mask;
  
inline unsigned word_bitcount (word_t x) { return uint64_bitcount (x); }
inline unsigned word_ffs (word_t x) { return uint64_ffs (x); }
  
inline word_t word_bitset (word_t x, unsigned i) { return uint64_bitset (x, i); }
inline word_t word_bitclear (word_t x, unsigned i) { return uint64_bitclear (x, i); }
inline bool word_bittest (word_t x, unsigned i) { return uint64_bittest (x, i); }

class bitset_iter;
class bitset_const_iter;

class bitset
{
private:
  friend class bitset_iter;
  friend class bitset_const_iter;
  
  friend class smallbitset;
  
  unsigned n;
  vector<word_t> v;
  
 public:
  typedef bitset_iter iter;
  typedef bitset_const_iter const_iter;
  
public:
  bitset () : n(0) { }
  inline bitset (unsigned n_);
  bitset (const bitset &bs) : n(bs.n), v(bs.v) { }
  
  bitset (copy, const bitset &bs) : n(bs.n), v(COPY, bs.v) { }
  // explicit bitset (const unsignedset &t);
  explicit bitset (const ullmanset<1> &t);
  
  ~bitset () { }
  
  bitset &operator = (const bitset &bs) { n = bs.n; v = bs.v; return *this; }
  // bitset &operator = (const unsignedset &t);
  bitset &operator = (const ullmanset<1> &t);
  
  bool is_empty () const;
  unsigned card () const;
  unsigned size () const { return n; }
  unsigned head () const;
  unsigned pop ();
  
  void push (unsigned i) { assert (! (*this % i)); operator += (i); }
  void yank (unsigned i) { assert (*this % i); operator -= (i); }
  inline void operator += (unsigned i);
  inline void operator -= (unsigned i);
  
  bitset &operator |= (const bitset &bs);
  bitset &operator &= (const bitset &bs);
  bitset &operator ^= (const bitset &bs);
  
  bool operator == (const bitset &bs) const;
  bool operator != (const bitset &bs) const { return !operator == (bs); }
  
  inline bool operator % (unsigned i) const;
  inline bool operator () (unsigned i) const { return operator % (i); }
};

class bitset_const_iter
{
private:
  const vector<word_t> &v;
  
  unsigned wi;
  word_t w;
  unsigned b;
  
  void find_next ();
  void advance ()
  {
    assert (wi < v.size ());
    assert (word_bittest (w, b)); 
    
    w = word_bitclear (w, b);
    find_next ();
  }
  
public:
  bitset_const_iter (const bitset &bs) : v(bs.v), wi(0), w(v[0]) { find_next (); }
  ~bitset_const_iter () { }
  
  int val () const { assert (wi < v.size ()); return wi * word_bits + b; }
  bitset_const_iter &operator ++ () { advance (); return *this; }
  void operator ++ (int) { advance (); }
  operator bool () const { return wi < v.size (); }
};

bitset::bitset (unsigned n_)
  : n(n_),
    v((n_ + word_bits - 1) / word_bits)
{
  unsigned wn = v.size ();
  for (unsigned i = 0; i < wn; i ++)
    v[i] = 0;
}

void
bitset::operator += (unsigned i)
{
  assert (i >= 1);
  assert (i <= n);
  
  unsigned w = (i - 1) / word_bits;
  unsigned b = (i - 1) & word_bit_mask;
  v[w] = word_bitset (v[w], b + 1);
}

void
bitset::operator -= (unsigned i)
{
  assert (i >= 1);
  assert (i <= n);
  
  unsigned w = (i - 1) / word_bits;
  unsigned b = (i - 1) & word_bit_mask;
  v[w] = word_bitclear (v[w], b + 1);
}

bool
bitset::operator % (unsigned i) const
{
  assert (i >= 1);
  assert (i <= n);
  
  unsigned w = (i - 1) / word_bits;
  unsigned b = (i - 1) & word_bit_mask;
  return word_bittest (v[w], b + 1);
}
