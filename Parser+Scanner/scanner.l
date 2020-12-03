
%option outfile="scanner.c"
%option noyywrap
%option nounput
%option noinput
%option yylineno

%{

#include "types.h"
#include "tables.h"
#include "ast.h"
#include "parser.h"

void yyerror(const char *s);

#define process_token(type) return type
extern StrTable *st;
extern char* id;
extern int num;
%}

number      [0-9]+
identifier  [a-zA-Z][0-9a-zA-Z]*
string      \"[^"]*\"
ignore      [ \t\n]+

/* Single line comments */
SLC         "//"[^\n]*\n
/* Multi-line comments */
%x          MLC

%%

"/*"            { BEGIN(MLC); } /* Enter special state. */
<MLC>"*/"       { BEGIN(INITIAL); } /* Go back to normal state. */
<MLC>.|\n       { } /* Ignore everything that is inside a comment. */

{ignore}        { } /* Eat up whitespace. */
{SLC}           { }

"else"          { process_token(ELSE); }
"if"            { process_token(IF); }
"input"         { process_token(INPUT); }
"int"           { process_token(INT); }
"output"        { process_token(OUTPUT); }
"return"        { process_token(RETURN); }
"void"          { process_token(VOID); }
"while"         { process_token(WHILE); }
"write"         { process_token(WRITE); }

"+"             { process_token(PLUS); }
"-"             { process_token(MINUS); }
"*"             { process_token(TIMES); }
"/"             { process_token(OVER); }

"<"             { process_token(LT); }
"<="            { process_token(LE); }
">"             { process_token(GT); }
">="            { process_token(GE); }
"=="            { process_token(EQ); }
"!="            { process_token(NEQ); }

"="             { process_token(ASSIGN); }

";"             { process_token(SEMI); }
","             { process_token(COMMA); }
"("             { process_token(LPAREN); }
")"             { process_token(RPAREN); }
"["             { process_token(LBRACK); }
"]"             { process_token(RBRACK); }
"{"             { process_token(LBRACE); }
"}"             { process_token(RBRACE); }

{number}        { num = atoi(yytext); process_token(NUM); }
{identifier}    { id = strdup(yytext); process_token(ID); }
{string}        { add_string(st, yytext); process_token(STRING); }

                /* Be sure to keep this as the last rule */
.               { printf("SCANNING ERROR (%d): Unknown symbol %s\n", yylineno, yytext);
                  exit(1); }

%%
