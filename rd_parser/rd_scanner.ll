
%{
#include "rd_parser.hh"

#define YY_DECL \
  yy::rd_parser::token_type rd_yylex (yy::rd_parser::semantic_type *yylval)

#define yyterminate() return token::END

typedef yy::rd_parser::token token;
typedef yy::rd_parser::token_type token_type;
%}

%option noyywrap nounput batch prefix="rd_yy"

%%

-?[0-9]+  { 
   yylval->integer = atoi (yytext);
   return token::INT;
 }

[a-zA-Z]+ { 
  if (!strcmp (yytext, "h"))
    return token::H;
  else if (!strcmp (yytext, "t"))
    return token::T;
  else if (!strcmp (yytext, "in"))
    {
      yylval->inside = 1;
      return token::IN;
    }
  else if (!strcmp (yytext, "out"))
    {
      yylval->inside = 0;
      return token::OUT;
    }
  else
    {
      fprintf (stderr, "rd_parser: unknown identifier `%s'\n", yytext);
      exit (EXIT_FAILURE);
    }
}

[\t\r\n ] /* ignore */

. { return token_type (yytext[0]); }

%%

void
rd_scan_string (const char *s)
{
  rd_yy_scan_string (s);
}
