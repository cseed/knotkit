#include <surfacekit.h>
#include <iostream>

void test_mvd()
{
    for(unsigned i = 1; i <= 4; i++)
    {
        marked_vertex_diagram mvd(sample_mvd(i));
        mvd.display_bohua();
        mvd.print_knot_sequence();
    }
}

void test_mvd_cube()
{
    for(unsigned i = 1; i <= 5; i++)
    {
        marked_vertex_diagram mvd(sample_mvd(i));
        mvd.display_bohua();
        mvd.print_knot_sequence();
        mvd_cube cc(mvd);
        printf("\n_____________________________\n");
    }
}

int
main ()
{  
  char buf[1000];
  sprintf (buf, HOME "/mvds/mvd_dat");
  basedvector<marked_vertex_diagram,1> mvd_list(load_mvds_from_file(buf));
 
  while(1)
  {
    std::cout << "enter the name of a surface\n";
    std::string name;
    std::cin >> name;

    marked_vertex_diagram mvd;
    if(name == "q")
      break;
    for(int i = 1; i <= mvd_list.size(); i++)
    {
      if(name == mvd_list[i].name)
        mvd = mvd_list[i];
    }
    
    std::cout << "mirror?\n";
    std::string mirror_str;
    std::cin >> mirror_str;
    
    if(mirror_str == "y" || mirror_str == "yes")
    {
        marked_vertex_diagram mvd_mirror(MIRROR, mvd);
        mvd = mvd_mirror;
    }
    
    printf("There are %d saddles. Which would you like to use?\n", mvd.saddles.size());
    std::string saddle_str;
    std::cin >> saddle_str;    
    
    if(saddle_str == "all" || saddle_str == "a")
    {
      mvd.display_bohua();
      mvd.print_knot_sequence();
      mvd_cube cc(mvd);
      printf("\n_____________________________\n");
    }
    if(saddle_str == "v")
    {
      printf("fight fight fight \n");
      mvd_playoffs cc(mvd);
      printf("\n_____________________________\n");
    }
  }
  
  //test_mvd_cube ();
  for(int i = 1; i <= mvd_list.size(); i++)
  {
    mvd_list[i].display_bohua();
    printf("\nribbon euler characteristic: %d\n",  mvd_list[i].ribbon_euler_char());
    printf("components: %d\n",  mvd_list[i].n_components());
    printf("framing shift: %d\n",  mvd_list[i].framing_shift());
    if(mvd_list[i].orientable())
      printf("Orientable\n");
    else
      printf("Nonorientable\n");
    printf("________________________________\n");
  }
  
    //test_mvd_cube ();
    std::cout<< "looking for orientation and framing conflicts\n\n";
  for(int i = 1; i <= mvd_list.size(); i++)
  {
    if(mvd_list[i].orientable() && mvd_list[i].framing_shift())
    {
    mvd_list[i].display_bohua();
    printf("\nribbon euler characteristic: %d\n",  mvd_list[i].ribbon_euler_char());
    printf("components: %d\n",  mvd_list[i].n_components());
    printf("framing shift: %d\n",  mvd_list[i].framing_shift());
    if(mvd_list[i].orientable())
      printf("Orientable\n");
    else
      printf("Nonorientable\n");
    printf("________________________________\n");
    }
  }
  
  }

