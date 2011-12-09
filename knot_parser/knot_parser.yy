
%skeleton "lalr1.cc"
%require "2.4.3"
%defines
%define parser_class_name "knot_parser"

%parse-param { knot_diagram &parsed_knot }

// %no-lines

%code requires {
#include <knotkit.h>
}

%code{
#define YY_DECL \
  yy::knot_parser::token_type knot_yylex (yy::knot_parser::semantic_type *yylval)
YY_DECL;
}

%union {
  int integer;
  bool alternating;
  basedvector<int, 1> *int_vec;
  basedvector<basedvector<int, 1>, 1> *int_vec2;
  const char *string;
}

%code {
#define yylex knot_yylex
}

%token <integer> INT

%token        END      0 "end of file"

%token PD DT BR LINK X T
%token U UNKNOT
%token <alternating> ALT NONALT
%token <string> STRING

%type <alternating> alt_spec

%type <int_vec> int_vec int_vec_1
%type <int_vec2> int_vec2

%type <int_vec> crossing
%type <int_vec2> crossing_vec

%%

%start knot;

knot:
    rolfsen_knot
  | htw_knot
  | mt_link
  | planar_diagram
  | dt
  | torus_link
  | unknot
  | braid
  ;

rolfsen_knot: 
    INT '_' INT
      { 
	unsigned n = $1,
	  k = $3;
	
	if (n >= 1 && n <= 10
	    && k >= 1 && k <= rolfsen_crossing_knots (n))
	  parsed_knot = knot_diagram (rolfsen_knot (n, k));
	else
	  {
	    fprintf (stderr, "knot_parser: no such Rolfsen knot `%d_%d'\n",
		     n, k);
	    exit (EXIT_FAILURE);
	  }
      }
  ;

htw_knot: 
    INT alt_spec INT
      {
	unsigned n = $1,
	  k = $3;
	bool alt = $2;
	
	if (n >= 1 && n <= 16
	    && k >= 1 && k <= htw_knots (n, alt))
	  parsed_knot = knot_diagram (htw_knot (n, alt, k));
	else
	  {
	    fprintf (stderr, "knot_parser: no such HTW knot `%d%c%d'\n",
		     n, alt ? 'a' : 'n', k);
	    exit (EXIT_FAILURE);
	  }
      }
  ;

mt_link: 
    LINK INT alt_spec INT
      {
	unsigned n = $2,
	  k = $4;
	bool alt = $3;
	
	if (n >= 1 && n <= 14
	    && k >= 1 && k <= mt_links (n, alt))
	  parsed_knot = knot_diagram (mt_link (n, alt, k));
	else
	  {
	    fprintf (stderr, "knot_parser: no such MT link `%d%c%d'\n", 
		     n, alt ? 'a' : 'n', k);
	    exit (EXIT_FAILURE);
	  }
      }
  ;

planar_diagram:
    PD '[' int_vec2 ']'
      { parsed_knot = knot_diagram (planar_diagram ("<parsed>", *$3)); }
  | PD '[' crossing_vec ']'
      { parsed_knot = knot_diagram (planar_diagram ("<parsed>", *$3)); }
  ;

dt:
    DT '[' int_vec_1 ']'
      {
	basedvector<basedvector<int, 1>, 1> even_labels (1);
	even_labels[1] = *$3;
	parsed_knot = knot_diagram (dt_code ("<parsed>", even_labels));
      }
  | DT '[' int_vec2 ']'
      { parsed_knot = knot_diagram (dt_code ("<parsed>", *$3)); }
  | DT '[' STRING ']'
      { parsed_knot = knot_diagram (dt_code ("<parsed>", $3)); }
  ;

torus_link:
    T '(' INT ',' INT ')'
      { parsed_knot = knot_diagram (torus_knot ($3, $5)); }
  ;

braid:
    BR '[' INT ',' int_vec ']'
      { parsed_knot = knot_diagram (braid ($3, *$5)); }
  ;

unknot:
    UNKNOT
      {
	unsigned unknot_ar[1][4] = {
	  { 2, 1, 3, 4, },
	};
	parsed_knot = knot_diagram ("U", 1, unknot_ar);
      }
  ;

alt_spec: 
    ALT
  | NONALT
  ;

int_vec2:
    int_vec
      {
	basedvector<basedvector<int, 1>, 1> *v
	  = new basedvector<basedvector<int, 1>, 1> ();
	v->append (*$1);
	$$ = v;
      }
  | int_vec2 ',' int_vec
      { 
	basedvector<basedvector<int, 1>, 1> *v = $1;
	v->append (*$3);
	$$ = v;
      }
  ;

int_vec: 
    '{' int_vec_1 '}'
      { $$ = $2; }
  | '[' int_vec_1 ']'
      { $$ = $2; }
  ;

int_vec_1:
    INT
      {
	basedvector<int, 1> *v =
	  new basedvector<int, 1> ();
	v->append ($1);
	$$ = v;
      }
  | int_vec_1 ',' INT
      {
	basedvector<int, 1> *v = $1;
	v->append ($3);
	$$ = v;
      }
  ;

crossing_vec:
    crossing
      {
	basedvector<basedvector<int, 1>, 1> *v
	  = new basedvector<basedvector<int, 1>, 1> ();
	v->append (*$1);
	$$ = v;
      }
  | crossing_vec ',' crossing
      { 
	basedvector<basedvector<int, 1>, 1> *v = $1;
	v->append (*$3);
	$$ = v;
      }
  ;

crossing:
    X '[' INT ',' INT ',' INT ',' INT ']'
      {
	basedvector<int, 1> *v
	  = new basedvector<int, 1> ();
	v->append ($3);
	v->append ($5);
	v->append ($7);
	v->append ($9);
	$$ = v;
      }

%%

void
yy::knot_parser::error (const yy::knot_parser::location_type &l,
                        const std::string &m)
{
  fprintf (stderr, "knot_parser: parse error.\n");
  exit (EXIT_FAILURE);
}

void knot_scan_string (const char *s);

knot_diagram
parse_knot (const char *s)
{
  knot_scan_string (s);
  
  knot_diagram d;
  yy::knot_parser parser (d);
  parser.parse ();
  
  return d;
}
