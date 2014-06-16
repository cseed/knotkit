
class grading
{
 public:
  int h, q;
  
 public:
  grading () : h(0), q(0) { }
  grading (int h_, int q_) : h(h_), q(q_) { }
  grading (const grading &gr) : h(gr.h), q(gr.q) { }
  grading (reader &r)
  {
    h = r.read_int ();
    q = r.read_int ();
  }
  
  ~grading () { }
  
  grading &operator = (const grading &gr) { h = gr.h; q = gr.q; return *this; }
  
  grading operator + (const grading &gr) const
  {
    return grading (h + gr.h, q + gr.q);
  }
  grading operator - (const grading &gr) const
  {
    return grading (h - gr.h, q - gr.q);
  }
  
  grading &operator += (const grading &gr)
  {
    h += gr.h;
    q += gr.q;
    return *this;
  }
  
  grading &operator -= (const grading &gr)
  {
    h -= gr.h;
    q -= gr.q;
    return *this;
  }
  
  bool operator == (const grading &gr) const { return h == gr.h && q == gr.q; }
  bool operator != (const grading &gr) const { return !operator == (gr); }
  
  bool operator < (const grading &gr) const
  {
    return h < gr.h
      || (h == gr.h && q < gr.q);
  }
  
  grading mirror_grading (unsigned nplus, unsigned nminus, bool torsion) const;
  
  int e() {return (q - 3*h);}
  int tau() {return (2 * h);}

  int delta() {return (q - 2*h);}

  void write_self (writer &w) const
  {
    w.write_int (h);
    w.write_int (q);
  }
    
  void show_self () const;
  void display_self () const;
};
