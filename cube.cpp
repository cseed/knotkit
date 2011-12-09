
#include <knotkit.h>

sseq
compute_szabo_sseq (const cube<Z2> &c)
{
  mod_map<Z2> d = c.compute_d (0, 0, 0, 0, 0);
  
  sseq_builder builder (c.khC, d);
  return builder.build_sseq ();
}
