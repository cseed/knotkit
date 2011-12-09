
#include <knotkit.h>

const char *program_name;

void
usage ()
{
  printf ("usage: %s [-f] [-h] [-o <file>] [-v] <knot>\n", program_name);
  printf ("  compute Szabo's geometric spectral sequence for knot <knot>\n");
  printf ("options:\n");
  printf ("  -f        : compute unreduced spectral sequence\n");
  printf ("                (reduced is the default)\n");
  printf ("  -h        : print this message\n");
  printf ("  -o <file> : write output to <file>\n");
  printf ("                (stdout is the default)\n");
  printf ("  -v        : verbose: report progress as the computation proceeds\n");
  printf ("<knot> can be one of:\n");
  printf ("  - the unknot, e.g. U or unknot\n");
  printf ("  - a torus knot, e.g. T(2,3)\n");
  printf ("  - a Rolfsen table knot, e.g. 10_124\n");
  printf ("  - a Hoste-Thistlethwaite-Weeks knot, e.g. 11a12 or 12n214\n");
  printf ("  - a Morwen Thistlethwaite link, e.g. L9a21 or L14n7631\n");
  printf ("  - a planar diagram, e.g.\n");
  printf ("      PD[X[1, 4, 2, 5], X[3, 6, 4, 1], X[5, 2, 6, 3]] or\n");
  printf ("      PD[[1, 4, 2, 5], [3, 6, 4, 1], [5, 2, 6, 3]]\n");
  printf ("  - a Dowker-Thistlethwaite code, e.g.\n");
  printf ("      DTCode[6,8,2,4],\n");
  printf ("      DT[dadbcda] or\n");
  printf ("      DT[{6, -8}, {-10, 12, -14, 2, -4}]\n");
  printf ("  - a braid, e.g. BR[2, {-1, -1, -1}]\n");
}

int
main (int argc, char **argv)
{
  program_name = argv[0];
  
  bool reduced = 1;
  const char *knot = 0;
  const char *file = 0;
  
  for (int i = 1; i < argc; i ++)
    {
      if (argv[i][0] == '-')
	{
	  if (strcmp (argv[i], "-f") == 0)
	    reduced = 0;
	  else if (strcmp (argv[i], "-h") == 0)
	    {
	      usage ();
	      exit (EXIT_SUCCESS);
	    }
	  else if (!strcmp (argv[i], "-v"))
	    verbose = 1;
	  else if (!strcmp (argv[i], "-o"))
	    {
	      i ++;
	      if (i == argc)
		{
		  fprintf (stderr, "error: missing argument to option `-o'\n");
		  exit (EXIT_FAILURE);
		}
	      file = argv[i];
	    }
	  else
	    {
	      fprintf (stderr, "error: unknown argument `%s'\n", argv[1]);
	      usage ();
	      exit (EXIT_FAILURE);
	    }
	}
      else
	{
	  if (knot)
	    {
	      fprintf (stderr, "error: too many arguments\n");
	      usage ();
	      exit (EXIT_FAILURE);
	    }
	  else
	    knot = argv[i];
	}
    }
  
  if (!knot)
    {
      fprintf (stderr, "error: missing <knot> argument\n");
      usage ();
      exit (EXIT_FAILURE);
    }
  
  FILE *outfp = 0;
  if (file)
    {
      outfp = fopen (file, "w");
      if (!outfp)
	{
	  stderror ("fopen: %s", file);
	  exit (EXIT_FAILURE);
	}
    }
  else
    outfp = stdout;
  
  knot_diagram d = parse_knot (knot);
  d.marked_edge = 1;
  cube<Z2> c (d, reduced);
  
  fprintf (outfp, "\\documentclass{article}\n\
\\usepackage{amsmath, tikz, hyperref}\n\
\\DeclareMathOperator{\\rank}{rank}\n\
\\setlength{\\parindent}{0pt}\n\
\n\
\\begin{document}\n\
\\pagestyle{empty}\n\
\\tableofcontents\n\
\\sloppy\n");
  
  compute_szabo_sseq (c).texshow (outfp, d.name);
  
  fprintf (outfp, "\\end{document}\n");
  
  if (file)
    fclose (outfp);
}
