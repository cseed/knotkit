
/* wrapper for std::map */

template<class K, class V>
class map : public map_wrapper<std::map<K, V>, K, V>
{
  typedef map_wrapper<std::map<K, V>, K, V> base;
  
 public:
  map () { }
  map (unsigned dummy_size) : base(dummy_size) { }
  map (const map &m) : base(m) { }
  map (copy, const map &m) : base(COPY, m) { }
  map (initializer_list<std::pair<const K, V> > il) : base(il) { }
  map (reader &r) : base(r) { }
  ~map () { }
  
  map &operator = (const map &m) { base::operator = (m); return *this; }
  map &operator = (initializer_list<std::pair<const K, V> > il)
  {
    base::operator = (il);
    return *this;
  }
};

template<class K, class V>
using map_iter = map_wrapper_iter<std::map<K, V>, K, V>;

template<class K, class V>
using map_const_iter = map_wrapper_const_iter<std::map<K, V>, K, V>;
