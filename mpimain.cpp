
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
  
  for (unsigned i = 1; i <= 14; i ++)
    for (unsigned j = 1; j <= mt_links (i, 0); j ++)
      {
	knot_diagram kd (mt_links (i, 0, j));
	unsigned n = kd.num_components ();
	if (n < 2)
	  continue;
	
	work.append (knot_desc (knot_desc::MT, i, j));
      }
  
#if 0  
  basedvector<basedvector<unsigned, 1>, 1> groups = mutant_knot_groups (15);

  for (unsigned i = 1; i <= groups.size (); i ++)
    for (unsigned j = 1; j <= groups[i].size (); j ++)
      work.append (knot_desc (knot_desc::HTW, 15, groups[i][j]));
#endif
  
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

bool
file_exists (const char *file)
{
  struct stat stat_buf;
  if (stat (file, &stat_buf) != 0)
    {
      if (errno == ENOENT)
	return 0;
      
      stderror ("stat: %s", file);
      exit (EXIT_FAILURE);
    }
  
  return 1;
}

void
compute_forgetful (int rank, knot_desc desc, const char *buf)
{
  knot_diagram kd = desc.diagram ();
  
  typedef Zp<97> R;
  
  unsigned n = kd.num_components ();
  assert (n >= 2);
  assert (n < 97);
  
  printf ("[% 2d] ", rank);
  show (kd);
  newline ();
  
  unionfind<1> u (kd.num_edges ());
  for (unsigned i = 1; i <= kd.n_crossings; i ++)
    {
      u.join (kd.ept_edge (kd.crossings[i][1]),
	      kd.ept_edge (kd.crossings[i][3]));
      u.join (kd.ept_edge (kd.crossings[i][2]),
	      kd.ept_edge (kd.crossings[i][4]));
    }
	
  map<unsigned, unsigned> root_comp;
  unsigned t = 0;
  for (unsigned i = 1; i <= kd.num_edges (); i ++)
    {
      if (u.find (i) == i)
	{
	  ++ t;
	  root_comp.push (i, t);
	}
    }
  assert (t == n);
	
  basedvector<R, 1> comp_weight (n);
  for (unsigned i = 1; i <= n; i ++)
    comp_weight[i] = R ((int)i);
	
  map<unsigned, R> crossing_over_sign;
	
  // crossings
  set<unsigned> pending;
  set<unsigned> finished;
	
  crossing_over_sign.push (1, R (1));
  pending.push (1);
  
  while (pending.card () > 0)
    {
      unsigned x = pending.pop ();
      finished.push (x);
      
      R s = crossing_over_sign(x);
      
      for (unsigned j = 1; j <= 4; j ++)
	{
	  unsigned p = kd.crossings[x][j];
	  R t = kd.is_over_ept (p) ? s : -s;  // sign of (x, p)
	  
	  unsigned q = kd.edge_other_ept (p);
	  unsigned x2 = kd.ept_crossing[q];
	  
	  R u = kd.is_over_ept (q) ? -t : t;
	  
	  if (crossing_over_sign % x2)
	    assert (crossing_over_sign(x2) == u);
	  else
	    crossing_over_sign.push (x2, u);
	  
	  if (! (finished % x2))
	    pending += x2;
	}
    }
  assert (finished.card () == kd.n_crossings);
	
#if 0
  printf ("crossing_over_sign:\n");
  for (unsigned i = 1; i <= kd.n_crossings; i ++)
    {
      printf ("  % 2d: ", i);
      display (crossing_over_sign(i));
    }
#endif
	
  cube<R> c (kd);
#if 0
  for (unsigned i = 0; i < c.n_resolutions; i ++)
    {
      smallbitset state (kd.n_crossings, i);
      smoothing s (kd, state);
      s.show_self (kd, state);
      newline ();
    }
#endif
  
  mod_map<R> untwisted_d = c.compute_d (1, 0, 0, 0, 0);
  assert (untwisted_d.compose (untwisted_d) == 0);
	
  mod_map<R> d = untwisted_d;
  for (unsigned x = 1; x <= kd.n_crossings; x ++)
    {
      unsigned p1 = kd.crossings[x][1],
	p2 = kd.crossings[x][2];
      assert (kd.is_over_ept (p2));
	    
      unsigned r1 = u.find (kd.ept_edge (p1)),
	r2 = u.find (kd.ept_edge (p2));
	    
      unsigned c1 = root_comp(r1),
	c2 = root_comp(r2);
	    
      if (c1 != c2)
	{
	  R s = crossing_over_sign(x);
		
	  R w_under = comp_weight[c1];
	  R w_over = comp_weight[c2];
		
	  d = d + c.compute_dinv (x)*(s*(w_over - w_under))
	    ;
	}
    }
  
  // display ("d:\n", d);
  
  // mod_map<Z> h = d.compose (d);
  // display ("h:\n", h);
  
  assert (d.compose (d) == 0);
  
  ptr<const module<R> > Ek = c.khC;
  mod_map<R> dk = d;
  
  basedvector<multivariate_laurentpoly<Z>, 1> pages;
  
  unsigned kinf;
  for (int dq = 0;; dq -= 2)
    {
      chain_complex_simplifier<R> s (Ek, dk, dq);
      Ek = s.new_C;
      dk = s.new_d;
      
      pages.append (Ek->free_poincare_polynomial ());
      
      printf ("[% 2d] ", rank);
      printf ("|E%d| = %d\n", ((- dq) / 2) + 1, Ek->dim ());
      if (dk == 0)
	{
	  kinf = ((- dq) / 2);
	  break;
	}
    }
  
  unsigned total_lk = kd.total_linking_number ();
  
  unsigned trivial_bound = total_lk == 0 ? 2 : total_lk;
  
  printf ("[% 2d] ", rank);
  printf ("kinf = %d, trivial bound = %d (total_lk = %d)\n",
	  kinf, trivial_bound, total_lk);
  
  if (trivial_bound < kinf)
    {
      printf ("[% 2d] ", rank);
      printf (" > BETTER\n");
    }
  fflush (stdout);
  
  assert (desc.t == knot_desc::MT);  
  gzfile_writer w (buf);
  write (w, desc);
  write (w, pages);
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
	    
	    assert (desc.t == knot_desc::MT);
	    char buf[1000];
	    sprintf (buf, "/scratch/network/cseed/forgetful/L%d_%d.dat.gz",
		     desc.i, desc.j);
	    
	    if (! file_exits ())
	      compute_forgetful (rank, desc, buf);
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
