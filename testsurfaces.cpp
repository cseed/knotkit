#include <surfacekit.h>
#include <iostream>

void test_mvd()
{
    for(unsigned i = 1; i <= 4; i++)
    {
        marked_vertex_diagram mvd(sample_mvd(i));
        mvd.display_bohua();
        mvd.print_knot_sequence();
    }
}


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
  printf ("usage: %s [-b] [-v] [-m] [-k|-z|-c] [<surface>]\n", program_name);
  printf ("  compute map induced by surface in knot homology theory\n");
  printf ("options:\n");
  printf ("  -b        : batch mode, <surface> argument required");
  printf ("                (interactive is the default)\n");
  printf ("  -h        : help: print this message\n");
  printf ("  -m        : mirror\n");
  printf ("  -v        : fight fight fight\n");
  printf ("  -k|-z|-c  : homology theory: Khovanov (-k),\n");
  printf ("                Szabo's geometric geometric spectral sequence (-z),\n");
  printf ("                twisted geometric geometric spectral sequence (-c)\n");
}

void
parse_options (int argc, char **argv)
{
  for (int i = 1; i < argc; i ++)
    {
      if (argv[i][0] == '-')
	{
	  if (strcmp (argv[i], "-b") == 0)
	    opt_batch = 1;
	  else if (strcmp (argv[i], "-v") == 0)
	    opt_fight = 1;
	  else if (strcmp (argv[i], "-m") == 0)
	    opt_mirror = 1;
	  else if (strcmp (argv[i], "-k") == 0)
	    opt_theory = KHOVANOV_HOMOLOGY;
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
  
  if (opt_batch
      && !opt_surface)
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
  
  if (opt_batch)
    {
      marked_vertex_diagram mvd;
      for (unsigned i = 1; i <= mvd_list.size (); i ++)
	{
	  if (mvd_list[i].name == opt_surface)
	    {
	      mvd = mvd_list[i];
	      break;
	    }
	}
      
      if (opt_mirror)
	mvd = marked_vertex_diagram (MIRROR, mvd);
      
      if (opt_fight)
	{
	  mvd_playoffs cc (mvd);
	}
      else
	{
	  mvd.display_bohua ();
	  mvd.print_knot_sequence ();
	  
	  switch (opt_theory)
	    {
	    case KHOVANOV_HOMOLOGY:
	      {
		mvd_cube<Z2, KHOVANOV_HOMOLOGY> cc (mvd);
	      }
	      break;
	      
	    case GEOMETRIC_SSEQ:
	      {
		mvd_cube<Z2, GEOMETRIC_SSEQ> cc (mvd);
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
	}
      
      exit (EXIT_SUCCESS);
    }
  else
    {
      // interactive
      while(1)
	{
	  std::cout << "enter the name of a surface\n";
	  std::string name;
	  std::cin >> name;
    
	  marked_vertex_diagram mvd;
	  if(name == "q")
	    break;
	  for (unsigned i = 1; i <= mvd_list.size(); i++)
	    {
	      if(name == mvd_list[i].name)
		mvd = mvd_list[i];
	    }
    
	  std::cout << "mirror?\n";
	  std::string mirror_str;
	  std::cin >> mirror_str;
    
	  if(mirror_str == "y" || mirror_str == "yes")
	    {
	      marked_vertex_diagram mvd_mirror(MIRROR, mvd);
	      mvd = mvd_mirror;
	    }
    
	  printf("There are %d saddles. Which would you like to use?\n", mvd.saddles.size());
	  std::string saddle_str;
	  std::cin >> saddle_str;    
    
	  if(saddle_str == "all" || saddle_str == "a")
	    {
	      mvd.display_bohua();
	      mvd.print_knot_sequence();
	      mvd_cube<Z2, KHOVANOV_HOMOLOGY> cc(mvd);
	      printf("\n_____________________________\n");
	    }
	  if(saddle_str == "v")
	    {
	      printf("fight fight fight \n");
	      mvd_playoffs cc(mvd);
	      printf("\n_____________________________\n");
	    }
	}
    }
  
  for(unsigned i = 1; i <= mvd_list.size(); i++)
  {
    mvd_list[i].display_bohua();
    printf("\nribbon euler characteristic: %d\n",  mvd_list[i].ribbon_euler_char());
    printf("components: %d\n",  mvd_list[i].n_components());
    printf("framing shift: %d\n",  mvd_list[i].framing_shift());
    if(mvd_list[i].orientable())
      printf("Orientable\n");
    else
      printf("Nonorientable\n");
    printf("________________________________\n");
  }
  
    std::cout<< "looking for orientation and framing conflicts\n\n";
  for(unsigned i = 1; i <= mvd_list.size(); i++)
  {
    if(mvd_list[i].orientable() && mvd_list[i].framing_shift())
    {
    mvd_list[i].display_bohua();
    printf("\nribbon euler characteristic: %d\n",  mvd_list[i].ribbon_euler_char());
    printf("components: %d\n",  mvd_list[i].n_components());
    printf("framing shift: %d\n",  mvd_list[i].framing_shift());
    if(mvd_list[i].orientable())
      printf("Orientable\n");
    else
      printf("Nonorientable\n");
    printf("________________________________\n");
    }
  }
  
  }

