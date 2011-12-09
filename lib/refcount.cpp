
#include <lib/lib.h>

#ifndef NDEBUG
uint64 allocations = 0;
uint64 deallocations = 0;

class finalize
{
public:
  finalize ();
  ~finalize ();
};

finalize::finalize ()
{
}

finalize::~finalize ()
{
  // assert (allocations == deallocations);
}

static finalize finalizer;
#endif
