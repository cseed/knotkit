
#include <lib/lib.h>

unsigned
directed_multigraph::num_components () const
{
  unionfind<1> u (n_vertices);
  
  unsigned n_edges = num_edges ();
  for (unsigned i = 1; i <= n_edges; i ++)
    u.join (edge_from[i], edge_to[i]);
  return u.num_sets ();
}
  
directed_multigraph 
directed_multigraph::component (unsigned v,
				basedvector<unsigned, 1> &edge_inj,
				basedvector<unsigned, 1> &virtex_inj) const
{
  unionfind<1> u (n_vertices);
  
  unsigned n_edges = num_edges ();
  for (unsigned i = 1; i <= n_edges; i ++)
    u.join (edge_from[i], edge_to[i]);
  
  ullmanset<1> comp_vertices (n_vertices);
  for (unsigned i = 1; i <= n_vertices; i ++)
    {
      if (u.find (v) == u.find (i))
	comp_vertices.push (i);
    }
  
  unsigned comp_n_vertices = comp_vertices.card ();
  virtex_inj = basedvector<unsigned, 1> (comp_n_vertices);
  for (unsigned i = 1; i <= comp_n_vertices; i ++)
    virtex_inj[i] = comp_vertices.nth (i - 1);
  
  basedvector<unsigned, 1> comp_edge_from,
    comp_edge_to;
  for (unsigned i = 1; i <= n_edges; i ++)
    {
      if (comp_vertices % edge_from[i])
	{
	  comp_edge_from.append (comp_vertices.position (edge_from[i]) + 1);
	  comp_edge_to.append (comp_vertices.position (edge_to[i]) + 1);
	  edge_inj.append (i);
	}
    }
  
  return directed_multigraph (comp_n_vertices,
			      comp_edge_from,
			      comp_edge_to);
}

class spanning_tree_builder
{
public:
  const directed_multigraph &g;
  basedvector<set<unsigned>, 1> vertex_edges;
  
  ullmanset<1> tree;
  ullmanset<1> visited_edges;
  ullmanset<1> visited_vertices;
  
  ullmanset<1> edges_to_visit;
  
  basedvector<set<unsigned>, 1> trees;
  
  void enumerate (unsigned i);
  
public:
  spanning_tree_builder (const directed_multigraph &g_);
};

spanning_tree_builder::spanning_tree_builder (const directed_multigraph &g_)
  : g(g_),
    vertex_edges(g.n_vertices),
    tree(g.num_edges ()),
    visited_edges(g.num_edges ()),
    visited_vertices(g.n_vertices),
    edges_to_visit(g.num_edges ())
{
  for (unsigned i = 1; i <= g.num_edges (); i ++)
    {
      vertex_edges[g.edge_from[i]] += i;
      vertex_edges[g.edge_to[i]] += i;
    }
  
  for (set_const_iter<unsigned> i = vertex_edges[1]; i; i ++)
    edges_to_visit += i.val ();
  enumerate (1);
}

void
spanning_tree_builder::enumerate (unsigned i)
{
  assert (i >= 1);
  
  if (visited_vertices.card () == g.n_vertices)
    {
      set<unsigned> s;
      setassign (s, tree.size (), tree);
      trees.append (s);
      
      return;
    }
  
  if (i > g.num_edges ()
      || i > edges_to_visit.card ())
    return;
  
  unsigned e = edges_to_visit.nth (i - 1);
  
  unsigned visited_edges_card = visited_edges.card ();
  visited_edges.push (e);
  
  unsigned from = g.edge_from[e],
    to = g.edge_to[e];
  if (! ((visited_vertices % from) && (visited_vertices % to)))
    {
      unsigned edges_to_visit_card = edges_to_visit.card ();
      unsigned visited_vertices_card = visited_vertices.card ();
      unsigned tree_card = tree.card ();
      
      tree.push (e);
      if (! (visited_vertices % from))
	{
	  visited_vertices += from;
	  
	  for (set_const_iter<unsigned> j = vertex_edges[from]; j; j ++)
	    {
	      if (! (visited_edges % j.val ()))
		edges_to_visit += j.val ();
	    }
	}
      if (! (visited_vertices % to))
	{
	  visited_vertices += to;
	  
	  for (set_const_iter<unsigned> j = vertex_edges[to]; j; j ++)
	    {
	      if (! (visited_edges % j.val ()))
		edges_to_visit += j.val ();
	    }
	}
      
      // with e
      enumerate (i + 1);
      
      tree.restore (tree_card);
      visited_vertices.restore (visited_vertices_card);
      edges_to_visit.restore (edges_to_visit_card);
    }
  
  // without e
  enumerate (i + 1);
  
  visited_edges.restore (visited_edges_card);
}

basedvector<set<unsigned>, 1> 
directed_multigraph::spanning_trees () const
{
  spanning_tree_builder builder (*this);
  return builder.trees;
}

void
directed_multigraph::display_self () const
{
  printf ("directed_multigraph\n");
  printf ("  n_vertices %d\n", n_vertices);
  printf ("  n_edges %d\n", num_edges ());
  printf ("  edges:\n");
  for (unsigned i = 1; i <= num_edges (); i ++)
    {
      printf ("  % 4d: %d -> %d\n",
	      i,
	      edge_from[i],
	      edge_to[i]);
    }
}
