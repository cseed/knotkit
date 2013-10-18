
/* Planar diagram of a knot.  For details, see:
   http://katlas.org/wiki/Planar_Diagrams */

class planar_diagram
{
public:
  std::string name;
  basedvector<basedvector<int, 1>, 1> crossings;
  
public:
  planar_diagram () { }
  planar_diagram (const std::string &name, unsigned n_crossings, const int crossings_ar[][4]);
  planar_diagram (const std::string &name_, const basedvector<basedvector<int, 1>, 1> &crossings_)
    : name(name_), crossings(crossings_)
  { }
  planar_diagram (const knot_diagram &kd);
  planar_diagram (const planar_diagram &pd)
    : name(pd.name),
      crossings(pd.crossings)
  { }
  ~planar_diagram () { }
  
  void show_knottheory () const;
  void display_knottheory () const { show_knottheory (); newline (); }
  
  void show_self () const { printf ("planar_diagram %s", name.c_str ()); }
  void display_self () const;
};
