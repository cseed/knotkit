
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

unsigned rolfsen_crossing_knots (unsigned n);
planar_diagram rolfsen_knot (unsigned n, unsigned k);

unsigned htw_knots (unsigned n, bool alternating);
unsigned htw_knots (unsigned n);

dt_code htw_knot (unsigned n, bool alternating, unsigned k);
dt_code htw_knot (unsigned n, unsigned k);

unsigned mt_links (unsigned n, bool alternating);
dt_code mt_link (unsigned n, bool alternating, unsigned k);

planar_diagram torus_knot (unsigned n_strands, unsigned n_shifts);

planar_diagram braid (unsigned n_strands, const basedvector<int, 1> &twists);
planar_diagram braid (unsigned n_strands, unsigned n_twists, int twists_ar[]);

knot_diagram parse_knot (const char *s);
resolution_diagram parse_resolution_diagram (const char *s);

// 11 <= n <= 15
basedvector<basedvector<unsigned, 1>, 1> mutant_knot_groups (unsigned n);
