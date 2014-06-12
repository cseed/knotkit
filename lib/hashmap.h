
/* wrapper for std::unordered_map */

template<class K>
struct hasher : public std::unary_function<K, hash_t>
{
  hash_t operator () (const K &k) const { return hash (k); }
};

template<class K, class V>
class hashmap : public map_wrapper<std::unordered_map<K, V, hasher<K> >, K, V>
{
  typedef map_wrapper<std::unordered_map<K, V, hasher<K> >, K, V> base;
  
 public:
  hashmap () { }
  hashmap (const hashmap &m) : base(m) { }
  hashmap (copy, const hashmap &m) : base(COPY, m) { }
  hashmap (reader &r) : base(r) { }
  ~hashmap () { }
  
  hashmap &operator = (const hashmap &m) { base::operator = (m); return *this; }
};
