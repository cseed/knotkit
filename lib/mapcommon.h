
template<class V, unsigned B> map<unsigned, V> &
mapassign (map<unsigned, V> &to, unsigned dummy_size, const ullmanmap<V, B> &from)
{
  assert (dummy_size == from.size ());
  
  to = map<unsigned, V> ();
  for (ullmanmap_const_iter<V, B> i = from; i; i ++)
    to.push (i.key (), i.val ());
  return to;
}

template<class V, unsigned B> ullmanmap<V, B> &
mapassign (ullmanmap<V, B> &to, unsigned size, const map<unsigned, V> &from)
{
  to = ullmanmap<V, B> (size);
  for (typename map<unsigned, V>::const_iter i = from; i; i ++)
    to.push (i.key (), i.val ());
  return to;
}

template<class K, class V> map<K, V> &
mapassign (map<K, V> &to, unsigned dummy_size, const map<K, V> &from)
{
  // todo: ?? careful about allocation: this unref's the previous object pointed to by to
  to = map<K, V> (COPY, from);
  return to;
}
