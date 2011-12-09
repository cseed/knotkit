
%skeleton "lalr1.cc"
%require "2.4.3"
%defines
%define parser_class_name "rd_parser"

%parse-param { resolution_diagram &parsed_rd }

// %no-lines

%code requires {
#include <knotkit.h>
}

%code{
#define YY_DECL \
  yy::rd_parser::token_type rd_yylex (yy::rd_parser::semantic_type *yylval)
YY_DECL;

#define CPT_MASK    0x0ffff
#define CPT_MARKED  0x10000
#define CPT_MARKED2 0x20000
}

%union {
  int integer;
  int cpt;
  bool inside;
  basedvector<int, 1> *cpt_vec;
}

%code {
#define yylex rd_yylex
}

%token <integer> INT

%token        END      0 "end of file"

%token H T
%token <inside> IN OUT

%type <cpt> crossing_point_spec crossing_point_1 crossing_point
%type <inside> inout

%type <cpt_vec> crossing_points

%%

%start resolution_diagram;

resolution_diagram:
    INT 
      {
	unsigned n_crossings = $1;
	parsed_rd.n_crossings = n_crossings;
	
	unsigned n_cpts = parsed_rd.num_cpts ();
	
	parsed_rd.resolved = smallbitset (n_crossings, 0);
	parsed_rd.to_resolve = smallbitset (n_crossings, ~0);
	parsed_rd.cpt_inside = smallbitset (n_cpts, 0);
	
	parsed_rd.prev = basedvector<unsigned, 1> (n_cpts);
	parsed_rd.next = basedvector<unsigned, 1> (n_cpts);
	parsed_rd.cpt_starting_circle = basedvector<unsigned, 1> (n_cpts);
	parsed_rd.cpt_ending_circle = basedvector<unsigned, 1> (n_cpts);
	
	for (unsigned i = 1; i <= n_cpts; i ++)
	  {
	    parsed_rd.prev[i]
	      = parsed_rd.next[i]
	      = parsed_rd.cpt_starting_circle[i]
	      = parsed_rd.cpt_ending_circle[i]
	      = 0;
	  }
      }
    ',' starting_circles dual_crossings_opt
  ;

starting_circles:
    starting_circle
  | starting_circles starting_circle
  ;

starting_circle:
    '(' crossing_points ')'
      {
	unsigned s = ++ parsed_rd.n_starting_circles;
	
	const basedvector<int, 1> &v = *$2;
	unsigned n = v.size ();
	unsigned p = v[n] & CPT_MASK;
	for (unsigned i = 1; i <= n; i ++)
	  {
	    unsigned c = v[i] & CPT_MASK;
	    
	    parsed_rd.next[p] = c;
	    parsed_rd.prev[c] = p;
	    parsed_rd.cpt_starting_circle[c] = s;
	    p = c;
	    
	    if (v[i] & CPT_MARKED)
	      parsed_rd.marked_edge = c;
	    if (v[i] & CPT_MARKED2)
	      parsed_rd.marked_edge2 = c;
	  }
      }
  ;

dual_crossings_opt:
    /* empty */
  | ',' dual_crossings

dual_crossings:
    dual_crossing
  | dual_crossing ',' dual_crossings
  ;

dual_crossing:
    INT '|' INT
  ;

crossing_points:
    crossing_point
      {
	basedvector<int, 1> *v =
	  new basedvector<int, 1> ();
	v->append ($1);
	$$ = v;
      }
  | crossing_points ',' crossing_point
      {
	basedvector<int, 1> *v = $1;
	v->append ($3);
	$$ = v;
      }
  ;

crossing_point:
    crossing_point_1
  | crossing_point_1 '*'
      { $$ = $1 | CPT_MARKED; }
  | crossing_point_1 '+'
      { $$ = $1 | CPT_MARKED2; }
  | crossing_point_1 '+' '*'
      { $$ = $1 | CPT_MARKED | CPT_MARKED2; }
  | crossing_point_1 '*' '+'
      { $$ = $1 | CPT_MARKED | CPT_MARKED2; }
  ;

crossing_point_1:
    crossing_point_spec inout
      {
	if ($2)
	  parsed_rd.cpt_inside.push ($1);
	$$ = $1;
      }
  ;

crossing_point_spec:
    H INT
      { $$ = parsed_rd.crossing_to_cpt ($2); }
  | T INT
      { $$ = parsed_rd.crossing_from_cpt ($2); }
  ;

inout:
    IN
  | OUT
  ;

%%

void
yy::rd_parser::error (const yy::rd_parser::location_type &l,
		      const std::string &m)
{
  fprintf (stderr, "rd_parser: parse error.\n");
  exit (EXIT_FAILURE);
}

void rd_scan_string (const char *s);

resolution_diagram
parse_resolution_diagram (const char *s)
{
  rd_scan_string (s);
  
  resolution_diagram rd;
  yy::rd_parser parser (rd);
  parser.parse ();
  
  return rd;
}
