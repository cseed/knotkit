
#include <surfacekit.h>

marked_vertex_diagram::marked_vertex_diagram (const std::string &name_, unsigned n_crossings, unsigned n_saddles, const int crossings_ar[][4], const int saddles_ar[][4])
  : name(name_),
    crossings(n_crossings),
    saddles(n_saddles)
{
    for(unsigned i = 1; i <= n_crossings; i++)
    {
      basedvector<int,1> this_crossing(4);
      for(unsigned j = 1; j <= 4; j++)
      {
        this_crossing[j] = crossings_ar[i-1][j-1];
      }
      crossings[i] = this_crossing;
    }
    
    for(unsigned i = 1; i <= n_saddles; i++)
    {
      basedvector<int,1> this_saddle(4);
      for(unsigned j = 1; j <= 4; j++)
        this_saddle[j] = saddles_ar[i-1][j-1];
      saddles[i] = this_saddle;
    }
}


marked_vertex_diagram::marked_vertex_diagram (mirror, const marked_vertex_diagram &mvd)
  : name("mirror(" + mvd.name + ")"),
    crossings(mvd.crossings.size()),
    saddles(mvd.saddles)
{
    for(unsigned i = 1; i <= crossings.size(); i++)
    {
      basedvector<int,1> this_crossing(4);
      for(unsigned j = 1; j <= 4; j++)
      {
        this_crossing[j] = mvd.crossings[i][add_base1_mod4(j,1)];
      }
      crossings[i] = this_crossing;
    }
}


planar_diagram 
marked_vertex_diagram::intermediate_knot(unsigned n)
{
  //make a pd from the crossings. make a unionfind, and go thru saddles identifying edges. then go thru and renumber the ones in the pd. make a knot_detector, and use its renumber_pd function.
  assert(n<=saddles.size());
  
  basedvector<basedvector<int, 1>, 1> crossings_copy(COPY2, crossings);
  planar_diagram pd(" ", crossings_copy);
  
  //printf("%d time thru\n",n);
  //pd.display_bohua();
  
  //how many edges?
  unsigned n_edges = 2*(crossings.size()+saddles.size());
  unionfind<1> edge_collapsed_name (n_edges);
  //do the zero res
  for(unsigned i = 1; i <= (saddles.size()-n); i++)
  {
      edge_collapsed_name.join(((unsigned) saddles[i][1]),((unsigned) saddles[i][2]));
      edge_collapsed_name.join(((unsigned) saddles[i][3]),((unsigned) saddles[i][4]));
  }
  //do the one res
  for(unsigned i = (saddles.size() - n + 1); i <= saddles.size(); i++)
  {
      edge_collapsed_name.join(((unsigned) saddles[i][1]),((unsigned) saddles[i][4]));
      edge_collapsed_name.join(((unsigned) saddles[i][2]),((unsigned) saddles[i][3]));
  }
  
  for(unsigned i = 1; i <= crossings.size(); i++)
  {
    for(unsigned j = 1; j <= 4; j++)
    {
        pd.crossings[i][j] = (int) edge_collapsed_name.find( (unsigned) crossings[i][j]);
    }
  }

  pd.renumber();

  return pd;
}

void
marked_vertex_diagram::display_bohua ()
{
  printf ("%s\nCrossings[", name.c_str ());
  for (unsigned i = 1; i <= crossings.size (); i ++)
    {
      if (i > 1)
	printf (",");
      printf ("[%d,%d,%d,%d]", 
	      crossings[i][1],
	      crossings[i][2],
	      crossings[i][3],
	      crossings[i][4]);
    }
  printf ("]\nSaddles[");
    for (unsigned i = 1; i <= saddles.size (); i ++)
    {
        if (i > 1)
            printf (",");
        printf ("[%d,%d,%d,%d]", 
                saddles[i][1],
                saddles[i][2],
                saddles[i][3],
                saddles[i][4]);
    }
    printf ("]\n");
    
    printf("open euler characteristic: %d\n", open_euler_char());
    printf("components: %d\n",  n_components());
    printf("framing shift: %d\n",  framing_shift());
}

/*our convention: put the saddles first.*/
knot_diagram
marked_vertex_diagram::mvd_as_kd()
{
  basedvector<basedvector<int, 1>, 1> vertices;
  for(unsigned i = 1; i <= saddles.size(); i++)
  {
    vertices.append(saddles[i]);
  }
  for(unsigned i = 1; i <= crossings.size(); i++)
  {
    vertices.append(crossings[i]);
  }
  planar_diagram pd(name, vertices);
  knot_diagram kd(pd);
  return kd;
}

int
marked_vertex_diagram::open_euler_char()
{
  return -saddles.size();
}


unsigned
marked_vertex_diagram::n_components()
{
  unionfind<1> collapsed_edges (n_edges());
  
  for(unsigned i = 1; i <= crossings.size(); i++)
  {
      collapsed_edges.join(((unsigned) crossings[i][1]),((unsigned) crossings[i][3]));
      collapsed_edges.join(((unsigned) crossings[i][2]),((unsigned) crossings[i][4]));
  }

  for(unsigned i = 1; i <= saddles.size(); i++)
  {
      collapsed_edges.join(((unsigned) saddles[i][1]),((unsigned) saddles[i][2]));
      collapsed_edges.join(((unsigned) saddles[i][3]),((unsigned) saddles[i][4]));
      collapsed_edges.join(((unsigned) saddles[i][1]),((unsigned) saddles[i][3]));
  }

  return collapsed_edges.num_sets();
}

int
marked_vertex_diagram::framing_shift()
{
  knot_diagram kd_init(initial_knot());
  knot_diagram kd_final(final_knot());
  
  return kd_final.writhe() - kd_init.writhe();
}
