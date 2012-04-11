
// includes lib.h
#include <algebra/algebra.h>

extern bool verbose;

static const unsigned max_edges = 58;
static const unsigned max_crossings = 29;
static const unsigned max_circles = 58;

class knot_diagram;

#include <planar_diagram.h>
#include <dt_code.h>
#include <knot_diagram.h>

#include <simplify_chain_complex.h>
#include <sseq.h>
#include <smoothing.h>
#include <cobordism.h>

#include <cube.h>
#include <steenrod_square.h>
#include <spanning_tree_complex.h>

class knot_desc
{
 public:
  enum table
  {
    NONE,
    ROLFSEN,
    HTW,
    HTW_ALT,
    HTW_NONALT,
    MT,
    MT_ALT,
    MT_NONALT,
    TORUS,
  };
  
  table t;
  unsigned i;
  unsigned j;
  
 public:
  knot_desc () : t(NONE) { }
  knot_desc (table t_, unsigned i_, unsigned j_)
    : t(t_), i(i_), j(j_)
  { }
  knot_desc (const knot_desc &desc)
    : t(desc.t), i(desc.i), j(desc.j)
  { }
  knot_desc (reader &r);
  ~knot_desc () { }
  
  knot_desc &operator = (const knot_desc &desc)
  {
    t = desc.t;
    i = desc.i;
    j = desc.j;
    return *this;
  }
  
  bool operator == (const knot_desc &desc) const
  {
    return t == desc.t && i == desc.i && j == desc.j;
  }
  bool operator < (const knot_desc &desc) const
  {
    if ((int)t < (int)desc.t)
      return 1;
    else if ((int)t > (int)desc.t)
      return 0;
    
    if (i < desc.i)
      return 1;
    else if (i > desc.i)
      return 0;
    
    return j < desc.j;
  }
  
  knot_diagram diagram () const;
  
  std::string name () const;
  unsigned table_crossing_knots () const;
  
  void write_self (writer &w) const;
  void show_self () const { printf ("%s", name ().c_str ()); }
  void display_self () const { show_self (); newline (); }
};

unsigned rolfsen_crossing_knots (unsigned n);
planar_diagram rolfsen_knot (unsigned n, unsigned k);

unsigned htw_knots (unsigned n, bool alternating);
unsigned htw_knots (unsigned n);

dt_code htw_knot (unsigned n, bool alternating, unsigned k);
dt_code htw_knot (unsigned n, unsigned k);

unsigned mt_links (unsigned n, bool alternating);
unsigned mt_links (unsigned n);

dt_code mt_link (unsigned n, bool alternating, unsigned k);
dt_code mt_link (unsigned n, unsigned k);

planar_diagram torus_knot (unsigned n_strands, unsigned n_shifts);

planar_diagram braid (unsigned n_strands, const basedvector<int, 1> &twists);
planar_diagram braid (unsigned n_strands, unsigned n_twists, int twists_ar[]);

knot_diagram parse_knot (const char *s);
resolution_diagram parse_resolution_diagram (const char *s);

// 11 <= n <= 15
basedvector<basedvector<unsigned, 1>, 1> mutant_knot_groups (unsigned n);
