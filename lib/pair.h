
template <class F, class S>
class pair
{
 public:
  F first;
  S second;
  
 public:
  pair () { }
  pair (const pair &p) : first(p.first), second(p.second) { }
  /* F, S might be references. */
  pair (F first_, S second_) : first(first_), second(second_) { }
  pair (reader &r);
  ~pair () { }
  
  pair &operator = (const pair &p) { first = p.first; second = p.second; return *this; }
  
  bool operator == (const pair &p) const { return first == p.first && second == p.second; }
  bool operator != (const pair &p) const { return !operator == (p); }
  bool operator < (const pair &p) const { return (first < p.first
						  || (first == p.first && second < p.second)); }
  
  void write_self (writer &w) const;
  hash_t hash_self () const { return hash_combine (hash (first), hash (second)); }
};

template <class F, class S>
pair<F, S>::pair (reader &r)
{
  read (r, first);
  read (r, second);
}

template<class F, class S> void
pair<F, S>::write_self (writer &w) const
{
  write (w, first);
  write (w, second);
}

template<class F, class S, class T>
class triple
{
public:
  F first;
  S second;
  T third;
  
  triple () { }
  triple (const triple &t)
    : first(t.first), second(t.second), third(t.third)
  { }
  triple (const F &first_, const S &second_, const T &third_)
    : first(first_), second(second_), third(third_)
  { }
  ~triple () { }

  triple &operator = (const triple &t)
  {
    first = t.first;
    second = t.second;
    third = t.third;
    return *this;
  }
  
  bool operator == (const triple &t) const
  {
    return first == t.first && second == t.second && third == t.third;
  }
  bool operator != (const triple &t) const { return !operator == (t); }
  
  bool operator < (const triple &t) const
  {
    if (first < t.first)
      return 1;
    else if (first > t.first)
      return 0;

    if (second < t.second)
      return 1;
    else if (second > t.second)
      return 0;

    if (third < t.third)
      return 1;
    else if (third > t.third)
      return 0;
  }
  
  hash_t hash_self () const
  {
    hash_t h = hash (first);
    h = hash_combine (h, hash (second));
    return hash_combine (h, hash (third));
  }
};
