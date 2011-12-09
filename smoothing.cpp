
#include <knotkit.h>

void
smoothing::init (const knot_diagram &d, smallbitset state)
{
  unsigned n_edges = d.num_edges ();
  
  n_circles = 0;
  assert (edge_circle.size () == n_edges);
  
  for (unsigned i = 1; i <= n_edges; i ++)
    edge_circle[i] = 0;
  
  for (unsigned i = 1; i <= n_edges; i ++)
    {
      if (edge_circle[i])
	continue;
      
      n_circles ++;
      for (unsigned p = edge_to (d, i);;)
	{
	  unsigned e = d.ept_edge (p);
	  edge_circle[e] = n_circles;
	  
	  p = d.resolve_next_ept (p, state % d.ept_crossing[p]);
	  assert (is_from_ept (d, p));
	  if (d.ept_edge (p) == i)
	    break;
          p = d.edge_other_ept (p);
	  assert (is_to_ept (d, p));
	}
    }
  
#ifndef NDEBUG
  for (unsigned c = 1; c <= d.n_crossings; c ++)
    {
      assert (crossing_from_inside (d, state, c)
	      == crossing_to_inside (d, state, c));
    }
#endif
}

unsigned
smoothing::monomial_from (const knot_diagram &d, const smoothing &from_s, unsigned j) const
{
  unsigned j2 = 0;
  for (unsigned k = 1; k <= d.num_edges (); k ++)
    {
      if (unsigned_bittest (j, from_s.edge_circle[k]))
	j2 = unsigned_bitset (j2, edge_circle[k]);
    }
  return j2;
}

unsigned
smoothing::monomial_from (const knot_diagram &d, const smoothing &from_s, unsigned j,
			  smallbitset skip) const
{
  unsigned j2 = 0;
  for (unsigned k = 1; k <= d.num_edges (); k ++)
    {
      if (skip % k)
	continue;
      if (unsigned_bittest (j, from_s.edge_circle[k]))
	j2 = unsigned_bitset (j2, edge_circle[k]);
    }
  return j2;
}

void
smoothing::show_self (const knot_diagram &d, smallbitset state) const
{
  printf ("smoothing ");
  show (state);
  printf (" ");
  
  smallbitset done (d.num_edges ());
  for (unsigned e = 1; e <= d.num_edges (); e ++)
    {
      if (done % e)
	continue;
      
      printf ("(");
      bool first = 1;
      for (unsigned f = e;;)
	{
	  done.push (f);
	  
	  unsigned p = edge_to (d, f);
	  unsigned c = d.ept_crossing (p);
	  
	  assert (is_to_ept (d, p));
	  
	  if (f == d.marked_edge)
	    printf ("*");
	  
	  if (first)
	    first = 0;
	  else
	    printf (", ");
	  if (is_crossing_from_ept (d, state, p))
	    {
	      printf ("t%d%s",
		      c,
		      crossing_from_inside (d, state, c) ? "in" : "out");
	    }
	  else
	    {
	      printf ("h%d%s",
		      c,
		      crossing_to_inside (d, state, c) ? "in" : "out");
	    }
	  
	  f = next_edge (d, state, f);
	  if (f == e)
	    break;
	}
      printf (")");
    }
}
