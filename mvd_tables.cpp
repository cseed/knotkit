
#include <surfacekit.h>

basedvector<marked_vertex_diagram, 1> load_mvds_from_file(std::string file_name)
{
  basedvector<marked_vertex_diagram, 1> mvd_vect;
  
  char buf[1000];
  
  FILE *fp = fopen (file_name.c_str(), "r");
  if (fp == 0)
    {
      stderror ("fopen: %s", file_name.c_str ());
      exit (EXIT_FAILURE);
    }
  while (fgets (buf, sizeof buf, fp) != 0)
    {
      const char *p = buf;
      
      int k;
      char mvd_name[50];
            
      //each mvd, peel off name
      while ( sscanf (p, " { %s %n", mvd_name, &k) == 1)
      {
        p += k;
        basedvector<basedvector<int, 1>, 1> crossings;
        basedvector<basedvector<int, 1>, 1> saddles;
        //peal off first curly brace of crossings data
        if(sscanf (p, " { %n", &k) < 0)
        {
          stderror ("bad formatting of an mvd table, no opening brace");
          exit (EXIT_FAILURE);
        }
        p+=k;
      
        //each crossing
        basedvector<int,1> this_crossing(4);
        while (sscanf (p, " { %d, %d, %d, %d } %n", &this_crossing[1], &this_crossing[2], &this_crossing[3], &this_crossing[4], &k) == 4)
        {
          p +=k;
          basedvector<int,1> c_to_add(COPY,this_crossing);
          crossings.append(c_to_add);
          if(p[0]==',')
            p+=1;
        }
        
        //peal off intermediate curlies
        if(sscanf (p, " } {%n", &k) < 0)
        {
          stderror ("bad formatting of an mvd table");
          exit (EXIT_FAILURE);
        }
        p+=k;
        
        //each saddle
        basedvector<int,1> this_saddle(4);
        while (sscanf (p, " { %d, %d, %d, %d }%n", &this_saddle[1], &this_saddle[2], &this_saddle[3], &this_saddle[4], &k) == 4)
        {
          p +=k;
          basedvector<int,1> s_to_add(COPY,this_saddle);
          saddles.append(s_to_add);
          if(p[0]==',')
            p+=1;
        }
        
        marked_vertex_diagram mvd(mvd_name, crossings, saddles);
        mvd_vect.append(mvd);
       // mvd.display_bohua();
      }
    }
    printf("loaded %d marked vertex diagrams\n", mvd_vect.size());
    return mvd_vect;
}
