
template<unsigned B>
class unionfind
{
  unsigned n_sets;
  basedvector<unsigned, B> parent;
  
 public:
  unionfind (unsigned size);
  unionfind (const unionfind &u) : n_sets(u.n_sets), parent(u.parent) { }
  unionfind (copy, const unionfind &u) : n_sets(u.n_sets), parent(COPY, u.parent) { }
  unionfind (reader &r);
  ~unionfind () { }
  
  unsigned num_sets () const { return n_sets; }
  void clear ();
  
  void join (unsigned x, unsigned y);
  unsigned find (unsigned x);
  
  void write_self (writer &w) const;
};

template<unsigned B>
unionfind<B>::unionfind (unsigned size)
  : n_sets(size),
    parent(size)
{
  for (unsigned i = 0; i < size; i ++)
    parent[i + B] = i + B;
}

template<unsigned B>
unionfind<B>::unionfind (reader &r)
{
  read (r, n_sets);
  read (r, parent);
}

template<unsigned B> void
unionfind<B>::clear ()
{
  unsigned size = parent.size ();
  n_sets = size;
  for (unsigned i = 0; i < size; i ++)
    parent[i + B] = i + B;
}

template<unsigned B> void
unionfind<B>::join (unsigned x, unsigned y)
{
  unsigned xp = find (x),
    yp = find (y);
  if (xp != yp)
    {
      n_sets --;
      parent[yp] = xp;
    }
}

template<unsigned B> unsigned
unionfind<B>::find (unsigned x)
{
  unsigned p = parent[x];
  while (parent[p] != p)
    p = parent[p];
  
  while (x != p)
    {
      unsigned xp = parent[x];
      parent[x] = p;
      x = xp;
    }
  
  return p;
}

template<unsigned B> void
unionfind<B>::write_self (writer &w) const
{
  write (w, n_sets);
  write (w, parent);
}
