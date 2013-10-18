/* A Bison parser, made by GNU Bison 2.6.5.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* First part of user declarations.  */

/* Line 278 of lalr1.cc  */
#line 38 "knot_parser/knot_parser.cc"


#include "knot_parser.hh"

/* User implementation prologue.  */

/* Line 284 of lalr1.cc  */
#line 46 "knot_parser/knot_parser.cc"
/* Unqualified %code blocks.  */
/* Line 285 of lalr1.cc  */
#line 15 "knot_parser/knot_parser.yy"

#define YY_DECL \
  yy::knot_parser::token_type knot_yylex (yy::knot_parser::semantic_type *yylval)
YY_DECL;

/* Line 285 of lalr1.cc  */
#line 30 "knot_parser/knot_parser.yy"

#define yylex knot_yylex


/* Line 285 of lalr1.cc  */
#line 62 "knot_parser/knot_parser.cc"


# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
/* Line 352 of lalr1.cc  */
#line 157 "knot_parser/knot_parser.cc"

  /// Build a parser object.
  knot_parser::knot_parser (knot_diagram &parsed_knot_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      parsed_knot (parsed_knot_yyarg)
  {
  }

  knot_parser::~knot_parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  knot_parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    std::ostream& yyo = debug_stream ();
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  knot_parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  knot_parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  knot_parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  knot_parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  knot_parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  knot_parser::debug_level_type
  knot_parser::debug_level () const
  {
    return yydebug_;
  }

  void
  knot_parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  knot_parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  knot_parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  knot_parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    // State.
    int yyn;
    int yylen = 0;
    int yystate = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    static semantic_type yyval_default;
    semantic_type yylval = yyval_default;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval);
      }

    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:
/* Line 661 of lalr1.cc  */
#line 60 "knot_parser/knot_parser.yy"
    { 
        parsed_knot = *(yysemantic_stack_[(1) - (1)].kd); 
        delete (yysemantic_stack_[(1) - (1)].kd);
      }
    break;

  case 4:
/* Line 661 of lalr1.cc  */
#line 69 "knot_parser/knot_parser.yy"
    {
       (yyval.kd) = new knot_diagram (DISJOINT_UNION, *(yysemantic_stack_[(2) - (1)].kd), *(yysemantic_stack_[(2) - (2)].kd));
       delete (yysemantic_stack_[(2) - (1)].kd);
       delete (yysemantic_stack_[(2) - (2)].kd);
     }
    break;

  case 13:
/* Line 661 of lalr1.cc  */
#line 89 "knot_parser/knot_parser.yy"
    { 
	unsigned n = (yysemantic_stack_[(3) - (1)].integer),
	  k = (yysemantic_stack_[(3) - (3)].integer);
	
	if (n >= 1 && n <= 10
	    && k >= 1 && k <= rolfsen_crossing_knots (n))
	  (yyval.kd) = new knot_diagram (rolfsen_knot (n, k));
	else
	  {
	    fprintf (stderr, "knot_parser: no such Rolfsen knot `%d_%d'\n",
		     n, k);
	    exit (EXIT_FAILURE);
	  }
      }
    break;

  case 14:
/* Line 661 of lalr1.cc  */
#line 107 "knot_parser/knot_parser.yy"
    {
	unsigned n = (yysemantic_stack_[(3) - (1)].integer),
	  k = (yysemantic_stack_[(3) - (3)].integer);
	bool alt = (yysemantic_stack_[(3) - (2)].alternating);
	
	if (n >= 1 && n <= 16
	    && k >= 1 && k <= htw_knots (n, alt))
	  (yyval.kd) = new knot_diagram (htw_knot (n, alt, k));
	else
	  {
	    fprintf (stderr, "knot_parser: no such HTW knot `%d%c%d'\n",
		     n, alt ? 'a' : 'n', k);
	    exit (EXIT_FAILURE);
	  }
      }
    break;

  case 15:
/* Line 661 of lalr1.cc  */
#line 126 "knot_parser/knot_parser.yy"
    {
	unsigned n = (yysemantic_stack_[(4) - (2)].integer),
	  k = (yysemantic_stack_[(4) - (4)].integer);
	bool alt = (yysemantic_stack_[(4) - (3)].alternating);
	
	if (n >= 1 && n <= 14
	    && k >= 1 && k <= mt_links (n, alt))
	  (yyval.kd) = new knot_diagram (mt_link (n, alt, k));
	else
	  {
	    fprintf (stderr, "knot_parser: no such MT link `%d%c%d'\n", 
		     n, alt ? 'a' : 'n', k);
	    exit (EXIT_FAILURE);
	  }
      }
    break;

  case 16:
