
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
  
  void display_bohua () const;

  void show_self () const { printf ("planar_diagram %s", name.c_str ()); }
  void display_self () const;
  
  bool is_oriented();
  void orient ();
  void renumber ();
  void rotate_crossing(int k)
  {
    int tmp;
    tmp = crossings[k][1];
    crossings[k][1] = crossings[k][3];
    crossings[k][3] = tmp;
    
    tmp = crossings[k][2];
    crossings[k][2] = crossings[k][4];
    crossings[k][4] = tmp;
  }
};
