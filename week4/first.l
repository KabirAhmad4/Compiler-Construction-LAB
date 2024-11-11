%{
#include <stdio.h>
#include <ctype.h>

// Token Definitions
#define KEYWORD 1
#define IDENTIFIER 2
#define NUMBER 3
#define OPERATOR 4
#define WHITESPACE 5
#define UNKNOWN 6
%}

%%
/* Keywords */
if|else|return             { printf("Keyword: %s\n", yytext); return KEYWORD; }

/* Identifiers (variable names) - A sequence starting with a letter or underscore, followed by letters, digits, or underscores */
[a-zA-Z_][a-zA-Z0-9_]*     { printf("Identifier: %s\n", yytext); return IDENTIFIER; }

/* Numbers - Integer and Floating-point numbers */
[0-9]+                     { printf("Number: %s\n", yytext); return NUMBER; }
[0-9]+"."[0-9]+            { printf("Number: %s\n", yytext); return NUMBER; }

/* Operators (e.g. arithmetic, logical) */
"+"|"-"|"*"|"/"|"="         { printf("Operator: %s\n", yytext); return OPERATOR; }

/* Whitespace (Spaces, Tabs, Newlines) */
[ \t\n]+                   { /* Ignore whitespace */ return WHITESPACE; }

/* Any other characters */
.                          { printf("Unknown character: %s\n", yytext); return UNKNOWN; }
%%

int main(void) {
    printf("Enter the input to be tokenized:\n");
    yylex();  // Call the lexical analyzer
    return 0;
}

int yywrap() {
    return 1;
}
