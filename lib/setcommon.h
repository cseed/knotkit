
extern unsigned unsigned_set (const ullmanset<1> &s);
extern unsigned unsigned_set (const bitset &s);
extern unsigned unsigned_set (const set<unsigned> &s);

smallbitset::smallbitset (const bitset &t)
  : n(t.size ()),
    x(0)
{
  assert (t.size () <= max_size);
  if (n)
    {
      assert (t.v.size () == 1);
      x = t.v[0];
    }
  assert ((x >> n) == 0);
}

template<unsigned B>
ullmanset<B>::ullmanset (const bitset &t)
  : d(0)
{
  *this = ullmanset (t.size ());
  for (bitset_const_iter i = t; i; i ++)
    push (i.val ());
}

template<unsigned B> ullmanset<B> &
ullmanset<B>::operator = (const bitset &t)
{
  *this = ullmanset (t.size ());
  for (bitset_const_iter i = t; i; i ++)
    push (i.val ());
  return *this;
}

template<unsigned B> set<unsigned> &
setassign (set<unsigned> &to, unsigned dummy_size, const ullmanset<B> &from)
{
  assert (dummy_size == from.size ());
  
  to = set<unsigned> ();
  for (ullmanset_const_iter<B> i = from; i; i ++)
    to.push (i.val ());
  return to;
}

template<unsigned B> ullmanset<B> &
setassign (ullmanset<B> &to, unsigned size, const set<unsigned> &from)
{
  to = ullmanset<B> (size);
  for (set_const_iter<unsigned> i = from; i; i ++)
    to.push (i.val ());
  return to;
}
