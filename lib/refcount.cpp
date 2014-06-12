
#include <lib/lib.h>

void *operator new (size_t size) throw(std::bad_alloc)
{
#ifndef NDEBUG
  allocations ++;
#endif
  return malloc (size);
}
void *operator new [] (size_t size) throw(std::bad_alloc)
{
#ifndef NDEBUG
  allocations ++;
#endif
  return malloc (size);
}

void operator delete (void *p) throw()
{
#ifndef NDEBUG
  deallocations ++;
#endif
  free (p);
}
void operator delete [] (void *p) throw()
{
#ifndef NDEBUG
  deallocations ++;
#endif
  free (p);
}

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