/* Line 661 of lalr1.cc  */
#line 145 "knot_parser/knot_parser.yy"
    { (yyval.kd) = new knot_diagram (planar_diagram ("<parsed>", *(yysemantic_stack_[(4) - (3)].int_vec2))); }
    break;

  case 17:
/* Line 661 of lalr1.cc  */
#line 147 "knot_parser/knot_parser.yy"
    { (yyval.kd) = new knot_diagram (planar_diagram ("<parsed>", *(yysemantic_stack_[(4) - (3)].int_vec2))); }
    break;

  case 18:
/* Line 661 of lalr1.cc  */
#line 152 "knot_parser/knot_parser.yy"
    {
	basedvector<basedvector<int, 1>, 1> even_labels (1);
	even_labels[1] = *(yysemantic_stack_[(4) - (3)].int_vec);
	(yyval.kd) = new knot_diagram (dt_code ("<parsed>", even_labels));
      }
    break;

  case 19:
/* Line 661 of lalr1.cc  */
#line 158 "knot_parser/knot_parser.yy"
    { (yyval.kd) = new knot_diagram (dt_code ("<parsed>", *(yysemantic_stack_[(4) - (3)].int_vec2))); }
    break;

  case 20:
/* Line 661 of lalr1.cc  */
#line 160 "knot_parser/knot_parser.yy"
    { (yyval.kd) = new knot_diagram (dt_code ("<parsed>", (yysemantic_stack_[(4) - (3)].string))); }
    break;

  case 21:
/* Line 661 of lalr1.cc  */
#line 165 "knot_parser/knot_parser.yy"
    { (yyval.kd) = new knot_diagram (torus_knot ((yysemantic_stack_[(6) - (3)].integer), (yysemantic_stack_[(6) - (5)].integer))); }
    break;

  case 22:
/* Line 661 of lalr1.cc  */
#line 170 "knot_parser/knot_parser.yy"
    { (yyval.kd) = new knot_diagram (braid ((yysemantic_stack_[(6) - (3)].integer), *(yysemantic_stack_[(6) - (5)].int_vec))); }
    break;

  case 23:
/* Line 661 of lalr1.cc  */
#line 175 "knot_parser/knot_parser.yy"
    {
	unsigned unknot_ar[1][4] = {
	  { 2, 1, 3, 4, },
	};
	(yyval.kd) = new knot_diagram ("U", 1, unknot_ar);
      }
    break;

  case 26:
/* Line 661 of lalr1.cc  */
#line 190 "knot_parser/knot_parser.yy"
    {
	basedvector<basedvector<int, 1>, 1> *v
	  = new basedvector<basedvector<int, 1>, 1> ();
	v->append (*(yysemantic_stack_[(1) - (1)].int_vec));
	(yyval.int_vec2) = v;
      }
    break;

  case 27:
/* Line 661 of lalr1.cc  */
#line 197 "knot_parser/knot_parser.yy"
    { 
	basedvector<basedvector<int, 1>, 1> *v = (yysemantic_stack_[(3) - (1)].int_vec2);
	v->append (*(yysemantic_stack_[(3) - (3)].int_vec));
	(yyval.int_vec2) = v;
      }
    break;

  case 28:
/* Line 661 of lalr1.cc  */
#line 206 "knot_parser/knot_parser.yy"
    { (yyval.int_vec) = (yysemantic_stack_[(3) - (2)].int_vec); }
    break;

  case 29:
/* Line 661 of lalr1.cc  */
#line 208 "knot_parser/knot_parser.yy"
    { (yyval.int_vec) = (yysemantic_stack_[(3) - (2)].int_vec); }
    break;

  case 30:
/* Line 661 of lalr1.cc  */
#line 213 "knot_parser/knot_parser.yy"
    {
	basedvector<int, 1> *v =
	  new basedvector<int, 1> ();
	v->append ((yysemantic_stack_[(1) - (1)].integer));
	(yyval.int_vec) = v;
      }
    break;

  case 31:
/* Line 661 of lalr1.cc  */
#line 220 "knot_parser/knot_parser.yy"
    {
	basedvector<int, 1> *v = (yysemantic_stack_[(3) - (1)].int_vec);
	v->append ((yysemantic_stack_[(3) - (3)].integer));
	(yyval.int_vec) = v;
      }
    break;

  case 32:
