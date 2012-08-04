
#include <knotkit.h>

#include <mpi.h>
#include <mpi_aux.h>

#define CMD_DO 1
#define CMD_DIE 2

static const int block_size = 100;

void
master ()
{
  basedvector<knot_desc, 1> work;
  
  basedvector<basedvector<unsigned, 1>, 1> groups = mutant_knot_groups (15);
  
  for (unsigned i = 1; i <= groups.size (); i ++)
    for (unsigned j = 1; j <= groups[i].size (); j ++)
      work.append (knot_desc (knot_desc::HTW, 15, groups[i][j]));
  
#if 0
  for (unsigned i = 14; i >= 1; i --)
    {
      if (i <= 10)
	{
	  for (unsigned j = 1; j <= rolfsen_crossing_knots (i); j += block_size)
	    {
	      work.append (knot_desc (ROLFSEN, i, j));
	    }
	}
      
      for (unsigned j = 1; j <= htw_knots (i); j += block_size)
	{
	  work.append (knot_desc (HTW, i, j));
	}
      
      if (i <= 13)
	{
	  for (unsigned j = 1; j <= mt_links (i); j += block_size)
	    {
	      work.append (knot_desc (MT, i, j));
	    }
	}
    }
#endif
  
  int ntasks = num_tasks ();
  
  set<unsigned> active;
  
  for (int rank = 1; rank < ntasks && work.size () > 0; rank ++)
    {
      send_int (CMD_DO, rank);
      
      knot_desc desc = work.pop ();
      send_int ((int)desc.t, rank);
      send_int ((int)desc.i, rank);
      send_int ((int)desc.j, rank);
      
      active.push (rank);
    }
  while (work.size () > 0)
    {
      int rank;
      int dummy = recv_int (&rank);
      
      send_int (CMD_DO, rank);
      
      knot_desc desc = work.pop ();
      send_int ((int)desc.t, rank);
      send_int ((int)desc.i, rank);
      send_int ((int)desc.j, rank);
    }
  
  while (active.card () > 0)
    {
      int rank;
      int dummy = recv_int (&rank);
      active -= rank;
    }
  
  for (int rank = 1; rank < ntasks; rank ++)
    {
      send_int (CMD_DIE, rank);
    }
}

void
slave ()
{
  int rank = self_rank ();
  
  for (;;)
    {
      int cmd = recv_int ();
      switch (cmd)
	{
	case CMD_DO:
	  {
	    knot_desc desc;
	    desc.t = (knot_desc::table)recv_int ();
	    desc.i = (unsigned)recv_int ();
	    desc.j = (unsigned)recv_int ();
	    
	    printf ("[% 2d] CMD_DO %s\n", rank, desc.name ().c_str ());
	    
	    char buf[1000];
	    sprintf (buf, "/scratch/network/cseed/incoming/K%d_%d.dat.gz",
		     desc.i, desc.j);
	    
	    struct stat stat_buf;
	    if (stat (buf, &stat_buf) != 0)
	      {
		if (errno == ENOENT)
		  {
		    knot_diagram kd = desc.diagram ();
	    
		    cube<Z2> c (kd);
		    mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
	    
		    chain_complex_simplifier<Z2> s (c.khC, d, 1);
		    assert (s.new_d == 0);
	    
		    steenrod_square sq (c, d, s);
		    mod_map<Z2> sq1 = sq.sq1 ();
		    mod_map<Z2> sq2 = sq.sq2 ();
		    
		    assert (sq1.compose (sq1) == 0);
		    assert (sq2.compose (sq2) + sq1.compose (sq2).compose (sq1) == 0);
		    
		    gzfile_writer w (buf);
		    write (w, sq1);
		    write (w, sq2);
		  }
		else
		  {
		    stderror ("stat: %s", buf);
		    exit (EXIT_FAILURE);
		  }
	      }
	    else
	      printf ("skip %s: exists.\n", buf);
	    
	    send_int (0, 0);
	  }
	  break;
	  
	case CMD_DIE:
	  return;
	}
    }
}

int
main (int argc, char **argv)
{
  comm_init (&argc, &argv);
  
  int rank = self_rank (),
    ntasks = num_tasks ();
  
  printf ("[% 2d] alive\n", rank);
  fflush (stdout);
  
  if (rank == 0)
    {
      printf ("ntasks = %d\n", ntasks);
      fflush (stdout);
      
      master ();
    }
  else
    slave ();
  
  comm_finalize ();
  return 0;
}
