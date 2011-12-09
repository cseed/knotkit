
class directed_multigraph
{
 public:
  unsigned n_vertices;
  basedvector<unsigned, 1> edge_from,
    edge_to;
  
 public:
  directed_multigraph () : n_vertices(0) { }
  directed_multigraph (unsigned n_vertices_,
		       basedvector<unsigned, 1> edge_from_,
		       basedvector<unsigned, 1> edge_to_)
    : n_vertices(n_vertices_),
      edge_from(edge_from_),
      edge_to(edge_to_)
  {
  }
  
  directed_multigraph (const directed_multigraph &g)
    : n_vertices(g.n_vertices),
      edge_from(g.edge_from),
      edge_to(g.edge_to)
  { }
  
  directed_multigraph (copy, const directed_multigraph &g)
    : n_vertices(g.n_vertices),
      edge_from(COPY, g.edge_from),
      edge_to(COPY, g.edge_to)
  {
  }
  
  directed_multigraph &operator = (const directed_multigraph &g)
  {
    n_vertices = g.n_vertices;
    edge_from = g.edge_from;
    edge_to = g.edge_to;
    return *this;
  }
  
  unsigned num_edges () const { return edge_from.size (); }
  unsigned num_components () const;
  
  // the component containing the vertex v
  directed_multigraph component (unsigned v,
				 basedvector<unsigned, 1> &edge_inj,
				 basedvector<unsigned, 1> &virtex_inj) const;
  
  basedvector<set<unsigned>, 1> spanning_trees () const;
  
  void display_self () const;
};
