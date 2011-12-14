
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

marked_vertex_diagram sample_mvd (unsigned n)
{

    
static const struct {
  const char *name;
  int crossings_ar[2][4];
  int saddles_ar[1][4];
} mvd2_1[] = {
  { "hopf seifert slantwise", {{1,2,4,3},{3,6,5,1}},{{4,2,5,6}}},
  { "hopf surface outside", {{1,2,4,3},{3,4,6,5}},{{5,6,2,1}}},
  { "hopf surface inside", {{1,2,4,3},{5,6,2,1}},{{3,4,6,5}}}
};

static const struct {
  const char *name;
  int crossings_ar[3][4];
  int saddles_ar[3][4];
} mvd3_3[] = {
  { "hope it lives bounding trefoil", {{1,4,3,2},{4,6,5,3},{6,8,7,5}},{{9,2,7,8},{1,12,11,10},{9,10,11,12}}}
};


static const struct {
  const char *name;
  int crossings_ar[5][4];
  int saddles_ar[2][4];
} mvd5_2[] = {
  { "5_2 int (klein)", {{1,3,4,2},{3,5,6,4},{5,7,8,6},{9,8,12,11},{13,14,7,10}},     {{1,2,9,10},{11,12,14,13}}},
  { "5_2 ext (klein)", {{1,3,4,2},{3,5,6,4},{5,7,8,6},{9,8,12,11},{11,12,14,13}},     {{1,2,9,10},{13,14,7,10}}},
  { "5_2 slant (torus)", {{1,3,4,2},{3,5,6,4},{5,7,8,6},{9,8,12,11},{11,14,13,10}},     {{1,2,9,10},{12,7,13,14}}},
  { "5_2 standard seifert (torus)",{{3,8,7,2},{5,9,8,4},{9,11,10,7},{11,13,12,10},{13,14,1,12}}, {{5,4,6,14},{3,2,1,6}}},
};

static const struct {
  const char *name;
  int crossings_ar[5][4];
  int saddles_ar[1][4];
} mvd5_1[] = {
   { "5_2 try again", {{4,3,2,1},{3,6,5,2},{6,8,7,5},{1,7,10,9},{9,10,12,11}},{{8,4,11,12}}}
};



    int crossings_ar1[][4] = {{4,3,2,1},{3,4,6,5}};
    int saddles_ar1[][4] = {{1,2,5,6}};

    int crossings_ar2[][4] = {{2,5,4,1},{3,7,6,2},{11,8,9,12}};
    int saddles_ar2[][4] = {{9,8,3,1},{11,10,6,7},{10,12,4,5}};
    
    int crossings_ar3[][4] = {{4,2,1,1},{8,7,7,6}};
    int saddles_ar3[][4] = {{3,6,2,5},{4,8,3,5}};
    
    int crossings_ar4[][4] = {{4,6,5,1},{6,3,2,5}};
    int saddles_ar4[][4] = {{2,3,4,1}};
    
    int crossings_ar5[][4] = {{2,8,7,1},{3,5,6,2},{1,4,5,3}};
    int saddles_ar5[][4] = {{6,4,7,8}};

    int crossings_ar6[][4] = {{2,5,6,1},{1,4,3,2}};
    int saddles_ar6[][4] = {{3,4,6,5}};

    int crossings_ar7[][4] = {{1,6,5,3},{3,10,9,2},{2,8,7,1}};
    int saddles_ar7[][4] = {{5,4,9,10},{4,6,7,8}};
      
    int crossings_ar8[][4] = {{2,6,16,1},{11,5,12,6},{4,10,5,11},{4,7,3,8},{7,2,1,3},{9,14,8,13}};
    int saddles_ar8[][4] = {{14,9,15,10},{16,12,15,13}};
      
    int crossings_ar9[][4] = {{17,2,16,1},{11,9,10,10},{18,8,17,9},{3,7,2,8},{4,19,3,18},{12,20,13,19},{21,13,20,14},{22,7,21,6},{1,16,22,15}};
    int saddles_ar9[][4] = {{15,6,14,5},{12,4,11,5}};
    
    int crossings_ar10[][4] = {{1,2,7,8},{9,16,15,10},{13,11,10,15},{4,11,3,12},{12,5,1,4},{6,2,5,3}};
    int saddles_ar10[][4] = {{8,14,16,9},{7,6,13,14}};
    
    int crossings_ar11[][4] = {{3,8,7,2},{5,9,8,4},{9,11,10,7},{11,13,12,10},{13,14,1,12}};
    int saddles_ar11[][4] = {{5,4,6,14},{3,2,1,6}};    
    
    int crossings_ar12[][4] = {{9,7,10,14},{13,11,8,12},{7,5,6,8},{5,3,4,6},{3,1,2,4}};
    int saddles_ar12[][4] = {{14,10,11,13},{2,1,9,12}}; 
    
    int crossings_ar13[][4] = {{4,1,2,3},{5,6,3,2},{6,5,22,7},{9,4,7,8},{18,10,8,21},{13,9,10,11},{12,14,15,13},{1,15,14,16},{19,16,12,17},{20,17,11,18}};
    int saddles_ar13[][4] = {{22,19,20,21}}; 
            
    //int crossings_ar10[][4] = {{1,17,2,16},{10,11,9,10},{9,18,8,17},{8,3,7,2},{18,4,19,3},{19,12,20,13},{14,21,13,20},{6,22,7,21},{15,1,16,22}};
    //int saddles_ar10[][4] = {{15,6,14,5},{12,4,11,5}};
    
    
    switch (n)
    {
        case 1:
            //return marked_vertex_diagram ("hopf rank 1 seifert" , 2, 1,crossings_ar1,saddles_ar1);
            return marked_vertex_diagram ("ribbon 8_9", 10, 1, crossings_ar13, saddles_ar13);
        case 2:
            return marked_vertex_diagram (mvd2_1[0].name, 2, 1, mvd2_1[0].crossings_ar, mvd2_1[0].saddles_ar);
            //return marked_vertex_diagram ("trefoil standard seifert" , 3, 3,crossings_ar2,saddles_ar2);
        case 3:
            return marked_vertex_diagram (mvd2_1[1].name, 2, 1, mvd2_1[1].crossings_ar, mvd2_1[1].saddles_ar);
            //return marked_vertex_diagram ("stabilized torus" , 2, 2,crossings_ar3,saddles_ar3);
        case 4:
            return marked_vertex_diagram (mvd2_1[2].name, 2, 1, mvd2_1[2].crossings_ar, mvd2_1[2].saddles_ar);
            //return marked_vertex_diagram ("hopf seifert", 2, 1, crossings_ar4, saddles_ar4);
        case 5:
            return marked_vertex_diagram ( mvd3_3[0].name, 3, 3, mvd3_3[0].crossings_ar, mvd3_3[0].saddles_ar);
            //return marked_vertex_diagram (mvd5_2[0].name, 5, 2, mvd5_2[0].crossings_ar, mvd5_2[0].saddles_ar);
            //return marked_vertex_diagram ("hopf to trefoil", 3, 1, crossings_ar5, saddles_ar5);
        case 6:
            return marked_vertex_diagram (mvd5_2[1].name, 5, 2, mvd5_2[1].crossings_ar, mvd5_2[1].saddles_ar);
              //return marked_vertex_diagram ("hopf other seifert", 2, 1, crossings_ar6, saddles_ar6);
        case 7:
            return marked_vertex_diagram (mvd5_2[2].name, 5, 2, mvd5_2[2].crossings_ar, mvd5_2[2].saddles_ar);
            //return marked_vertex_diagram ("to trefoil, i hope", 3, 2, crossings_ar7, saddles_ar7);
        case 8:
            return marked_vertex_diagram (mvd5_2[3].name, 5, 2, mvd5_2[3].crossings_ar, mvd5_2[3].saddles_ar);

            //return marked_vertex_diagram ("genus 1 on solomon again" , 6, 2,crossings_ar8,saddles_ar8);
        case 9:
            return marked_vertex_diagram ("vanishing on 5_2 mirror" , 6, 2,crossings_ar10,saddles_ar10);
        case 10:
            return marked_vertex_diagram ( mvd5_1[0].name, 5, 1, mvd5_1[0].crossings_ar, mvd5_1[0].saddles_ar);
            //return marked_vertex_diagram ("standard seifert on 5_2" , 5, 2,crossings_ar11,saddles_ar11);
        case 11:
            return marked_vertex_diagram ("annular res of a crossing 5_2" ,5 , 2,crossings_ar12,saddles_ar12);
        case 12:
            return marked_vertex_diagram ("genus 1 on solomon seal mirror" , 9, 2,crossings_ar9,saddles_ar9);

       // case 2: 
        //    return marked_vertex_diagram ("unknotted torus", 2, {{1, 4, 2, 3},{3, 2, 4, 1}},{HORIZONTAL, VERTICAL});
       // case 3:
         //   return marked_vertex_diagram ("stabilized unknotted torus", 4, {{1, 4, 2, 1},{8, 7, 7, 6},{4, 8, 3, 5}, {5, 3, 6, 2}}, {OVER, UNDER, HORIZONTAL, VERTICAL});
        default: abort();
    }
}
