s
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
  
  work.append (knot_desc (TORUS, 0, 0));
  
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
compute_kh_sq (map<knot_desc,
		   triple<ptr<const module<Z2> >, mod_map<Z2>, mod_map<Z2> > > &knot_kh_sq,
	       knot_desc &desc)
{
  knot_diagram kd = desc.diagram ();
  
  unsigned rank = self_rank ();
  
  printf ("[% 2d] %s\n", rank, kd.name.c_str ());
  fflush (stdout);
  
  cube<Z2> c (kd);
  mod_map<Z2> d = c.compute_d (1, 0, 0, 0, 0);
  
  chain_complex_simplifier<Z2> s (c.khC, d, 1);
  assert (s.new_d == 0);
  
  steenrod_square sq (c, d, s);
  mod_map<Z2> sq1 = sq.sq1 ();
  mod_map<Z2> sq2 = sq.sq2 ();
  
  assert (sq1.compose (sq1) == 0);
  assert (sq2.compose (sq2) + sq1.compose (sq2).compose (sq1) == 0);
  
  knot_kh_sq.push (desc,
		   triple<ptr<const module<Z2> >, mod_map<Z2>, mod_map<Z2> > (s.new_C, sq1, sq2));
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
	    desc.i = (knot_desc::table)recv_int ();
	    desc.j = (knot_desc::table)recv_int ();
	    
	    printf ("[% 2d] CMD_DO %s\n", rank, desc.name ().c_str ());
	    
	    map<knot_desc,
		triple<ptr<const module<Z2> >, mod_map<Z2>, mod_map<Z2> > > knot_kh_sq;
	    
	    char buf[1000];
	    if (desc.t == knot_desc::TORUS)
	      {
		sprintf (buf, "T.dat");
		
		for (unsigned t = 3; t <= 16; t ++)   // twists
		  for (unsigned s = 2; s <= t; s ++) // strands
		    {
		      if ((s - 1) * t > 18)
			continue;
		      
		      desc.i = s;
		      desc.j = t;
		      
		      compute_kh_sq (knot_kh_sq, desc);
		    }
	      }
	    else
	      {
		unsigned j0 = desc.j;
		
		switch (desc.t)
		  {
		  case knot_desc::ROLFSEN:
		    sprintf (buf, "%d_%d.dat", desc.i, j0);
		    break;
		    
		  case knot_desc::HTW:
		    sprintf (buf, "K%d_%d.dat", desc.i, j0);
		    break;
		    
		  case knot_desc::MT:
		    sprintf (buf, "L%d_%d.dat", desc.i, j0);
		    break;
		    
		  default: abort ();
		  }
		
		for (unsigned j = j0;
		     j <= std::min (j0 + block_size - 1,
				    desc.table_crossing_knots ());
		     j ++)
		  {
		    desc.j = j;
		    
		    compute_kh_sq (knot_kh_sq, desc);
		  }
	      }
	    
	    {
	      writer w (buf);
	      write (w, knot_kh_sq);
	    }
	    
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
