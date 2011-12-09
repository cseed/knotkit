
/* Dowker and Thistlethwaite codes */

/* minus = over */

class dt_code
{
 public:
  std::string name;
  basedvector<basedvector<int, 1>, 1> even_labels;
  
 public:
  dt_code ();
  dt_code (const std::string &name_, unsigned n_crossings, int even_labels_ar[])
    : name(name_), even_labels(1)
  {
    even_labels[1] = basedvector<int, 1> (n_crossings);
    for (unsigned i = 0; i < n_crossings; i ++)
      even_labels[1][i + 1] = even_labels_ar[i];
  }
  dt_code (const std::string &name_, const basedvector<int, 1> &even_labels_)
    : name(name_), even_labels(1)
  {
    even_labels[1] = even_labels_;
  }
  dt_code (const std::string &name_, const basedvector<basedvector<int, 1>, 1> &even_labels_)
    : name(name_), even_labels(even_labels_)
  {
  }
  dt_code (const std::string &name_, const char *p);
  
  dt_code (const dt_code &dt)
    : name(dt.name), even_labels(dt.even_labels)
  { }
  ~dt_code () { }
  
  dt_code &operator = (const dt_code &dt)
  {
    name = dt.name;
    even_labels = dt.even_labels;
    return *this;
  }

  unsigned num_crossings () const;
  unsigned num_components () const { return even_labels.size (); }
  
  void show_self () const { printf ("dt_code %s", name.c_str ()); }
  void display_self () const;
};
