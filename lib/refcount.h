
#ifndef NDEBUG
extern uint64 allocations, deallocations;
#endif

inline void *operator new (size_t size)
{
#ifndef NDEBUG
  allocations ++;
#endif
  return malloc (size);
}
inline void *operator new [] (size_t size)
{
#ifndef NDEBUG
  allocations ++;
#endif
  return malloc (size);
}

inline void operator delete (void *p)
{
#ifndef NDEBUG
  deallocations ++;
#endif
  free (p);
}
inline void operator delete [] (void *p)
{
#ifndef NDEBUG
  deallocations ++;
#endif
  free (p);
}

class refcounted
{
 private:
  template<class T> friend class ptr;
  
  mutable unsigned refcount;
  
 public:
  refcounted () : refcount(0) { }
  refcounted (const refcounted &rc); // doesn't exist
  ~refcounted () { }
  
  refcounted &operator = (const refcounted &rc); // doesn't exist
};

template<class T>
class ptr
{
 private:
  template<class S> friend class ptr;
  
  T *p;
  
  void ref (T *newp) { assert (p == 0); p = newp; if (p) p->refcount ++; }
  void unref () { if (p && -- p->refcount == 0) { delete p; } p = 0; }
  
 public:
  ptr () : p(0) { }
  ptr (const ptr &r) : p(0) { ref (r.p); }
  template<class S> ptr (S *p_) : p(0) { ref (p_); }
  template<class S> ptr (const ptr<S> &r) : p(0) { ref (r.p); }
  ~ptr () { unref (); }
  
  ptr &operator = (const ptr &r) { unref (); ref (r.p); return *this; }
  template<class S> ptr &operator = (S *p_) { unref (); ref (p_); return *this; }
  
  template<class S> bool operator == (const ptr<S> &r) const { return p == r.p; }
  template<class S> bool operator != (const ptr<S> &r) const { return p != r.p; }
  
  template<class S> bool operator == (S *p_) const { return p == p_; }
  template<class S> bool operator != (S *p_) const { return p != p_; }
  
  bool operator == (int x) const { assert (x == 0); return p == 0; }
  bool operator != (int x) const { assert (x == 0); return p != 0; }
  
  T &operator * () const { assert (p); return *p; }
  T *get () const { assert (p); return p; }
  T *operator -> () const { assert (p); return p; }
};
