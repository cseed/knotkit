
/* wrapper for std::unordered_map */

template<class K, class V>
class hashmap : public map_wrapper<std::unordered_map<K, V, hasher<K> >, K, V>
{
  typedef map_wrapper<std::unordered_map<K, V, hasher<K> >, K, V> base;
  
 public:
  hashmap () { }
  hashmap (const hashmap &m) : base(m) { }
  hashmap (copy, const hashmap &m) : base(COPY, m) { }
  hashmap (initializer_list<std::pair<const K, V> > il) : base(il) { }
  hashmap (reader &r) : base(r) { }
  ~hashmap () { }
  
  hashmap &operator = (const hashmap &m) { base::operator = (m); return *this; }
  hashmap &operator = (initializer_list<std::pair<const K, V> > il)
  {
    base::operator = (il);
    return *this;
  }
};

template<class K, class V>
using hashmap_iter = map_wrapper_iter<std::unordered_map<K, V>, K, V>;

template<class K, class V>
using hashmap_const_iter = map_wrapper_const_iter<std::unordered_map<K, V>, K, V>;
