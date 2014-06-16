/* A Bison parser, made by GNU Bison 2.7.  */

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

/* Line 279 of lalr1.cc  */
#line 38 "rd_parser/rd_parser.cc"


#include "rd_parser.hh"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 46 "rd_parser/rd_parser.cc"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 15 "rd_parser/rd_parser.yy"

#define YY_DECL \
  yy::rd_parser::token_type rd_yylex (yy::rd_parser::semantic_type *yylval)
YY_DECL;

#define CPT_MASK    0x0ffff
#define CPT_MARKED  0x10000
#define CPT_MARKED2 0x20000

/* Line 286 of lalr1.cc  */
#line 32 "rd_parser/rd_parser.yy"

#define yylex rd_yylex


/* Line 286 of lalr1.cc  */
#line 66 "rd_parser/rd_parser.cc"


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
# define YY_SYMBOL_PRINT(Title, Type, Value, Location) YYUSE(Type)
# define YY_REDUCE_PRINT(Rule)        static_cast<void>(0)
# define YY_STACK_PRINT()             static_cast<void>(0)

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
/* Line 353 of lalr1.cc  */
#line 161 "rd_parser/rd_parser.cc"

  /// Build a parser object.
  rd_parser::rd_parser (resolution_diagram &parsed_rd_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      parsed_rd (parsed_rd_yyarg)
  {
  }

  rd_parser::~rd_parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  rd_parser::yy_symbol_value_print_ (int yytype,
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
  rd_parser::yy_symbol_print_ (int yytype,
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
  rd_parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  rd_parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  rd_parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  rd_parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  rd_parser::debug_level_type
  rd_parser::debug_level () const
  {
    return yydebug_;
  }

  void
  rd_parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  rd_parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  rd_parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  rd_parser::parse ()
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

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
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

    // Compute the default @$.
    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }

    // Perform the reduction.
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
          case 2:
/* Line 670 of lalr1.cc  */
#line 54 "rd_parser/rd_parser.yy"
    {
	unsigned n_crossings = (yysemantic_stack_[(1) - (1)].integer);
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
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 88 "rd_parser/rd_parser.yy"
    {
	unsigned s = ++ parsed_rd.n_starting_circles;
	
	const basedvector<int, 1> &v = *(yysemantic_stack_[(3) - (2)].cpt_vec);
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
    break;

  case 12:
/* Line 670 of lalr1.cc  */
#line 126 "rd_parser/rd_parser.yy"
    {
	basedvector<int, 1> *v =
	  new basedvector<int, 1> ();
	v->append ((yysemantic_stack_[(1) - (1)].cpt));
	(yyval.cpt_vec) = v;
      }
    break;

  case 13:
/* Line 670 of lalr1.cc  */
#line 133 "rd_parser/rd_parser.yy"
    {
	basedvector<int, 1> *v = (yysemantic_stack_[(3) - (1)].cpt_vec);
	v->append ((yysemantic_stack_[(3) - (3)].cpt));
	(yyval.cpt_vec) = v;
      }
    break;

  case 15:
/* Line 670 of lalr1.cc  */
#line 143 "rd_parser/rd_parser.yy"
    { (yyval.cpt) = (yysemantic_stack_[(2) - (1)].cpt) | CPT_MARKED; }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 145 "rd_parser/rd_parser.yy"
    { (yyval.cpt) = (yysemantic_stack_[(2) - (1)].cpt) | CPT_MARKED2; }
    break;

  case 17:
/* Line 670 of lalr1.cc  */
#line 147 "rd_parser/rd_parser.yy"
    { (yyval.cpt) = (yysemantic_stack_[(3) - (1)].cpt) | CPT_MARKED | CPT_MARKED2; }
    break;

  case 18:
/* Line 670 of lalr1.cc  */
#line 149 "rd_parser/rd_parser.yy"
    { (yyval.cpt) = (yysemantic_stack_[(3) - (1)].cpt) | CPT_MARKED | CPT_MARKED2; }
    break;

  case 19:
/* Line 670 of lalr1.cc  */
#line 154 "rd_parser/rd_parser.yy"
    {
	if ((yysemantic_stack_[(2) - (2)].inside))
	  parsed_rd.cpt_inside.push ((yysemantic_stack_[(2) - (1)].cpt));
	(yyval.cpt) = (yysemantic_stack_[(2) - (1)].cpt);
      }
    break;

  case 20:
/* Line 670 of lalr1.cc  */
#line 163 "rd_parser/rd_parser.yy"
    { (yyval.cpt) = parsed_rd.crossing_to_cpt ((yysemantic_stack_[(2) - (2)].integer)); }
    break;

  case 21:
/* Line 670 of lalr1.cc  */
#line 165 "rd_parser/rd_parser.yy"
    { (yyval.cpt) = parsed_rd.crossing_from_cpt ((yysemantic_stack_[(2) - (2)].integer)); }
    break;


/* Line 670 of lalr1.cc  */
#line 556 "rd_parser/rd_parser.cc"
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
    while (1 < yystate_stack_.height ())
      {
        yydestruct_ ("Cleanup: popping",
                     yystos_[yystate_stack_[0]],
                     &yysemantic_stack_[0],
                     &yylocation_stack_[0]);
        yypop_ ();
      }

    return yyresult;
    }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (yychar != yyempty_)
          {
            /* Make sure we have latest lookahead translation.  See
               comments at user semantic actions for why this is
               necessary.  */
            yytoken = yytranslate_ (yychar);
            yydestruct_ (YY_NULL, yytoken, &yylval, &yylloc);
          }

        while (1 < yystate_stack_.height ())
          {
            yydestruct_ (YY_NULL,
                         yystos_[yystate_stack_[0]],
                         &yysemantic_stack_[0],
                         &yylocation_stack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  // Generate an error message.
  std::string
  rd_parser::yysyntax_error_ (int, int)
  {
    return YY_("syntax error");
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char rd_parser::yypact_ninf_ = -12;
  const signed char
  rd_parser::yypact_[] =
  {
        -2,   -12,    11,     4,   -12,     5,    -1,    -3,   -12,    10,
      12,    -8,   -12,    -5,     3,    13,   -12,   -12,   -12,   -12,
      -1,   -12,     6,     8,   -12,   -12,   -12,     7,   -12,     9,
     -12,   -12,   -12,    18,    13,   -12,   -12
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  rd_parser::yydefact_[] =
  {
         0,     2,     0,     0,     1,     0,     0,     7,     4,     0,
       0,     0,    12,    14,     0,     0,     5,     3,    20,    21,
       0,     6,    15,    16,    22,    23,    19,     0,     8,     9,
      13,    18,    17,     0,     0,    11,    10
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  rd_parser::yypgoto_[] =
  {
       -12,   -12,   -12,   -12,    15,   -12,   -11,   -12,   -12,    14,
     -12,   -12,   -12
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  rd_parser::yydefgoto_[] =
  {
        -1,     2,     3,     7,     8,    17,    28,    29,    11,    12,
      13,    14,    26
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char rd_parser::yytable_ninf_ = -1;
  const unsigned char
  rd_parser::yytable_[] =
  {
        20,     1,    21,     9,    10,    15,     6,    22,    23,    24,
      25,     4,     5,    18,     6,    19,    27,    34,    33,    31,
      32,    35,    16,    36,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    30
  };

  /* YYCHECK.  */
  const signed char
  rd_parser::yycheck_[] =
  {
         8,     3,    10,     4,     5,     8,     9,    12,    13,     6,
       7,     0,     8,     3,     9,     3,     3,     8,    11,    13,
      12,     3,     7,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    20
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  rd_parser::yystos_[] =
  {
         0,     3,    15,    16,     0,     8,     9,    17,    18,     4,
       5,    22,    23,    24,    25,     8,    18,    19,     3,     3,
       8,    10,    12,    13,     6,     7,    26,     3,    20,    21,
      23,    13,    12,    11,     8,     3,    20
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  rd_parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,    44,    40,
      41,   124,    42,    43
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  rd_parser::yyr1_[] =
  {
         0,    14,    16,    15,    17,    17,    18,    19,    19,    20,
      20,    21,    22,    22,    23,    23,    23,    23,    23,    24,
      25,    25,    26,    26
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  rd_parser::yyr2_[] =
  {
         0,     2,     0,     5,     1,     2,     3,     0,     2,     1,
       3,     3,     1,     3,     1,     2,     2,     3,     3,     2,
       2,     2,     1,     1
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const rd_parser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "INT", "H", "T", "IN", "OUT",
  "','", "'('", "')'", "'|'", "'*'", "'+'", "$accept",
  "resolution_diagram", "$@1", "starting_circles", "starting_circle",
  "dual_crossings_opt", "dual_crossings", "dual_crossing",
  "crossing_points", "crossing_point", "crossing_point_1",
  "crossing_point_spec", "inout", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const rd_parser::rhs_number_type
  rd_parser::yyrhs_[] =
  {
        15,     0,    -1,    -1,     3,    16,     8,    17,    19,    -1,
      18,    -1,    17,    18,    -1,     9,    22,    10,    -1,    -1,
       8,    20,    -1,    21,    -1,    21,     8,    20,    -1,     3,
      11,     3,    -1,    23,    -1,    22,     8,    23,    -1,    24,
      -1,    24,    12,    -1,    24,    13,    -1,    24,    13,    12,
      -1,    24,    12,    13,    -1,    25,    26,    -1,     4,     3,
      -1,     5,     3,    -1,     6,    -1,     7,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  rd_parser::yyprhs_[] =
  {
         0,     0,     3,     4,    10,    12,    15,    19,    20,    23,
      25,    29,    33,    35,    39,    41,    44,    47,    51,    55,
      58,    61,    64,    66
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  rd_parser::yyrline_[] =
  {
         0,    54,    54,    53,    82,    83,    87,   111,   113,   116,
     117,   121,   125,   132,   141,   142,   144,   146,   148,   153,
     162,   164,   169,   170
  };

  // Print the state stack on the debug stream.
  void
  rd_parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  rd_parser::yy_reduce_print_ (int yyrule)
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
  rd_parser::token_number_type
  rd_parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       9,    10,    12,    13,     8,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    11,     2,     2,     2,     2,     2,
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
       5,     6,     7
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int rd_parser::yyeof_ = 0;
  const int rd_parser::yylast_ = 34;
  const int rd_parser::yynnts_ = 13;
  const int rd_parser::yyempty_ = -2;
  const int rd_parser::yyfinal_ = 4;
  const int rd_parser::yyterror_ = 1;
  const int rd_parser::yyerrcode_ = 256;
  const int rd_parser::yyntokens_ = 14;

  const unsigned int rd_parser::yyuser_token_number_max_ = 262;
  const rd_parser::token_number_type rd_parser::yyundef_token_ = 2;


} // yy
/* Line 1141 of lalr1.cc  */
#line 1002 "rd_parser/rd_parser.cc"
/* Line 1142 of lalr1.cc  */
#line 173 "rd_parser/rd_parser.yy"


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
