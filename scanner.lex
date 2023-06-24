%{

/* declarations section */
#include <stdio.h>
#include "parser.tab.hpp"
#include "hw3_output.hpp"

%}

%option yylineno
%option noyywrap

digit   		 ([0-9])
positive_digit   ([1-9])
letter  		 ([a-zA-Z])
whitespace		 ([\r\t\n ])

%%

{whitespace}                                    ;
void                                            {yylval = new VoidType();           return VOID;}
int                                             {yylval = new IntType();            return INT;}
byte                                            {yylval = new ByteType();           return BYTE;}
b                                               {yylval = new GenericType();        return B;}
bool                                            {yylval = new BoolType();           return BOOL;}
override                                        {yylval = new OverrideType();       return OVERRIDE;}
and                                             {yylval = new GenericType();        return AND;}
or                                              {yylval = new GenericType();        return OR;}
not                                             {yylval = new GenericType();        return NOT;}
true                                            {yylval = new BoolType();           return TRUE;}
false                                           {yylval = new BoolType();           return FALSE;}
return                                          {yylval = new GenericType();        return RETURN;}
if                                              {yylval = new GenericType();        return IF;}
else                                            {yylval = new GenericType();        return ELSE;}
while                                           {yylval = new GenericType();        return WHILE;}
break                                           {yylval = new GenericType();        return BREAK;}
continue                                        {yylval = new GenericType();        return CONTINUE;}
;                                               {yylval = new GenericType();        return SC;}
,                                               {yylval = new GenericType();        return COMMA;}
\(                                              {yylval = new GenericType();        return LPAREN;}
\)                                              {yylval = new GenericType();        return RPAREN;}
\{                                              {yylval = new GenericType();        return LBRACE;}
\}                                              {yylval = new GenericType();        return RBRACE;}
=                                               {yylval = new GenericType();        return ASSIGN;}
==|!=                                           {yylval = new GenericType();        return EQUALITY;}
([<>]=?)                                        {yylval = new GenericType();        return RELATIONAL;}
\*|\/                                           {yylval = new GenericType();        return MULTIPLICATIVE;}
\+|-                                            {yylval = new GenericType();        return ADDITIVE;}
({letter})({letter}|{digit})*                   {yylval = new IDType(yytext);       return ID;}
{positive_digit}({digit})*|0                    {yylval = new NumType(yytext);      return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"                   {yylval = new StringType(yytext);   return STRING;}
\/\/[^\r\n]*[ \r|\n|\r\n]?                      ;
.		                                        {output::errorLex(yylineno); exit(1);}

%%

/* user code section */
