
#include <knotkit.h>

sseq
compute_szabo_sseq (const cube<Z2> &c)
{
  mod_map<Z2> d = c.compute_d (0, 0, 0, 0, 0);
  
  sseq_builder builder (c.khC, d);
  return builder.build_sseq ();
}

mod_map<fraction_field<polynomial<Z2> > >
twisted_differential (const cube<fraction_field<polynomial<Z2> > > &c)
{
  twisted_cube<Z2> tc (c);
  
  basedvector<int, 1> edge_weight (c.kd.num_edges ());
  for (unsigned i = 1; i <= c.kd.num_edges (); i ++)
    edge_weight[i] = 1;
  
  mod_map<fraction_field<polynomial<Z2> > > d
    = (tc.twisted_d0 (edge_weight)
       + c.compute_d (0, 0, 0, 0, 0)
       + tc.compute_twisted_barE (edge_weight, 0, 0));
  return d;
}
