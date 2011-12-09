
// includes knotkit
#include "knot_parser.hh"

void knot_scan_string (const char *s);

knot_diagram
parse_knot (const char *s)
{
  knot_scan_string (s);
  
  knot_diagram d;
  yy::knot_parser parser (d);
  parser.parse ();
  
  return d;
}
