#include <surfacekit.h>
#include <iostream>

/* option flags */
static bool opt_batch = 0;
static theory opt_theory = KHOVANOV_HOMOLOGY;
static bool opt_fight = 0;
static bool opt_mirror = 0;

static const char *opt_surface = 0;

static const char *program_name;

void
usage ()
{
  printf ("usage: %s [-v] [-m] [-k|-z|-c] <surface>\n", program_name);
  printf ("  compute map induced by surface in knot homology theory\n");
  printf ("options:\n");
  printf ("  -h           : help: print this message\n");
  printf ("  -m           : mirror\n");
  printf ("  -k|-l|-z|-c  : homology theory: Khovanov (-k),\n");
  printf ("                 Lee's spectral sequence (-l),\n");
  printf ("                 Szabo's geometric geometric spectral sequence (-z),\n");
  printf ("                 twisted geometric geometric spectral sequence (-c)\n");
}

void
parse_options (int argc, char **argv)
{
  for (int i = 1; i < argc; i ++)
    {
      if (argv[i][0] == '-')
	{
	  if (strcmp (argv[i], "-m") == 0)
	    opt_mirror = 1;
	  else if (strcmp (argv[i], "-k") == 0)
	    opt_theory = KHOVANOV_HOMOLOGY;
	  else if (strcmp (argv[i], "-l") == 0)
	    opt_theory = LEE_HOMOLOGY;
	  else if (strcmp (argv[i], "-z") == 0)
	    opt_theory = GEOMETRIC_SSEQ;
	  else if (strcmp (argv[i], "-c") == 0)
	    opt_theory = TWISTED_GEOMETRIC_SSEQ;
	  else if (strcmp (argv[i], "-h") == 0)
	    {
	      usage ();
	      exit (EXIT_SUCCESS);
	    }
	}
      else
	{
	  if (opt_surface)
	    {
	      fprintf (stderr, "error: too many arguments\n");
	      usage ();
	      exit (EXIT_FAILURE);
	    }
	  else
	    opt_surface = argv[i];
	}
    }
  
  if (!opt_surface)
    {
      fprintf (stderr, "error: missing <surface> argument\n");
      usage ();
      exit (EXIT_FAILURE);
    }
}

int
main (int argc, char **argv)
{
  program_name = argv[0];
  
  basedvector<marked_vertex_diagram, 1> mvd_list (load_mvds_from_file (HOME "/mvds/mvd_dat"));
  
  parse_options (argc, argv);
  
  marked_vertex_diagram mvd;
  bool found = 0;
  for (unsigned i = 1; i <= mvd_list.size (); i ++)
    {
      if (mvd_list[i].name == opt_surface)
	{
	  mvd = mvd_list[i];
	  found = 1;
	  break;
	}
    }
  
  if (!found)
    {
      fprintf (stderr, "error: unknown marked vertex diagram `%s'\n", opt_surface);
      exit (EXIT_FAILURE);
    }
  
  if (opt_mirror)
    mvd = marked_vertex_diagram (MIRROR, mvd);
  
  mvd.display_bohua ();
  
  typedef Z2 DefaultF;
  // typedef Z DefaultF;
  // typedef Q DefaultF;
  // typedef Zp<3> DefaultF;
  
  switch (opt_theory)
    {
    case KHOVANOV_HOMOLOGY:
      {
	mvd_cube<DefaultF, KHOVANOV_HOMOLOGY> cc (mvd);
      }
      break;
      
    case LEE_HOMOLOGY:
      {
	mvd_cube<DefaultF, LEE_HOMOLOGY> cc (mvd);
      }
      break;
      
    case GEOMETRIC_SSEQ:
      {
	mvd_cube<DefaultF, GEOMETRIC_SSEQ> cc (mvd);
      }
      break;
      
    case TWISTED_GEOMETRIC_SSEQ:
      {
	typedef fraction_field<polynomial<Z2> > R;
	
	mvd_cube<R, TWISTED_GEOMETRIC_SSEQ> cc (mvd);
      }
      break;
      
    default: abort ();
    }
  
  exit (EXIT_SUCCESS);
}

