
#include <knotkit.h>

dt_code::dt_code (const std::string &name_, const char *p)
  : name(name_)
{
  unsigned n_crossings = alpha_to_int (*p++);
  
  unsigned n_components = alpha_to_int (*p++);
  even_labels = basedvector<basedvector<int, 1>, 1> (n_components);
  
  for (unsigned i = 1; i <= n_components; i ++)
    even_labels[i] = basedvector<int, 1> (alpha_to_int (*p++));
  
  for (unsigned i = 1; i <= n_components; i ++)
    {
      for (unsigned j = 1; j <= even_labels[i].size (); j ++)
	even_labels[i][j] = alpha_to_int (*p++) * 2;
    }
  if (*p == '\n')
    p++;
  assert (*p == 0);
  
  assert (num_crossings () == n_crossings);
}

unsigned
dt_code::num_crossings () const
{
  unsigned n = 0;
  for (unsigned i = 1; i <= even_labels.size (); i ++)
    n += even_labels[i].size ();
  return n;
}

void
dt_code::display_self () const
{
  printf ("dt_code %s [", name.c_str ());
  for (unsigned i = 1; i <= even_labels.size (); i ++)
    {
      if (i > 1)
	printf (", ");
      printf ("[");
      for (unsigned j = 1; j <= even_labels[i].size (); j ++)
	{
	  if (j > 1)
	    printf (", ");
	  printf ("%d", even_labels[i][j]);
	}
      printf ("]");
    }
  printf ("]\n");
}
