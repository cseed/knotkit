
#include <knotkit.h>

static bool
find_cycle_1 (map<unsigned, int> &cycle, const directed_multigraph &bg,
	      set<unsigned> edges,
	      unsigned f,
	      unsigned e, unsigned e_to)
{
  for (set_const_iter<unsigned> i = edges; i; i ++)
    {
      if (i.val () == e)
	continue;
      
      if (bg.edge_from[i.val ()] == e_to)
	{
	  if (i.val () == f)
	    return 1;
	  
	  cycle.push (i.val (), 1);
	  if (find_cycle_1 (cycle, bg, edges, f,
			    i.val (), bg.edge_to[i.val ()]))
	    return 1;
	  
	  cycle.yank (i.val ());
	}
      else if (bg.edge_to[i.val ()] == e_to)
	{
	  if (i.val () == f)
	    return 1;
	  
	  cycle.push (i.val (), -1);
	  if (find_cycle_1 (cycle, bg, edges, f,
			    i.val (), bg.edge_from[i.val ()]))
	    return 1;
	  
	  cycle.yank (i.val ());
	}
    }
  
  return 0;
}

map<unsigned, int>
find_cycle (const directed_multigraph &bg,
	    set<unsigned> edges,
	    unsigned f, unsigned f_to)
{
  map<unsigned, int> cycle;
  
  if (bg.edge_to[f] == f_to)
    cycle.push (f, 1);
  else
    {
      assert (bg.edge_from[f] == f_to);
      cycle.push (f, -1);
    }
  
  bool b = find_cycle_1 (cycle, bg, edges, f,
			 f, f_to);
  assert (b);
  
  return cycle;
}

