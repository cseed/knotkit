
%{
#include "knot_parser.hh"

#define YY_DECL \
  yy::knot_parser::token_type knot_yylex (yy::knot_parser::semantic_type *yylval)

#define yyterminate() return token::END

typedef yy::knot_parser::token token;
typedef yy::knot_parser::token_type token_type;
%}

%option noyywrap nounput batch prefix="knot_yy"

%%

-?[0-9]+  { 
   yylval->integer = atoi (yytext);
   return token::INT;
 }

[a-zA-Z]+ { 
  if (!strcmp (yytext, "a"))
    {
      yylval->alternating = 1;
      return token::ALT;
    }
  else if (!strcmp (yytext, "L"))
    return token::LINK;
  else if (!strcmp (yytext, "n"))
    {
      yylval->alternating = 0;
      return token::NONALT;
    }
  else if (!strcmp (yytext, "T"))
    return token::T;
  else if (!strcmp (yytext, "U")
	   || !strcmp (yytext, "unknot"))
    return token::UNKNOT;
  else if (!strcmp (yytext, "X"))
    return token::X;
  else if (!strcmp (yytext, "BR"))
    return token::BR;
  else if (!strcmp (yytext, "PD"))
    return token::PD;
  else if (!strcmp (yytext, "DT")
	   || !strcmp (yytext, "DTCode"))
    return token::DT;
  else if (!strcmp (yytext, "BR"))
    return token::BR;
  else
    {
      yylval->string = strdup (yytext);
      return token::STRING;
    }
}

[\t\r\n ] /* ignore */

. { return token_type (yytext[0]); }

%%

void
knot_scan_string (const char *s)
{
  knot_yy_scan_string (s);
}
