#include <surfacekit.h>

#define HOME "/Users/josh/Dropbox/knot_kit/knotkit-r114"

int
main ()
{
  
  
  char buf[1000];
    
  sprintf (buf, HOME "/mvds/mvd_dat");
  load_mvds_from_file(buf);
 }