/* Line 661 of lalr1.cc  */
#line 229 "knot_parser/knot_parser.yy"
    {
	basedvector<basedvector<int, 1>, 1> *v
	  = new basedvector<basedvector<int, 1>, 1> ();
	v->append (*(yysemantic_stack_[(1) - (1)].int_vec));
	(yyval.int_vec2) = v;
      }
    break;

  case 33:
/* Line 661 of lalr1.cc  */
#line 236 "knot_parser/knot_parser.yy"
    { 
	basedvector<basedvector<int, 1>, 1> *v = (yysemantic_stack_[(3) - (1)].int_vec2);
	v->append (*(yysemantic_stack_[(3) - (3)].int_vec));
	(yyval.int_vec2) = v;
      }
    break;

  case 34:
/* Line 661 of lalr1.cc  */
#line 245 "knot_parser/knot_parser.yy"
    {
	basedvector<int, 1> *v
	  = new basedvector<int, 1> ();
	v->append ((yysemantic_stack_[(10) - (3)].integer));
	v->append ((yysemantic_stack_[(10) - (5)].integer));
	v->append ((yysemantic_stack_[(10) - (7)].integer));
	v->append ((yysemantic_stack_[(10) - (9)].integer));
	(yyval.int_vec) = v;
      }
    break;


