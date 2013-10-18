
/* wrapper for std::unordered_set */

template<class T>
class hashset : public set_wrapper<std::unordered_set<T, hasher<T> >, T>
{
  typedef set_wrapper<std::unordered_set<T, hasher<T> >, T> base;
  
 public:
  hashset () { }
  hashset (const hashset &m) : base(m) { }
  hashset (copy, const hashset &m) : base(COPY, m) { }
  hashset (initializer_list<T> il) : base(il) { }
  hashset (reader &r) : base(r) { }
  ~hashset () { }
  
  hashset &operator = (const hashset &m) { base::operator = (m); return *this; }
  hashset &operator = (initializer_list<T> il) { base::operator = (il); return *this; }
};

template<class T>
using hashset_iter = set_wrapper_iter<std::unordered_set<T, hasher<T> >, T>;

template<class T>
using hashset_const_iter = set_wrapper_const_iter<std::unordered_set<T, hasher<T> >, T>;
