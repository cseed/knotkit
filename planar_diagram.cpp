
#include <knotkit.h>

planar_diagram::planar_diagram (const std::string &name_, unsigned n_crossings, const int crossings_ar[][4])
  : name(name_),
    crossings(n_crossings)
{
  for (unsigned i = 1; i <= n_crossings; i ++)
    {
      for (unsigned j = 1; j <= 4; j ++)
	crossings[i].append (crossings_ar[i - 1][j - 1]);
    }
}

planar_diagram::planar_diagram (const knot_diagram &kd)
  : name(kd.name),
    crossings(kd.planar_diagram_crossings ())
{
}

void
planar_diagram::show_knottheory () const
{
  printf ("PD[");
  for (unsigned i = 1; i <= crossings.size (); i ++)
    {
      if (i > 1)
	printf (",");
      printf ("X[%d,%d,%d,%d]", 
	      crossings[i][1],
	      crossings[i][2],
	      crossings[i][3],
	      crossings[i][4]);
    }
  printf ("]");
}

void
planar_diagram::display_self () const
{
  printf ("planar_diagram %s ", name.c_str ());
  for (unsigned i = 1; i <= crossings.size (); i ++)
    {
      int i1 = crossings[i][1],
	i2 = crossings[i][2],
	i3 = crossings[i][3],
	i4 = crossings[i][4];
      
      if (i1 >= 10 || i2 >= 10 || i3 >= 10 || i4 >= 10)
	printf ("X_%d,%d,%d,%d", i1, i2, i3, i4);
      else
	printf ("X_%d%d%d%d", i1, i2, i3, i4);
    }
  printf ("\n");
}