/* Line 661 of lalr1.cc  */
#line 646 "knot_parser/knot_parser.cc"
	default:
          break;
      }
    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (!yy_pact_value_is_default_ (yyn))
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  knot_parser::yysyntax_error_ (int, int)
  {
    return YY_("syntax error");
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char knot_parser::yypact_ninf_ = -39;
  const signed char
  knot_parser::yypact_[] =
  {
        -1,    10,    -9,    25,    26,    37,     9,   -39,    43,    -1,
     -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,
     -39,    41,    42,    -7,    -3,    44,    24,    45,   -39,    -1,
     -39,   -39,    30,    46,    46,     7,   -39,    11,   -39,   -39,
      33,    12,    15,    -4,    48,    34,    49,    16,    -2,   -39,
       0,   -39,    47,   -39,   -39,   -39,    51,     0,   -39,    53,
      38,   -39,   -39,   -39,   -39,   -39,    50,    39,    55,   -39,
     -39,    52,    57,    54,    58,    59,   -39
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  knot_parser::yydefact_[] =
  {
         0,     0,     0,     0,     0,     0,     0,    23,     0,     2,
       3,     5,     6,     7,     8,     9,    10,    12,    11,    24,
      25,     0,     0,     0,     0,     0,     0,     0,     1,     4,
      13,    14,     0,     0,     0,     0,    26,     0,    32,    30,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    16,
       0,    17,     0,    20,    19,    18,     0,     0,    15,     0,
       0,    29,    28,    27,    33,    31,     0,     0,     0,    22,
      21,     0,     0,     0,     0,     0,    34
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  knot_parser::yypgoto_[] =
  {
       -39,   -39,    62,   -39,   -39,   -39,   -39,   -39,   -39,   -39,
     -39,   -39,    40,    56,   -38,     5,   -39,    13
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  knot_parser::yydefgoto_[] =
  {
        -1,     8,    29,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    22,    35,    36,    42,    37,    38
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char knot_parser::yytable_ninf_ = -1;
  const unsigned char
  knot_parser::yytable_[] =
  {
        39,    32,     1,     2,     3,     4,     5,    23,     6,    33,
       7,    40,    63,    33,    34,    57,    33,    56,    34,    66,
      62,    34,    19,    20,    49,    21,    50,    27,    51,    54,
      52,    50,    55,    61,    56,    56,    19,    20,    47,    48,
      26,    24,    25,    28,    30,    31,    46,    43,    45,    39,
      53,    58,    60,    59,    65,    32,    67,    68,    71,    70,
      73,    75,     9,     0,     0,    64,    44,    69,     0,     0,
       0,    72,     0,    74,     0,     0,    76,     0,     0,     0,
      41
  };

  /* YYCHECK.  */
  const signed char
  knot_parser::yycheck_[] =
  {
         3,     8,     3,     4,     5,     6,     7,    16,     9,    16,
      11,    14,    50,    16,    21,    19,    16,    19,    21,    57,
      22,    21,    12,    13,    17,    15,    19,    18,    17,    17,
      19,    19,    17,    17,    19,    19,    12,    13,    33,    34,
       3,    16,    16,     0,     3,     3,    16,     3,     3,     3,
      17,     3,     3,    19,     3,     8,     3,    19,     3,    20,
       3,     3,     0,    -1,    -1,    52,    26,    17,    -1,    -1,
      -1,    19,    -1,    19,    -1,    -1,    17,    -1,    -1,    -1,
      24
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  knot_parser::yystos_[] =
  {
         0,     3,     4,     5,     6,     7,     9,    11,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    12,
      13,    15,    35,    16,    16,    16,     3,    18,     0,    25,
       3,     3,     8,    16,    21,    36,    37,    39,    40,     3,
      14,    36,    38,     3,    35,     3,    16,    38,    38,    17,
      19,    17,    19,    17,    17,    17,    19,    19,     3,    19,
       3,    17,    22,    37,    40,     3,    37,     3,    19,    17,
      20,     3,    19,     3,    19,     3,    17
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  knot_parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,    95,    91,    93,    40,    44,
      41,   123,   125
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  knot_parser::yyr1_[] =
  {
         0,    23,    24,    25,    25,    26,    26,    26,    26,    26,
      26,    26,    26,    27,    28,    29,    30,    30,    31,    31,
      31,    32,    33,    34,    35,    35,    36,    36,    37,    37,
      38,    38,    39,    39,    40
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  knot_parser::yyr2_[] =
  {
         0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     4,     4,     4,     4,     4,
       4,     6,     6,     1,     1,     1,     1,     3,     3,     3,
       1,     3,     1,     3,    10
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const knot_parser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "INT", "PD", "DT", "BR",
  "LINK", "X", "T", "U", "UNKNOT", "ALT", "NONALT", "STRING", "'_'", "'['",
  "']'", "'('", "','", "')'", "'{'", "'}'", "$accept", "entry", "knot",
  "knot_1", "rolfsen_knot", "htw_knot", "mt_link", "planar_diagram", "dt",
  "torus_link", "braid", "unknot", "alt_spec", "int_vec2", "int_vec",
  "int_vec_1", "crossing_vec", "crossing", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const knot_parser::rhs_number_type
  knot_parser::yyrhs_[] =
  {
        24,     0,    -1,    25,    -1,    26,    -1,    25,    25,    -1,
      27,    -1,    28,    -1,    29,    -1,    30,    -1,    31,    -1,
      32,    -1,    34,    -1,    33,    -1,     3,    15,     3,    -1,
       3,    35,     3,    -1,     7,     3,    35,     3,    -1,     4,
      16,    36,    17,    -1,     4,    16,    39,    17,    -1,     5,
      16,    38,    17,    -1,     5,    16,    36,    17,    -1,     5,
      16,    14,    17,    -1,     9,    18,     3,    19,     3,    20,
      -1,     6,    16,     3,    19,    37,    17,    -1,    11,    -1,
      12,    -1,    13,    -1,    37,    -1,    36,    19,    37,    -1,
      21,    38,    22,    -1,    16,    38,    17,    -1,     3,    -1,
      38,    19,     3,    -1,    40,    -1,    39,    19,    40,    -1,
       8,    16,     3,    19,     3,    19,     3,    19,     3,    17,
      -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  knot_parser::yyprhs_[] =
  {
         0,     0,     3,     5,     7,    10,    12,    14,    16,    18,
      20,    22,    24,    26,    30,    34,    39,    44,    49,    54,
      59,    64,    71,    78,    80,    82,    84,    86,    90,    94,
      98,   100,   104,   106,   110
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  knot_parser::yyrline_[] =
  {
         0,    59,    59,    67,    68,    77,    78,    79,    80,    81,
      82,    83,    84,    88,   106,   125,   144,   146,   151,   157,
     159,   164,   169,   174,   184,   185,   189,   196,   205,   207,
     212,   219,   228,   235,   244
  };

  // Print the state stack on the debug stream.
  void
  knot_parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  knot_parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  knot_parser::token_number_type
  knot_parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      18,    20,     2,     2,    19,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    16,     2,    17,     2,    15,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    21,     2,    22,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int knot_parser::yyeof_ = 0;
  const int knot_parser::yylast_ = 80;
  const int knot_parser::yynnts_ = 18;
  const int knot_parser::yyempty_ = -2;
  const int knot_parser::yyfinal_ = 28;
  const int knot_parser::yyterror_ = 1;
  const int knot_parser::yyerrcode_ = 256;
  const int knot_parser::yyntokens_ = 23;

  const unsigned int knot_parser::yyuser_token_number_max_ = 269;
  const knot_parser::token_number_type knot_parser::yyundef_token_ = 2;


} // yy
/* Line 1106 of lalr1.cc  */
#line 1099 "knot_parser/knot_parser.cc"
/* Line 1107 of lalr1.cc  */
#line 255 "knot_parser/knot_parser.yy"


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
