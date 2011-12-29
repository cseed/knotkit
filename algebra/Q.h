
class Q
{
 public:
  typedef ::linear_combination<Q> linear_combination;
  typedef ::linear_combination_const_iter<Q> linear_combination_const_iter;

 private:
  enum steal { STEAL };
  
  class Q_impl : public refcounted
  {
  public:
    mpq_t x;
    
  public:
    Q_impl () { mpq_init (x); }
    Q_impl (int init)
    {
      mpq_init (x);
      mpq_set_si (x, init, 1);
    }
    
    Q_impl (copy, mpq_srcptr init)
    {
      mpq_init (x);
      mpq_set (x, init);
    }
    
    Q_impl (steal, mpq_srcptr init) { x[0] = *init; }
    Q_impl (reader &r)
    {
      mpq_init (x);
      mpz_inp_raw (mpq_numref (x), r.fp);
      mpz_inp_raw (mpq_denref (x), r.fp);
    }
    
    ~Q_impl () { mpq_clear (x); }
    
    void write_self (writer &w) const
    {
      mpz_out_raw (w.fp, mpq_numref (x));
      mpz_out_raw (w.fp, mpq_denref (x));
    }
  };
  
  ptr<Q_impl> impl;
  
  Q (steal, mpq_srcptr init) : impl(new Q_impl (STEAL, init)) { }

 public:
  Q () : impl(new Q_impl) { }
  Q (int init) : impl(new Q_impl (init)) { }
  Q (const Q &q) : impl(q.impl) { }
  Q (copy, const Q &q) : impl(new Q_impl (COPY, q.impl->x)) { }
  Q (reader &r) : impl(new Q_impl (r)) { }
  ~Q () { }
  
  Q &operator = (const Q &q) { impl = q.impl; return *this; }
  Q &operator = (int x) { impl = new Q_impl (x); return *this; }
  
  bool operator == (const Q &q) const { return mpq_cmp (impl->x,q.impl->x) == 0; }
  
  bool operator == (int r) const { return mpq_cmp_si (impl->x, r, 1) == 0; }
  bool operator != (int r) const { return !operator == (r); }
  
  bool operator < (const Q &q) const { return mpq_cmp (impl->x, q.impl->x) < 0; }
  
  bool is_unit () const
  {
    return *this != 0;
  }
  
  Q operator - () const
  {
    mpq_t x;
    mpq_init (x);
    mpq_neg (x, impl->x);
    return Q (STEAL, x);
  }

  Q recip () const
  {
    mpq_t x;
    mpq_init (x);
    mpq_inv (x, impl->x);
    return Q (STEAL, x);
  }
  
  Q operator + (const Q &q) const
  {
    mpq_t x;
    mpq_init (x);
    mpq_add (x, impl->x, q.impl->x);
    return Q (STEAL, x);
  }
  
  Q operator - (const Q &q) const
  {
    mpq_t x;
    mpq_init (x);
    mpq_sub (x, impl->x, q.impl->x);
    return Q (STEAL, x);
  }
  
  Q operator * (const Q &q) const
  {
    mpq_t x;
    mpq_init (x);
    mpq_mul (x, impl->x, q.impl->x);
    return Q (STEAL, x);
  }
  
  Q operator / (const Q &q) const
  {
    assert (q != 0);
    
    mpq_t x;
    mpq_init (x);
    mpq_div (x, impl->x, q.impl->x);
    return Q (STEAL, x);
  }
  
  Q &muladdeq (const Q &q1, const Q &q2)
  {
    // ??? do inline saves refcount overhead
    return operator += (q1 * q2);
  }
  
  Q &operator += (const Q &q)
  {
    mpq_add (impl->x, impl->x, q.impl->x);
    return *this;
  }
  
  Q &operator -= (const Q &q)
  {
    mpq_sub (impl->x, impl->x, q.impl->x);
    return *this;
  }
  
  Q &operator *= (const Q &q)
  {
    mpq_mul (impl->x, impl->x, q.impl->x);
    return *this;
  }
  
  Q &operator /= (const Q &q)
  {
    assert (q != 0);
    
    mpq_div (impl->x, impl->x, q.impl->x);
    return *this;
  }
  
  // d | n, d.divides (n)
  bool divides (const Q &num) const
  {
    return *this != 0 || num == 0;
  }
  
  bool operator | (const Q &num) const { return divides (num); }
  
  Q div (const Q &d) const { return operator / (d); }
  
  triple<Q, Q, Q> extended_gcd (const Q &q) const
  {
    if (*this != 0)
      return triple<Q, Q, Q> (*this, 1, 0);
    else
      return triple<Q, Q, Q> (q, 0, 1);
  }
  
  Q gcd (const Q &q) const
  {
    assert (*this != 0 || q != 0);
    return 1;
  }
  
  static void show_ring () { printf ("Q"); }
  void show_self () const { mpq_out_str (stdout, 10, impl->x); }
  void display_self () const { show_self (); newline (); }
  void write_self (writer &w) const { write (w, *impl); }
};
