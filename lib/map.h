
/* wrapper for stl map */

template<class K, class V>
class map : public map_wrapper<std::map<K, V>, K, V>
{
  typedef map_wrapper<std::map<K, V>, K, V> base;
  
 public:
  map () { }
  map (unsigned dummy_size) : base(dummy_size) { }
  map (const map &m) : base(m) { }
  map (copy, const map &m) : base(COPY, m) { }
  map (reader &r) : base(r) { }
  ~map () { }
  
  map &operator = (const map &m) { base::operator = (m); return *this; }
};
